#pragma once
// Basic types used everywhere. Frame is encoded in variable names
// (v_cam, v_sky, A_cam_from_sky), see docs/conventions.md.
#include <Eigen/Dense>
#include <cstdint>

namespace st {

using Vec3 = Eigen::Vector3d;
using Mat3 = Eigen::Matrix3d;
using Quat = Eigen::Quaterniond;

constexpr double kPi = 3.14159265358979323846;
constexpr double deg2rad(double d) { return d * kPi / 180.0; }
constexpr double rad2deg(double r) { return r * 180.0 / kPi; }
constexpr double rad2arcsec(double r) { return rad2deg(r) * 3600.0; }

// Shared tolerances
constexpr double kTolUnit = 1e-12;        
constexpr double kTo1Exact = 1e-9;
constexpr double kTo1Crosscheck = 1e-8;

// One catalog star. Direction is a unit vector in the sky frame.
struct Star {
  std::int32_t id = 0;     // catalog ID (HR number for BSC5)
  double ra = 0.0;
  double dec = 0.0;
  Vec3 dir_sky = Vec3::UnitX(); // Unit vector, sky freame
  double mag = 0.0;  // smaller = brighter
};

// One camera measurement (used from Phase 1 on).
struct Measurement {
  double u = 0.0;
  double v = 0.0;
  Vec3 dir_cam = Vec3::UnitZ(); // Unit direction, camera frame
  double brightness = 0.0; 
};

}  // namespace st
