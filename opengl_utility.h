#ifndef _GRAV2D_OPENGL_UTILITY_H_
#define _GRAV2D_OPENGL_UTILITY_H_
#include <tuple>

#include <OpenGL/gltypes.h>

#include "colors.h"
#include "position_2d.h"

#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif // GL_SILENCE_DEPRECATION

void DrawPoint(const Position2D &pos, const RGBAf &color);
void DrawCurve(const std::vector<Position2D> &dots, const RGBAf &color);
void DrawCircle(const Position2D &pos, float radius, const RGBAf &color);
void DrawCircle(const Position2D &pos, int radius, const RGBAf &color,
                __unused float step_width);
#endif // !_GRAV2D_OPENGL_UTILITY_H_