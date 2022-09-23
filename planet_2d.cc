#include "planet_2d.h"
#include "vector_force_2d.h"
#include <cmath>
#include <limits>

namespace {
float kGravityConstant = 9.8;
}

double Planet2D::DistanceSquare(const Planet2D &p) const {
  auto delta_x = std::abs((double)(origin.x - p.origin.x));
  auto delta_y = std::abs((double)(origin.y - p.origin.y));
  return std::sqrt(delta_x * delta_x + delta_y * delta_y);
}

VectorForce2D Gravity(const Planet2D &p1, const Planet2D &p2) {
  VectorForce2D vf;
  if (p1.weight == 0 || p2.weight == 0) {
    return VectorForce2D(0, 0);
  }
  auto delta_x = std::abs((double)(p1.origin.x - p2.origin.x));
  auto delta_y = std::abs((double)(p1.origin.y - p2.origin.y));
  if (delta_x == 0.0f || delta_y == 0.0f) {
    return VectorForce2D(std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());
  }
  auto distance = std::sqrt(delta_x * delta_x + delta_y * delta_y);

  auto tmp = kGravityConstant * p1.weight * p2.weight * distance;
  vf.force_axis_x_ = tmp / delta_y;
  vf.force_axis_y_ = tmp / delta_x;
  return vf;
}