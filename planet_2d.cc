#include "planet_2d.h"
#include <gflags/gflags.h>

#include "colors.h"
#include "position_2d.h"
#include "vector_force_2d.h"
#include <cmath>
#include <limits>

namespace {
const float kGravityConstant = 6.67;
} // namespace

DECLARE_uint32(history_tracks);

Planet2D::Planet2D(Position2D &&pos, double rad, double w, RGBAf &&col)
    : origin_(pos), radius_(rad), weight_(w),
      color_({std::get<0>(col) * 255, std::get<1>(col) * 255,
              std::get<2>(col) * 255, std::get<3>(col) * 255}) {}

double Planet2D::DistanceSquare(const Planet2D &p) const {
  auto delta_x = std::abs((double)(origin_.x - p.origin_.x));
  auto delta_y = std::abs((double)(origin_.y - p.origin_.y));
  return std::sqrt(delta_x * delta_x + delta_y * delta_y);
}

VectorForce2D Gravity(const Planet2D &p1, const Planet2D &p2) {
  VectorForce2D vf;
  if (p1.Weight() == 0 || p2.Weight() == 0) {
    return {0, 0};
  }
  auto delta_x = std::abs((double)(p1.Position().x - p2.Position().x));
  auto delta_y = std::abs((double)(p1.Position().y - p2.Position().y));
  double distance = 0;
  if (delta_x == 0.0) {
    distance = delta_y;
  } else if (delta_y == 0.0) {
    distance = delta_x;
  } else {
    distance = std::sqrt(delta_x * delta_x + delta_y * delta_y);
  }

  auto tmp = kGravityConstant * p1.Weight() * p2.Weight() / distance /
             distance / distance;
  vf.force_axis_x_ = tmp * delta_x;
  vf.force_axis_y_ = tmp * delta_y;
  return vf;
}

void Planet2D::Move(const Position2D &new_pos) {
  tracks_.push_front(origin_);
  origin_ = new_pos;
  while (tracks_.size() > FLAGS_history_tracks) {
    tracks_.pop_back();
  }
}