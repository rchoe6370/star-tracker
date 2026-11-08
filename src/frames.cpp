#include "startracker/frames.hpp"
#include <algorithm>
#include <stdexcept>

namespace st {

Vec3 radec_to_unit(double /*ra_rad*/, double /*dec_rad*/) {
  // TODO (Phase 0): guide Section 3.2
  throw std::logic_error("radec_to_unit not implemented");
}

double angle_between(const Vec3& /*u*/, const Vec3& /*v*/) {
  // TODO (Phase 0): dot product, clamp to [-1, 1], acos
  throw std::logic_error("angle_between not implemented");
}

}  // namespace st
