#include "opengl_utility.h"
#include <OpenGL/gl.h>
#include <cmath>
#include <glut/glut.h>

void DrawCircle(const Position2D &pos, int radius, const RGBAf &color,
                __unused float step_width) {
  glColor4f(std::get<0>(color), std::get<1>(color), std::get<2>(color),
            std::get<3>(color));
  glBegin(GL_POLYGON); // Middle circle
  const auto kConst = 300;
  for (int i = 0; i <= kConst; i++) {
    double angle = 2 * M_PI * i / kConst;
    double x = cos(angle) * radius;
    double y = sin(angle) * radius;
    glVertex2d(pos.x + x, pos.y + y);
  }
  glEnd();
  glFlush();
}

void DrawCircle(const Position2D &pos, int radius, const RGBAf &color) {
  glColor4f(std::get<0>(color), std::get<1>(color), std::get<2>(color),
            std::get<3>(color));
  glBegin(GL_POLYGON); // Middle circle
  const auto kConst = 300;
  for (int i = 0; i <= kConst; i++) {
    double angle = 2 * M_PI * i / kConst;
    double x = cos(angle) * radius;
    double y = sin(angle) * radius;
    glVertex2d(pos.x + x, pos.y + y);
  }
  glEnd();
  glFlush();
}