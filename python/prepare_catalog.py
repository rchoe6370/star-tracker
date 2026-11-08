"""BSC5P JSON (data/raw/) -> data/catalog.csv

Output columns (contract with C++): star_id,ra_deg,dec_deg,magnitude
  star_id : HR number (== "lineNumber" in the JSON)
  ra_deg  : J2000 right ascension in DEGREES (hours * 15)
  dec_deg : J2000 declination in degrees
  magnitude: visual magnitude

Usage:
  python python/prepare_catalog.py                  # uses the only .json in data/raw/
  python python/prepare_catalog.py path/to/file.json --max-mag 6.5
"""
import argparse
import csv
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RAW_DIR = ROOT / "data" / "raw"
OUT = ROOT / "data" / "catalog.csv"


def _num(s: str):
    """Parse a numeric string; return None if blank."""
    s = (s or "").strip()
    return float(s) if s else None


def parse_star(rec: dict):
    """Return (hr, ra_deg, dec_deg, mag) or None if the record is unusable."""
    hr = (rec.get("lineNumber") or "").strip()
    h, m, s = (_num(rec.get(k)) for k in
               ("hoursRaJ2000", "minutesRaJ2000", "secondsRaJ2000"))
    d, dm, ds = (_num(rec.get(k)) for k in
                 ("degreesDecJ2000", "minutesDecJ2000", "secondsDecJ2000"))
    mag = _num(rec.get("visualMagnitude"))
    if not hr or None in (h, m, s, d, dm, ds, mag):
        return None
    ra_deg = (h + m / 60.0 + s / 3600.0) * 15.0
    dec_abs = d + dm / 60.0 + ds / 3600.0
    # Sign is its own field and applies to the whole value (handles "-00").
    dec_deg = -dec_abs if rec.get("signDecJ2000", "+").strip() == "-" else dec_abs
    return int(hr), ra_deg, dec_deg, mag


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("json_path", nargs="?", type=Path, default=None)
    ap.add_argument("--max-mag", type=float, default=6.5,
                    help="drop stars fainter than this (C++ filters further)")
    args = ap.parse_args()

    path = args.json_path
    if path is None:
        found = sorted(RAW_DIR.glob("*.json"))
        if len(found) != 1:
            raise SystemExit(f"Expected exactly one .json in {RAW_DIR}, found {len(found)}. "
                             "Pass the path explicitly.")
        path = found[0]

    with open(path, encoding="utf-8") as f:
        data = json.load(f)
    if isinstance(data, dict):  # tolerate a wrapper object
        data = next(v for v in data.values() if isinstance(v, list))

    rows, skipped, too_faint = [], 0, 0
    for rec in data:
        star = parse_star(rec)
        if star is None:
            skipped += 1
        elif star[3] > args.max_mag:
            too_faint += 1
        else:
            rows.append(star)
    rows.sort()

    OUT.parent.mkdir(parents=True, exist_ok=True)
    with open(OUT, "w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["star_id", "ra_deg", "dec_deg", "magnitude"])
        for hr, ra, dec, mag in rows:
            w.writerow([hr, f"{ra:.6f}", f"{dec:.6f}", f"{mag:.2f}"])

    print(f"Read {len(data)} records from {path.name}")
    print(f"  skipped (missing coords/mag): {skipped}")
    print(f"  dropped (mag > {args.max_mag}): {too_faint}")
    print(f"  wrote {len(rows)} stars to {OUT}")
    by_id = {r[0]: r for r in rows}
    if 424 in by_id:
        print(f"  Polaris check (HR 424): RA={by_id[424][1]:.4f} Dec={by_id[424][2]:.4f}")


if __name__ == "__main__":
    main()