#ifndef _GRAV2D_VECTOR_FORCE_2D_H_
#define _GRAV2D_VECTOR_FORCE_2D_H_

class VectorForce2D {
public:
  VectorForce2D() = default;
  VectorForce2D(double x, double y);
  double force_axis_x_ = 0.0;
  double force_axis_y_ = 0.0;

  double Force() const;
};

#endif // !_GRAV2D_VECTOR_FORCE_2D_H_