#include "vector_force_2d.h"

#include <cmath>

float VectorForce2D::Force() const {
  return std::sqrt(force_axis_x_ * force_axis_x_ +
                   force_axis_y_ * force_axis_y_);
}