/// Copyright (c) . All rights reserved.
///
/// Author: premiumpeng@tencent.com(premiumpeng)
#ifndef _GRAV2D_PLANET2D_H_
#define _GRAV2D_PLANET2D_H_

#include <list>

#include "colors.h"
#include "position_2d.h"
#include "vector_force_2d.h"

class Planet2D {
public:
  Planet2D(Position2D &&pos, double rad, double w, RGBAf &&col);

  double DistanceSquare(const Planet2D &p) const;
  void Move(const Position2D &new_pos);

  inline const Position2D Position() const { return origin_; }
  inline double Radius() const { return radius_; }
  inline double Weight() const { return weight_; }
  inline RGBAf Color() const { return color_; }
  inline const std::list<Position2D> &Tracks() const { return tracks_; }

private:
  Position2D origin_;
  double radius_;
  double weight_;
  RGBAf color_;

  std::list<Position2D> tracks_;
};

VectorForce2D Gravity(const Planet2D &p1, const Planet2D &p2);
#endif // _GRAV2D_PLANET2D_H_