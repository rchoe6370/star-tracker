#pragma once
// Basic types used everywhere. Frame is encoded in variable names
// (v_cam, v_sky, A_cam_from_sky), see docs/conventions.md.
#include <Eigen/Dense>
#include <cstdint>

namespace st {

using Vec3 = Eigen::Vector3d;
using Mat3 = Eigen::Matrix3d;
using Vec2 = Eigen::Vector2d;

constexpr double kPi = 3.14159265358979323846;
constexpr double deg2rad(double d) { return d * kPi / 180.0; }
constexpr double rad2deg(double r) { return r * 180.0 / kPi; }
constexpr double rad2arcsec(double r) { return rad2deg(r) * 3600.0; }

// One catalog star. Direction is a unit vector in the sky frame.
struct Star {
  std::int32_t id = 0;     // catalog ID (HR number for BSC5)
  Vec3 v_sky = Vec3::UnitX();
  double magnitude = 0.0;  // smaller = brighter
};

// One camera measurement (used from Phase 1 on).
struct Measurement {
  Vec2 pixel = Vec2::Zero();      // (column, row)
  Vec3 v_cam = Vec3::UnitZ();     // unit direction in camera frame
};

}  // namespace st
