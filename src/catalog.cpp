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
double parse_double(const std::string& text, const char* name,
                    const std::string& path, std::size_t line) {
  try {
    std::size_t used = 0;
    const double v = std::stod(text, &used);
    if (used != text.size())
      fail(path, line, std::string("trailing junk in ") + name + ": '" + text + "'");
    return v;
  } catch (const std::invalid_argument&) {
    fail(path, line, std::string("bad ") + name + " = '" + text + "'");
  } catch (const std::out_of_range&) {
    fail(path, line, std::string(name) + " out of range: '" + text + "'");
  }
}
int parse_int(const std::string& text, const char* name,
              const std::string& path, std::size_t line) {
  try {
    std::size_t used = 0;
    const int v = std::stoi(text, &used);
    if (used != text.size()) fail(path, line, std::string("trailing junk in ") + name + ": '" + text + "'");
    return v;
  } catch (const std::invalid_argument&) {
    fail(path, line, std::string("bad ") + name + " = '" + text + "'");
  } catch (const std::out_of_range&) {
    fail(path, line, std::string(name) + " out of range: '" + text + "'");
  }
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
    for (auto& field : f)                        // 4 passes: split the row into fields
      if (!std::getline(ss, field, ',')) fail(csv_path, lineno, "expected 4 columns");

    Star s;                                      // inside the while, after the for
    s.id = parse_int(f[0], "star_id", csv_path, lineno);
    s.ra = deg2rad(parse_double(f[1], "ra_deg", csv_path, lineno));
    s.dec = deg2rad(parse_double(f[2], "dec_deg", csv_path, lineno));
    s.mag = parse_double(f[3], "mag", csv_path, lineno);
    if (s.mag > mag_limit) continue;             // skip faint stars
    s.dir_sky = radec_to_unit(s.ra, s.dec);
    cat.stars_.push_back(s);
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