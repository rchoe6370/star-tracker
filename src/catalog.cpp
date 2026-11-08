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

Catalog Catalog::load(const std::string& csv_path, double max_magnitude) {
  std::ifstream in(csv_path);
  if (!in) throw std::runtime_error("cannot open catalog: " + csv_path);

  std::string line;
  std::size_t lineno = 0;

  // Header must match the contract exactly.
  if (!std::getline(in, line)) fail(csv_path, 1, "empty file");
  ++lineno;
  if (!line.empty() && line.back() == '\r') line.pop_back();
  if (line != "star_id,ra_deg,dec_deg,magnitude")
    fail(csv_path, lineno, "unexpected header: '" + line + "'");

  Catalog cat;
  while (std::getline(in, line)) {
    ++lineno;
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line.empty()) continue;

    std::stringstream ss(line);
    std::string f[4];
    for (auto& field : f)
      if (!std::getline(ss, field, ',')) fail(csv_path, lineno, "expected 4 columns");

    try {
      const int id = std::stoi(f[0]);
      const double ra_deg = std::stod(f[1]);
      const double dec_deg = std::stod(f[2]);
      const double mag = std::stod(f[3]);
      if (mag > max_magnitude) continue;

      Star s;
      s.id = id;
      s.v_sky = radec_to_unit(deg2rad(ra_deg), deg2rad(dec_deg));
      s.magnitude = mag;
      cat.stars_.push_back(s);
    } catch (const std::exception&) {
      fail(csv_path, lineno, "could not parse numbers in '" + line + "'");
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