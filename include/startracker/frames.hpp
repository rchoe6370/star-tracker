#pragma once
// All convention-dependent math lives here (and in camera.hpp).
// If you flip an axis, this is the file you change.
#include "startracker/types.hpp"

namespace st {

// RA/Dec in RADIANS -> unit vector in the sky frame.
// x = cos(dec)cos(ra), y = cos(dec)sin(ra), z = sin(dec)
Vec3 radec_to_unit(double ra_rad, double dec_rad);

// Angle in radians between two unit vectors (clamp before acos!).
double angle_between(const Vec3& u, const Vec3& v);

// Phase 1 (declare now, implement later):
// Mat3 attitude_from_pointing(double ra_rad, double dec_rad, double roll_rad);
// Mat3 random_attitude(std::mt19937_64& rng);
// Eigen::Quaterniond dcm_to_quat(const Mat3& A);
// Mat3 quat_to_dcm(const Eigen::Quaterniond& q);

}  // namespace st
