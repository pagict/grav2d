#include "vector_force_2d.h"

#include <cmath>

VectorForce2D::VectorForce2D(float x, float y)
    : force_axis_x_(x), force_axis_y_(y) {}

float VectorForce2D::Force() const {
  return std::sqrt(force_axis_x_ * force_axis_x_ +
                   force_axis_y_ * force_axis_y_);
}