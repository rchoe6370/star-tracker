#include "startracker/frames.hpp"
#include <cmath>

namespace st {

Vec3 radec_to_unit(double ra_rad, double dec_rad) {
  const double c = std::cos(dec_rad);
  return Vec3(c * std::cos(ra_rad), c * std::sin(ra_rad), std::sin(dec_rad));
}

double angle_between(const Vec3& u, const Vec3& v) {
  // atan2(|u x v|, u . v) is accurate for tiny AND near-180 degree angles,
  // and never produces NaN, so no clamp is needed. (acos of a dot product
  // loses precision near 0, which matters at arcsecond level.)
  return std::atan2(u.cross(v).norm(), u.dot(v));
}

}  // namespace st