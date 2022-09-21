#ifndef _GRAV2D_OPENGL_UTILITY_H_
#define _GRAV2D_OPENGL_UTILITY_H_
#include "position_2d.h"
#include <OpenGL/gltypes.h>
#include <tuple>

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif // GL_SILENCE_DEPRECATION
using RGB = std::tuple<GLfloat, GLfloat, GLfloat>;
using RGBA = std::tuple<GLfloat, GLfloat, GLfloat, GLfloat>;

void DrawCircle(const Position2D &pos, int radius, const RGBA &color);
void DrawCircle(const Position2D &pos, int radius, const RGBA &color,
                float step_width);
#endif // !_GRAV2D_OPENGL_UTILITY_H_