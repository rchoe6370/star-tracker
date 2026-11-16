#pragma once
// All convention-dependent rotation and direction math lives here

#include <random>
#include "startracker/types.hpp"

namespace st {

// RA/Dec in RADIANS -> unit vector in the sky frame.
Vec3 radec_to_unit(double ra_rad, double dec_rad);

// Angle between two vectors, radians
double angle_between(const Vec3& u, const Vec3& v);

//Position angle pa - angle of image-up measured from north toward east
Mat3 attitude_from_pointing(double ra_red, double dec_rad, double pa_red);

Mat3 random_attitude(std::mt19937_64& rng);

Quat matrix_to_quaternion(const Mat3& A_cam_from_sky);
Mat3 quaternion_to_matrix(const Quat& q);

struct Pointing {
    double ra_rad;
    double dec_rad;
    double pa_rad;
};

Pointing attitude_to_radec_pa(const Mat3& A_cam_from_sky);


}  // namespace st
