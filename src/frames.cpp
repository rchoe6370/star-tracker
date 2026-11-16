#include "startracker/frames.hpp"
#include <cmath>
#include <algorithm>

namespace st {

namespace {
double wrap_2pi(double a) {
  a = std::fmod(a, 2.0 * kPi);
  return (a < 0.0) ? a + 2.0 * kPi : a;
}
} // namespace

Vec3 radec_to_unit(double ra_rad, double dec_rad) {
  const double c = std::cos(dec_rad);
  return Vec3(c * std::cos(ra_rad), c * std::sin(ra_rad), std::sin(dec_rad));
}

double angle_between(const Vec3& u, const Vec3& v) {
  return std::atan2(u.cross(v).norm(), u.dot(v));
}

Mat3 attitude_from_pointing(double ra_rad, double dec_rad, double pa_rad) {
  const double sr = std::sin(ra_rad);
  const double cr = std::cos(ra_rad);
  const double sd = std::sin(dec_rad);
  const double cd = std::cos(dec_rad);

  const Vec3 b(cd * cr, cd * sr, sd);
  const Vec3 n(-sd * cr, -sd * sr, cd);
  const Vec3 e(-sr, cr, 0.0);

  const Vec3 up = std::cos(pa_rad) * n + std::sin(pa_rad) * e;
  const Vec3 y_cam = -up;
  const Vec3 x_cam = b.cross(up);

  Mat3 A;
  A.row(0) = x_cam.transpose();
  A.row(1) = y_cam.transpose();
  A.row(2) = b.transpose();
  return A;
}

Mat3 random_attitude(std::mt19937_64& rng) {
  std::normal_distribution<double> gauss(0.0, 1.0);

  const double w = gauss(rng);
  const double x = gauss(rng);
  const double y = gauss(rng);
  const double z = gauss(rng);
  
  Quat q(w, x, y, z);
  q.normalize();
  return quaternion_to_matrix(q);
}

Quat matrix_to_quaternion(const Mat3& A_cam_from_sky) {
  const Mat3 A_sky_from_cam = A_cam_from_sky.transpose();

  Quat q(A_sky_from_cam);
  q.normalize();
  if (q.w() < 0.0) q.coeffs() = -q.coeffs();
  return q;
}

Mat3 quaternion_to_matrix(const Quat& q) {
  const Mat3 A_sky_from_cam = q.normalized().toRotationMatrix();
  return A_sky_from_cam.transpose();
}

Pointing attitude_to_radec_pa(const Mat3& A_cam_from_sky) {
  const Vec3 b = A_cam_from_sky.row(2).transpose();
  const Vec3 up = -A_cam_from_sky.row(1).transpose();

  const double ra = std::atan2(b.y(), b.x());
  const double dec = std::asin(std::clamp(b.z(), -1.0, 1.0));

  const double sr = std::sin(ra), cr = std::cos(ra);
  const double sd = std::sin(dec), cd = std::cos(dec);
  const Vec3 n(-sd * cr, -sd * sr, cd);
  const Vec3 e(-sr, cr, 0.0);
  const double pa = std::atan2(up.dot(e), up.dot(n));

  return Pointing{wrap_2pi(ra), dec, wrap_2pi(pa)};
}

}  // namespace st