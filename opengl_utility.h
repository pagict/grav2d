#ifndef _GRAV2D_OPENGL_UTILITY_H_
#define _GRAV2D_OPENGL_UTILITY_H_
#include "colors.h"
#include "position_2d.h"
#include <OpenGL/gltypes.h>
#include <tuple>

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif // GL_SILENCE_DEPRECATION

void DrawCircle(const Position2D &pos, int radius, const RGBAf &color);
void DrawCircle(const Position2D &pos, int radius, const RGBAf &color,
                __unused float step_width);
#endif // !_GRAV2D_OPENGL_UTILITY_H_