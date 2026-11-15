#include "startracker/catalog.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "startracker/frames.hpp"

namespace st {

namespace {
[[noreturn]] void fail(const std::string& path, std::size_t line, const std::string& msg) {
  throw std::runtime_error(path + ":" + std::to_string(line) + ": " + msg);
}
}  // namespace

Catalog Catalog::load(const std::string& csv_path, double mag_limit) {
  std::ifstream in(csv_path);
  if (!in) throw std::runtime_error("cannot open catalog: " + csv_path);

  std::string line;
  std::size_t lineno = 0;

  if (!std::getline(in, line)) fail(csv_path, 1, "empty file");
  ++lineno;
  if (!line.empty() && line.back() == '\r') line.pop_back();
  if (line != "star_id,ra_deg,dec_deg,mag") {
    fail(csv_path, lineno, "unexpected header: '" + line + "'");
  }

  Catalog cat;
  while (std::getline(in, line)) {
    ++lineno;
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line.empty()) continue;

    std::stringstream ss(line);
    std::string f[4];
    for (auto& field : f) {
      if (!std::getline(ss, field, ',')) fail(csv_path, lineno, "expected 4 columns");

      try {
        Star s;
        s.id = std::stoi(f[0]);
        s.ra = deg2rad(std::stod(f[1]));
        s.dec = deg2rad(std::stod(f[2]));
        s.mag = std::stod(f[3]);
        if (s.mag > mag_limit) continue;
        s.dir_sky = radec_to_unit(s.ra, s.dec);
        cat.stars_.push_back(s);
      } catch (const std::exception&) {
        fail(csv_path, lineno, "could not parse numbers in '" + line + "'");
      }
    }
  }
  return cat;
}

const Star* Catalog::find(std::int32_t id) const {
  // Linear scan: ~3000 stars, fine for now. Build an index if profiling says so.
  for (const auto& s : stars_)
    if (s.id == id) return &s;
  return nullptr;
}

}  // namespace st