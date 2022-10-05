/// Copyright (c) . All rights reserved.
///
/// Author: premiumpeng@tencent.com(premiumpeng)
#ifndef _GRAV2D_PLANET2D_H_
#define _GRAV2D_PLANET2D_H_

#include "colors.h"
#include "position_2d.h"
#include "vector_force_2d.h"

class Planet2D {
public:
  double DistanceSquare(const Planet2D &p) const;
  Position2D origin;
  double radius;
  double weight;
  RGBAf color;
};

VectorForce2D Gravity(const Planet2D &p1, const Planet2D &p2);
#endif // _GRAV2D_PLANET2D_H_