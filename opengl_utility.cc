#include "opengl_utility.h"
#include "colors.h"
#include <cmath>

#include <OpenGL/gl.h>
#include <glut/glut.h>
#include <vector>

void DrawCircle(const Position2D &pos, int radius, const RGBAuc &color,
                __unused float step_width) {
  glColor4ub(std::get<0>(color), std::get<1>(color), std::get<2>(color),
             std::get<3>(color));
  glBegin(GL_POLYGON); // Middle circle
  const auto kConst = 400;
  for (int i = 0; i <= kConst; i++) {
    double angle = 2 * M_PI * i / kConst;
    double x = cos(angle) * radius;
    double y = sin(angle) * radius;
    glVertex2d(pos.x + x, pos.y + y);
  }
  glEnd();
  glFlush();
}

void DrawCircle(const Position2D &pos, float radius, const RGBAuc &color) {
  if (radius <= 0.5f) {
    return DrawPoint(pos, color);
  }

  glColor4ub(std::get<0>(color), std::get<1>(color), std::get<2>(color),
             std::get<3>(color));
  glBegin(GL_POLYGON); // Middle circle
  const auto kConst = 400;
  for (int i = 0; i <= kConst; i++) {
    double angle = 2 * M_PI * i / kConst;
    double x = cos(angle) * radius;
    double y = sin(angle) * radius;
    glVertex2d(pos.x + x, pos.y + y);
  }
  glEnd();
}

void DrawPoint(const Position2D &pos, const RGBAuc &color) {
  glColor4ub(std::get<0>(color), std::get<1>(color), std::get<2>(color),
             std::get<3>(color));
  glBegin(GL_POINTS);
  glVertex2d(pos.x, pos.y);
  glEnd();
}

void DrawCurve(const std::vector<Position2D> &dots, const RGBAuc &color) {
  glColor4ub(std::get<0>(color), std::get<1>(color), std::get<2>(color),
             std::get<3>(color));
  glBegin(GL_LINE_STRIP);
  for (const auto &d : dots) {
    glVertex2d(d.x, d.y);
  }
  glEnd();
}
