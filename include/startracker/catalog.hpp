#pragma once
#include <string>
#include <vector>
#include "startracker/types.hpp"

namespace st {

constexpr double kDefaultMagLimit = 5.5;

// Reads data/catalog.csv 
// Stars are filtered by magnitude
class Catalog {
 public:
  static Catalog load(const std::string& csv_path, double mag_limit = kDefaultMagLimit);

  const std::vector<Star>& stars() const { return stars_; }
  std::size_t size() const { return stars_.size(); }

  // Returns nullptr if the ID is not in the catalog.
  const Star* find(std::int32_t id) const;

 private:
  std::vector<Star> stars_;
};

}  // namespace st
