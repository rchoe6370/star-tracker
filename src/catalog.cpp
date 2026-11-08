#include "startracker/catalog.hpp"
#include <stdexcept>

namespace st {

Catalog Catalog::load(const std::string& /*csv_path*/, double /*max_magnitude*/) {
  // TODO (Phase 0): open file, skip header, parse rows,
  // filter by magnitude, convert RA/Dec (deg->rad) with radec_to_unit.
  throw std::logic_error("Catalog::load not implemented");
}

const Star* Catalog::find(std::int32_t /*id*/) const {
  // TODO: linear scan is fine for now; build an index later if needed.
  throw std::logic_error("Catalog::find not implemented");
}

}  // namespace st
