#ifndef _GRAV2D_OPENGL_UTILITY_H_
#define _GRAV2D_OPENGL_UTILITY_H_
#include <tuple>
#include <type_traits>

#include <OpenGL/gl.h>
#include <OpenGL/gltypes.h>
#include <glut/glut.h>

#include "base.h"
#include "colors.h"
#include "position_2d.h"

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif // GL_SILENCE_DEPRECATION

void DrawPoint(const Position2D &pos, const RGBAuc &color);
void DrawCircle(const Position2D &pos, float radius, const RGBAuc &color);
void DrawCircle(const Position2D &pos, int radius, const RGBAuc &color,
                __unused float step_width);

template <typename T,
          typename std::enable_if_t<is_iterable<T>::value, bool> =
              true, // should be a iterable container, vector/list
          typename std::enable_if_t<
              std::is_same<Position2D, std::remove_all_extents_t<
                                           typename T::value_type>>::value,
              bool> = true> // and the value_type of the container should be
                            // Position2D
void DrawCurve(const T &dots, const RGBAuc &color, float width) {
  glColor4ub(std::get<0>(color), std::get<1>(color), std::get<2>(color),
             std::get<3>(color));
  glLineWidth(width);
  glBegin(GL_LINE_STRIP);
  for (const auto &d : dots) {
    glVertex2d(d.x, d.y);
  }
  glEnd();
}

#endif // !_GRAV2D_OPENGL_UTILITY_H_