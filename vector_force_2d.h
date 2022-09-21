#ifndef _GRAV2D_VECTOR_FORCE_2D_H_
#define _GRAV2D_VECTOR_FORCE_2D_H_

class VectorForce2D {
public:
  float force_axis_x_;
  float force_axis_y_;

  float Force() const;
};

#endif // !_GRAV2D_VECTOR_FORCE_2D_H_