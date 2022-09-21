/// Copyright (c) . All rights reserved.
///
/// Author: premiumpeng@tencent.com(premiumpeng)
#ifndef _GRAV2D_PLANET2D_H_
#define _GRAV2D_PLANET2D_H_

#include "position_2d.h"
#include "vector_force_2d.h"
#include <functional>

class Planet2D {
public:
  double DistanceSquare(const Planet2D &p) const;
  Position2D origin;
  float radius;

  int weight;
};

VectorForce2D Gravity(const Planet2D &p1, const Planet2D &p2);
#endif // _GRAV2D_PLANET2D_H_