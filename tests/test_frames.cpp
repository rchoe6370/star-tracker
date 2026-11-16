#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "startracker/frames.hpp"
#include <cmath>
#include <random>

using namespace st;
using Catch::Matchers::WithinAbs;

namespace {
double max_abs_diff(const Mat3& a, const Mat3& b) {
  return (a - b).cwiseAbs().maxCoeff();
}
} // namespace

TEST_CASE("radec_to_unit: known directions") {
  CHECK((radec_to_unit(0.0, 0.0) - Vec3(1, 0, 0)).norm() < kTolUnit);
  // Dec = +90 deg gives +z for any RA
  CHECK((radec_to_unit(deg2rad(123.0), deg2rad(90.0)) - Vec3(0, 0, 1)).norm() < kTolUnit);
  CHECK((radec_to_unit(deg2rad(90.0), 0.0) - Vec3(0, 1, 0)).norm() < kTolUnit);
}

TEST_CASE("radec_to_unit: unit length") {
  for (double ra = 0; ra < 360; ra += 17)
    for (double dec = -90; dec <= 90; dec += 7)
      CHECK_THAT(radec_to_unit(deg2rad(ra), deg2rad(dec)).norm(), WithinAbs(1.0, kTolUnit));
}

TEST_CASE("angle_between: edge cases") {
  CHECK_THAT(angle_between(Vec3::UnitX(), Vec3::UnitX()), WithinAbs(0.0, kTolUnit));
  CHECK_THAT(angle_between(Vec3::UnitX(), -Vec3::UnitX()), WithinAbs(kPi, kTolUnit));
  CHECK_THAT(angle_between(Vec3::UnitX(), Vec3::UnitY()), WithinAbs(kPi / 2, kTolUnit));

  // Tiny angle: acos(dot) would return 0 or NaN here, atan2 stays accurate.
  const Vec3 tiny = Vec3(1.0, 1e-12, 0.0).normalized();
  const double a = angle_between(Vec3::UnitX(), tiny);
  CHECK(!std::isnan(a));
  CHECK_THAT(a, WithinAbs(1e-12, 1e-15));
}

TEST_CASE("attitude_from_pointing: paper test at RA=0 Dec=0 pa=0") {
  Mat3 expected;
  expected << 0, -1, 0,
              0, 0, -1,
              1, 0, 0;
  CHECK(max_abs_diff(attitude_from_pointing(0.0, 0.0, 0.0), expected) < kTolUnit);
}

TEST_CASE("attitude_from_pointing: paper test at RA=90 Dec=0 pa=0") {
  Mat3 expected;
  expected << 1, 0, 0,
              0, 0, -1,
              0, 1, 0;
  CHECK(max_abs_diff(attitude_from_pointing(deg2rad(90.0), 0.0, 0.0), expected) < kTolUnit);
}

TEST_CASE("attitude_from_pointing: north is up and east is left") {
  const double eps = 0.01;  // rad
  for (double ra = 0.0; ra < 6.2; ra += 0.9) {
    for (double dec = -1.2; dec <= 1.2; dec += 0.4) {
      const Mat3 A = attitude_from_pointing(ra, dec, 0.0);

      // Boresight lands on the optical axis.
      const Vec3 c = A * radec_to_unit(ra, dec);
      CHECK((c - Vec3::UnitZ()).norm() < kTolUnit);

      // North of center: negative camera y means ABOVE center (y points down).
      CHECK((A * radec_to_unit(ra, dec + eps)).y() < 0.0);
      // East of center: negative camera x means LEFT of center.
      CHECK((A * radec_to_unit(ra + eps / std::cos(dec), dec)).x() < 0.0);
    }
  }
}

TEST_CASE("attitude_from_pointing: pa = 90 deg puts east up") {
  const double ra = 1.0, dec = 0.3, eps = 0.01;
  const Mat3 A = attitude_from_pointing(ra, dec, deg2rad(90.0));
  CHECK((A * radec_to_unit(ra + eps / std::cos(dec), dec)).y() < 0.0);  // east is up
}

TEST_CASE("random_attitude: valid rotations") {
  std::mt19937_64 rng(12345);
  for (int i = 0; i < 1000; ++i) {
    const Mat3 A = random_attitude(rng);
    CHECK((A * A.transpose() - Mat3::Identity()).cwiseAbs().maxCoeff() < kTolUnit);
    CHECK_THAT(A.determinant(), WithinAbs(1.0, kTolUnit));
  }
}

TEST_CASE("random_attitude: boresight is uniform on the sphere") {
  std::mt19937_64 rng(777);
  double sum_z = 0.0;
  const int n = 10000;
  for (int i = 0; i < n; ++i) sum_z += random_attitude(rng)(2, 2);  // boresight z
  CHECK(std::abs(sum_z / n) < 0.03);
}

TEST_CASE("quaternion: 90 degree rotation about z (guide 5.4)") {
  const double s = std::sqrt(0.5);
  Mat3 expected_cam_from_sky;
  expected_cam_from_sky << 0, 1, 0,
                          -1, 0, 0,
                           0, 0, 1;

  CHECK(max_abs_diff(quaternion_to_matrix(Quat(s, 0, 0, s)), expected_cam_from_sky) < kTolExact);

  const Quat q = matrix_to_quaternion(expected_cam_from_sky);
  CHECK_THAT(q.w(), WithinAbs(s, kTolExact));
  CHECK_THAT(q.x(), WithinAbs(0.0, kTolExact));
  CHECK_THAT(q.y(), WithinAbs(0.0, kTolExact));
  CHECK_THAT(q.z(), WithinAbs(s, kTolExact));
}

TEST_CASE("quaternion: round trip and canonical sign") {
  std::mt19937_64 rng(2024);
  for (int i = 0; i < 1000; ++i) {
    const Mat3 A = random_attitude(rng);
    const Quat q = matrix_to_quaternion(A);
    CHECK(q.w() >= 0.0);
    CHECK_THAT(q.norm(), WithinAbs(1.0, kTolUnit));
    CHECK(max_abs_diff(quaternion_to_matrix(q), A) < kTolExact);
    // -q is the same rotation, so it must give the same matrix.
    CHECK(max_abs_diff(quaternion_to_matrix(Quat(-q.coeffs())), A) < kTolExact);
  }
}

TEST_CASE("attitude_to_radec_pa: inverts attitude_from_pointing") {
  std::mt19937_64 rng(99);
  std::uniform_real_distribution<double> ra_d(0.0, 2.0 * kPi);
  std::uniform_real_distribution<double> dec_d(-1.4, 1.4);  // stay off the poles
  std::uniform_real_distribution<double> pa_d(0.0, 2.0 * kPi);
  for (int i = 0; i < 1000; ++i) {
    const Mat3 A = attitude_from_pointing(ra_d(rng), dec_d(rng), pa_d(rng));
    const Pointing p = attitude_to_radec_pa(A);
    CHECK(p.ra_rad >= 0.0);
    CHECK(p.ra_rad < 2.0 * kPi);
    CHECK(max_abs_diff(attitude_from_pointing(p.ra_rad, p.dec_rad, p.pa_rad), A) < kTolExact);
  }
}
