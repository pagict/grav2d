#include "engine.h"

#include <_types/_uint64_t.h>
#include <cmath>
#include <memory>
#include <tuple>
#include <utility>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <gflags/gflags.h>
#include <spdlog/spdlog.h>
#include <vector>

#include "colors.h"
#include "opengl_utility.h"
#include "planet_2d.h"

extern std::shared_ptr<spdlog::logger> grav2d_logger;

Engine engine;

DECLARE_uint32(draw_interval);
DECLARE_int64(recalc_cnt);
DECLARE_uint32(recalc_millisec);

void EngineDisplay() { engine.Display(); }
void EngineTimer(int val) {
  switch (val) {
  case RECALC:
    engine.Recalc();
    return;
  case REDRAW:
    glutPostRedisplay();
    glutTimerFunc(FLAGS_draw_interval * FLAGS_recalc_millisec, EngineTimer,
                  REDRAW);
    return;
  }
}

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
  gluOrtho2D(-(double)(size_x) / 2, (double)(size_x) / 2, -(double)size_y / 2,
             (double)(size_y) / 2);

  glutDisplayFunc(EngineDisplay);
  glutTimerFunc(FLAGS_recalc_millisec, EngineTimer, RECALC);
  glutTimerFunc(FLAGS_draw_interval * FLAGS_recalc_millisec, EngineTimer,
                REDRAW);
  return 0;
}

int Engine::Display() {
  glClear(GL_COLOR_BUFFER_BIT);
  for (const auto &en : entites_) {
    DrawCircle(en.p.origin, en.p.radius, en.p.color);
  }

  return 0;
}

int Engine::Recalc() {
  const auto kIntervalSec = (double)FLAGS_recalc_millisec / 1000;
  const auto kIntervalMillisecSquareHalf = kIntervalSec * kIntervalSec / 2;

  std::vector<std::vector<double>> deltas(entites_.size(),
                                          std::vector<double>(4, 0.0));
  for (auto i = 0u; i < entites_.size(); ++i) {
    for (auto j = i + 1; j < entites_.size(); ++j) {

      auto forces = Gravity(entites_[i].p, entites_[j].p);
      auto diff_x = entites_[j].p.origin.x - entites_[i].p.origin.x;
      auto diff_y = entites_[j].p.origin.y - entites_[i].p.origin.y;
      forces.force_axis_x_ *= (diff_x < 0 ? -1 : 1);
      forces.force_axis_y_ *= (diff_y < 0 ? -1 : 1);

      // auto distance = std::sqrt(diff_x * diff_x + diff_y * diff_y);
      // auto vvv = std::sqrt(forces.Force() * distance / entites_[i].p.weight);
      // grav2d_logger->trace("vvv={} distance={}", vvv, distance);

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

      deltas[i][0] += delta_s_x;
      deltas[i][1] += delta_s_y;
      deltas[i][2] += delta_v_x;
      deltas[i][3] += delta_v_y;

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

      deltas[j][0] += delta_s_x;
      deltas[j][1] += delta_s_y;
      deltas[j][2] += delta_v_x;
      deltas[j][3] += delta_v_y;
    }
  }
  for (auto i = 0u; i < entites_.size(); ++i) {
    grav2d_logger->trace("planet[{}] before update, loc[{},{}], vel[{},{}]={}",
                         i, entites_[i].p.origin.x, entites_[i].p.origin.y,
                         entites_[i].velocity.force_axis_x_,
                         entites_[i].velocity.force_axis_y_,
                         entites_[i].velocity.Force());
    entites_[i].p.origin.x += deltas[i][0];
    entites_[i].p.origin.y += deltas[i][1];
    entites_[i].velocity.force_axis_x_ += deltas[i][2];
    entites_[i].velocity.force_axis_y_ += deltas[i][3];

    grav2d_logger->info(
        "planet[{}] now at[{},{}] velocity[{},{}]={}, dS=[{},{}], dV=[{},{}]",
        i, entites_[i].p.origin.x, entites_[i].p.origin.y,
        entites_[i].velocity.force_axis_x_, entites_[i].velocity.force_axis_y_,
        entites_[i].velocity.Force(), deltas[i][0], deltas[i][1], deltas[i][2],
        deltas[i][3]);
  }
  static uint64_t count = 0;
  ++count;
  if (FLAGS_recalc_cnt < 0 || count < FLAGS_recalc_cnt) {
    glutTimerFunc(FLAGS_recalc_millisec, EngineTimer, RECALC);
  }
  return 0;
}
