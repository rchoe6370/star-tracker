"""Raw BSC5 (from VizieR, saved in data/raw/) -> data/catalog.csv

Output columns (contract with C++): star_id,ra_deg,dec_deg,magnitude
- star_id: HR number
- RA must be in DEGREES (convert from hours if needed: deg = hours * 15)
- Keep magnitude <= 5.5 here or leave filtering to C++ (Catalog::load).
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RAW_DIR = ROOT / "data" / "raw"
OUT = ROOT / "data" / "catalog.csv"


def main() -> None:
    # TODO: inspect the downloaded file in RAW_DIR, parse it (pandas),
    # convert RA to degrees, drop rows missing RA/Dec/magnitude,
    # write OUT with a header row.
    raise NotImplementedError("fill in after downloading the catalog")


if __name__ == "__main__":
    main()
