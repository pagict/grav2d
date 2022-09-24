#include "engine.h"

#include <cmath>
#include <tuple>
#include <utility>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <gflags/gflags.h>

#include "colors.h"
#include "opengl_utility.h"
#include "planet_2d.h"

Engine engine;

DECLARE_uint32(draw_interval);

void EngineDisplay() { engine.Display(); }
void EngineTimer(int val) { engine.TimerFlush(); }

int Engine::AddPlanet(Planet2D &&p2d, VectorForce2D &&v2d) {
  entites_.push_back({.p = std::move(p2d), .velocity = v2d});
  return 0;
}

int Engine::EngineInit(int sx, int sy, RGBAf bgcolor) {
  size_x = sx;
  size_y = sy;

  int argc = 0;
  glutInit(&argc, nullptr);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(size_x, size_y);
  glutInitWindowPosition(0, 0);
  glutCreateWindow(kWindowTitle);

  glClearColor(std::get<0>(bgcolor), std::get<1>(bgcolor), std::get<2>(bgcolor),
               std::get<3>(bgcolor));
  glClear(GL_COLOR_BUFFER_BIT);

  // glColor3f(0.0f, 1.0f, 0.0f);
  glPointSize(1.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  const auto kMargin = 36u;
  gluOrtho2D(-(double)(size_x) / 2, (double)(size_x) / 2,
             -(double)size_y / 2 - kMargin, (double)(size_y) / 2 + kMargin);
  glViewport(-(double)(size_x) / 2, (double)(size_x) / 2,
             -(double)size_y / 2 - kMargin, (double)(size_y) / 2 + kMargin);

  glutDisplayFunc(EngineDisplay);
  glutTimerFunc(kFlushIntervalMillisec, EngineTimer, 0);
  return 0;
}

int Engine::Display() {
  for (const auto en : entites_) {
    DrawCircle(en.p.origin, en.p.radius, en.p.color);
  }

  return 0;
}

int Engine::TimerFlush() {
  glClear(GL_COLOR_BUFFER_BIT);
  const auto kIntervalSec = (double)kFlushIntervalMillisec / 1000;
  const auto kIntervalMillisecSquareHalf = kIntervalSec * kIntervalSec / 2;
  for (auto i = 0u; i < entites_.size(); ++i) {
    for (auto j = i + 1; j < entites_.size(); ++j) {

      auto forces = Gravity(entites_[i].p, entites_[j].p);
      auto diff_x = entites_[j].p.origin.x - entites_[i].p.origin.x;
      auto diff_y = entites_[j].p.origin.y - entites_[i].p.origin.y;
      forces.force_axis_x_ *= (diff_x < 0 ? -1 : 1);
      forces.force_axis_y_ *= (diff_y < 0 ? -1 : 1);

      auto distance = std::sqrt(diff_x * diff_x + diff_y * diff_y);
      auto vvv = std::sqrt(forces.Force() * distance / entites_[i].p.weight);

      auto delta_s_x = forces.force_axis_x_ * kIntervalMillisecSquareHalf /
                           entites_[i].p.weight +
                       entites_[i].velocity.force_axis_x_ * kIntervalSec;
      auto delta_s_y = forces.force_axis_y_ * kIntervalMillisecSquareHalf /
                           entites_[i].p.weight +
                       entites_[i].velocity.force_axis_y_ * kIntervalSec;
      auto delta_v_x =
          forces.force_axis_x_ * kIntervalSec / entites_[i].p.weight;
      auto delta_v_y =
          forces.force_axis_y_ * kIntervalSec / entites_[i].p.weight;
      entites_[i].p.origin.x += delta_s_x;
      entites_[i].p.origin.y += delta_s_y;
      entites_[i].velocity.force_axis_x_ += delta_v_x;
      entites_[i].velocity.force_axis_y_ += delta_v_y;

      forces.force_axis_x_ *= -1;
      forces.force_axis_y_ *= -1;
      delta_s_x = forces.force_axis_x_ * kIntervalMillisecSquareHalf /
                      entites_[j].p.weight +
                  entites_[j].velocity.force_axis_x_ * kIntervalSec;
      delta_s_y = forces.force_axis_y_ * kIntervalMillisecSquareHalf /
                      entites_[j].p.weight +
                  entites_[j].velocity.force_axis_y_ * kIntervalSec;
      delta_v_x = forces.force_axis_x_ * kIntervalSec / entites_[j].p.weight;
      delta_v_y = forces.force_axis_y_ * kIntervalSec / entites_[j].p.weight;

      entites_[j].p.origin.x += delta_s_x;
      entites_[j].p.origin.y += delta_s_y;
      entites_[j].velocity.force_axis_x_ += delta_v_x;
      entites_[j].velocity.force_axis_y_ += delta_v_y;
    }
  }
  static int count = 0;
  if (count % FLAGS_draw_interval == 0) {
    glutPostRedisplay();
  }
  ++count;
  glutTimerFunc(kFlushIntervalMillisec, EngineTimer, 0);
  return 0;
}
