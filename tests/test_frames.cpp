#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "startracker/frames.hpp"

using namespace st;
using Catch::Matchers::WithinAbs;
constexpr double kTol = 1e-9;

TEST_CASE("radec_to_unit known directions") {
  Vec3 a = radec_to_unit(0.0, 0.0);
  CHECK((a - Vec3(1, 0, 0)).norm() < kTol);

  // Dec = +90 deg gives +z regardless of RA
  Vec3 b = radec_to_unit(deg2rad(123.0), deg2rad(90.0));
  CHECK((b - Vec3(0, 0, 1)).norm() < kTol);
}

TEST_CASE("radec_to_unit returns unit vectors") {
  for (double ra = 0; ra < 360; ra += 37)
    for (double dec = -90; dec <= 90; dec += 15)
      CHECK_THAT(radec_to_unit(deg2rad(ra), deg2rad(dec)).norm(), WithinAbs(1.0, kTol));
}

TEST_CASE("angle_between") {
  CHECK_THAT(angle_between(Vec3::UnitX(), Vec3::UnitY()), WithinAbs(kPi / 2, kTol));
  CHECK_THAT(angle_between(Vec3::UnitX(), Vec3::UnitX()), WithinAbs(0.0, kTol));
  // Slightly >1 dot product must not produce NaN
  Vec3 v = Vec3(1, 1e-9, 0).normalized();
  CHECK(!std::isnan(angle_between(v, v)));
}
