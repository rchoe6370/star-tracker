#pragma once
#include <string>
#include <vector>
#include "startracker/types.hpp"

namespace st {

// Contract with python/prepare_catalog.py:
//   data/catalog.csv, header row required:
//   star_id,ra_deg,dec_deg,magnitude
// RA/Dec are converted to unit vectors here in C++ (decision: C++ side).
class Catalog {
 public:
  // Load from CSV; keep only stars with magnitude <= max_magnitude.
  // Throws std::runtime_error if the file cannot be read or is malformed.
  static Catalog load(const std::string& csv_path, double max_magnitude = 5.5);

  const std::vector<Star>& stars() const { return stars_; }
  std::size_t size() const { return stars_.size(); }

  // Returns nullptr if the ID is not in the catalog.
  const Star* find(std::int32_t id) const;

 private:
  std::vector<Star> stars_;
};

}  // namespace st
