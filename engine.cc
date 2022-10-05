#include "engine.h"

#include <_types/_uint64_t.h>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <gflags/gflags.h>
#include <spdlog/spdlog.h>

#include "colors.h"
#include "opengl_utility.h"
#include "planet_2d.h"

Engine engine;

DECLARE_uint32(draw_interval);
DECLARE_int64(recalc_cnt);
DECLARE_uint32(recalc_millisec);

extern unsigned bg_stars;

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

void EngineIdle() { glutPostRedisplay(); }

int Engine::AddPlanet(Planet2D &&p2d, VectorForce2D &&v2d) {
  entites_.push_back({.p = std::move(p2d), .velocity = v2d});
  return 0;
}

void Engine::GenerateBackgroundStars(unsigned count) {
  for (auto i = 0; i < count; ++i) {
    double x = rand() % (size_x / 2);
    if (rand() % 2) {
      x *= -1;
    }
    double y = rand() % (size_y / 2);
    if (rand() % 2) {
      y *= -1;
    }
    background_stars_.push_back(Planet2D({.origin = {.x = x, .y = y},
                                          .radius = 1,
                                          .weight = M_PI,
                                          .color = {1.0, 1.0, 1.0, 1.0}}));
  }
}

void Engine::ShuffleBackground() {
  auto rand_num = rand();

  if ((rand_num % 200) < 2) {
    return;
  }

  auto cnt = background_stars_.size() / 100;
  for (auto i = 0u; i < cnt; ++i) {
    auto &bg_star =
        background_stars_[(rand_num % background_stars_.size() + i) %
                          background_stars_.size()];
    auto delta_x = rand_num % 5;
    if (rand() % 2) {
      delta_x *= -1;
    }
    auto delta_y = rand_num % 5;
    if (rand() % 2) {
      delta_y *= -1;
    }
    bg_star.origin.x += delta_x;
    if (bg_star.origin.x > (size_x / 2) || bg_star.origin.x < -(size_x / 2)) {
      bg_star.origin.x = rand() % (size_x / 2);
      bg_star.origin.x *= (rand() % 2 ? 1 : -1);
    }
    bg_star.origin.y += delta_y;
    if (bg_star.origin.y > (size_y / 2) || bg_star.origin.y < -(size_y / 2)) {
      bg_star.origin.y = rand() % (size_y / 2);
      bg_star.origin.y *= (rand() % 2 ? 1 : -1);
    }
  }
}

int Engine::EngineInit(int sx, int sy, RGBAf bgcolor) {
  size_x = sx;
  size_y = sy;
  GenerateBackgroundStars(bg_stars);
  int argc = 0;
  glutInit(&argc, nullptr);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
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

  gluOrtho2D(-(double)(size_x) / 2, (double)(size_x) / 2, -(double)size_y / 2,
             (double)(size_y) / 2);

  glutDisplayFunc(EngineDisplay);
  glutIdleFunc(EngineIdle);
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

  ShuffleBackground();
  for (const auto &en : background_stars_) {
    DrawPoint(en.origin, en.color);
  }
  glutSwapBuffers();
  // glFlush();
  return 0;
}

int Engine::Recalc() {
  const auto kIntervalSec = (double)FLAGS_recalc_millisec / 1000;
  const auto kIntervalMillisecSquareHalf = kIntervalSec * kIntervalSec / 2;

  std::vector<std::vector<double>> deltas(entites_.size(),
                                          std::vector<double>(4, 0.0));
  for (auto i = 0u; i < entites_.size(); ++i) {
    deltas[i][0] += entites_[i].velocity.force_axis_x_ * kIntervalSec;
    deltas[i][1] += entites_[i].velocity.force_axis_y_ * kIntervalSec;
    for (auto j = i + 1; j < entites_.size(); ++j) {

      auto forces = Gravity(entites_[i].p, entites_[j].p);
      auto diff_x = entites_[j].p.origin.x - entites_[i].p.origin.x;
      auto diff_y = entites_[j].p.origin.y - entites_[i].p.origin.y;
      forces.force_axis_x_ *= (diff_x < 0 ? -1 : 1);
      forces.force_axis_y_ *= (diff_y < 0 ? -1 : 1);

      auto delta_s_x = forces.force_axis_x_ * kIntervalMillisecSquareHalf /
                       entites_[i].p.weight;
      auto delta_s_y = forces.force_axis_y_ * kIntervalMillisecSquareHalf /
                       entites_[i].p.weight;
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
    SPDLOG_TRACE("planet[{}] before update, loc[{:.3f},{:.3f}], "
                 "vel[{:.3f},{:.3f}]={:.3f}",
                 i, entites_[i].p.origin.x, entites_[i].p.origin.y,
                 entites_[i].velocity.force_axis_x_,
                 entites_[i].velocity.force_axis_y_,
                 entites_[i].velocity.Force());
    entites_[i].p.origin.x += deltas[i][0];
    entites_[i].p.origin.y += deltas[i][1];
    entites_[i].velocity.force_axis_x_ += deltas[i][2];
    entites_[i].velocity.force_axis_y_ += deltas[i][3];

    auto bb_ret = BorderBounce(entites_[i]);
    SPDLOG_INFO(
        "planet[{}] now at[{:.3f},{:.3f}] velocity[{:.3f},{:.3f}]={:.3f}, "
        "dS=[{:.3f},{:.3f}], dV=[{:.3f},{:.3f}], border_bouce[{}]",
        i, entites_[i].p.origin.x, entites_[i].p.origin.y,
        entites_[i].velocity.force_axis_x_, entites_[i].velocity.force_axis_y_,
        entites_[i].velocity.Force(), deltas[i][0], deltas[i][1], deltas[i][2],
        deltas[i][3], bb_ret);
  }
  static uint64_t count = 0;
  ++count;
  if (FLAGS_recalc_cnt < 0 || count < FLAGS_recalc_cnt) {
    glutTimerFunc(FLAGS_recalc_millisec, EngineTimer, RECALC);
  }
  return 0;
}

int Engine::BorderBounce(decltype(Engine::entites_)::value_type &entity) const {
  const auto kHalfX = size_x / 2;
  const auto kHalfY = size_y / 2;
  if (entity.p.origin.x < -kHalfX) {
    SPDLOG_TRACE("hit left border[x({:.3f})<left_bder({})], speed[{:.3f}]",
                 entity.p.origin.x, -kHalfX, entity.velocity.force_axis_x_);
    entity.p.origin.x = -size_x - entity.p.origin.x;
    if (entity.velocity.force_axis_x_ < 0) {
      entity.velocity.force_axis_x_ *= -1;
    }
    return 0;
  } else if (entity.p.origin.x > kHalfX) {
    SPDLOG_TRACE("hit right border[x({:.3f})>right_bder({})], speed[{:.3f}]",
                 entity.p.origin.x, kHalfX, entity.velocity.force_axis_x_);
    entity.p.origin.x = size_x - entity.p.origin.x;
    if (entity.velocity.force_axis_x_ > 0) {
      entity.velocity.force_axis_x_ *= -1;
    }
    return 2;
  } else if (entity.p.origin.y < -kHalfY) {
    SPDLOG_TRACE("hit top border[y({:.3f})<top_bder({})], speed[{:.3f}]",
                 entity.p.origin.y, -kHalfY, entity.velocity.force_axis_y_);
    entity.p.origin.y = -size_y - entity.p.origin.y;
    if (entity.velocity.force_axis_y_ < 0) {
      entity.velocity.force_axis_y_ *= -1;
    }
    return 1;
  } else if (entity.p.origin.y > kHalfY) {
    SPDLOG_TRACE("hit bottom border[y({:.3f})>bottom_bder({})], speed[{:.3f}]",
                 entity.p.origin.y, kHalfY, entity.velocity.force_axis_y_);
    entity.p.origin.y = size_y - entity.p.origin.y;
    if (entity.velocity.force_axis_y_ > 0) {
      entity.velocity.force_axis_y_ *= -1;
    }
    return 3;
  }

  return -1;
}
