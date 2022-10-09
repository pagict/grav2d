#include "engine.h"

#include <_types/_uint64_t.h>
#include <cmath>
#include <cstdlib>
#include <iterator>
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
#include "position_2d.h"

Engine engine;

DECLARE_uint32(draw_interval);
DECLARE_int64(recalc_cnt);
DECLARE_uint32(recalc_millisec);
DECLARE_uint32(predict_cnt);

extern unsigned bg_stars;

void EngineDisplay() { engine.Display(); }
void EngineTimer(int val) {
  switch (val) {
  case RECALC:
    engine.Recalc();
    return;
  case REDRAW:
    engine.Redraw();
    return;
  }
}
void EngineKeyboardFunc(unsigned char c, int x, int y) {
  engine.Keyboard(c, x, y);
}

int Engine::Keyboard(unsigned char c, int x, int y) {
  switch (c) {
  case 32: // spacebar
    is_paused_ = !is_paused_;
    if (!is_paused_) {
      Recalc();
      Redraw();
    }
    return 0;
  default:
    return 1;
  }
}

int Engine::Redraw() {
  if (is_paused_) {
    return 1;
  }
  glutPostRedisplay();
  glutTimerFunc(FLAGS_draw_interval * FLAGS_recalc_millisec, EngineTimer,
                REDRAW);
  return 0;
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
    background_stars_.push_back(
        Planet2D({{.x = x, .y = y}, 1, M_PI, {1.0, 1.0, 1.0, 1.0}}));
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
    auto pos = bg_star.Position();
    pos.x += delta_x;
    if (pos.x > (size_x / 2) || pos.x < -(size_x / 2)) {
      pos.x = rand() % (size_x / 2);
      pos.x *= (rand() % 2 ? 1 : -1);
    }
    pos.y += delta_y;
    if (pos.y > (size_y / 2) || pos.y < -(size_y / 2)) {
      pos.y = rand() % (size_y / 2);
      pos.y *= (rand() % 2 ? 1 : -1);
    }
    bg_star.Move(pos);
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

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glPointSize(1.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(-(double)(size_x) / 2, (double)(size_x) / 2, -(double)size_y / 2,
             (double)(size_y) / 2);

  predicts_ = std::vector<std::vector<Position2D>>(entites_.size());

  glutDisplayFunc(EngineDisplay);
  glutIdleFunc(EngineIdle);
  glutTimerFunc(FLAGS_recalc_millisec, EngineTimer, RECALC);
  glutTimerFunc(FLAGS_draw_interval * FLAGS_recalc_millisec, EngineTimer,
                REDRAW);
  glutKeyboardFunc(EngineKeyboardFunc);
  return 0;
}

int Engine::Display() {
  if (is_paused_) {
    return 1;
  }

  glClear(GL_COLOR_BUFFER_BIT);
  for (auto i = 0u; i < entites_.size(); ++i) {
    const auto &en = entites_[i];
    auto r = en.p.Radius();
    auto color = en.p.ColorUc();
    DrawCircle(en.p.Position(), r, color);

    const unsigned kAlphaIndex = 3;
    std::get<kAlphaIndex>(color) *= 0.05;
    const auto tracks = en.p.Tracks();
    const auto track_sz = tracks.size();
    r *= 0.7;
    const auto radius_grad = r / track_sz;
    const auto alpha_grad = std::get<kAlphaIndex>(color) / track_sz;
    auto iter = tracks.begin();
    for (auto i = 0u; i < track_sz; ++i) {
      DrawCircle(*iter, r, color);

      std::advance(iter, 1);
      r -= radius_grad;
      std::get<kAlphaIndex>(color) -= alpha_grad;
    }

    color = en.p.ColorUc();
    std::get<kAlphaIndex>(color) *= 0.2;
    DrawCurve(predicts_[i], color);
  }

  ShuffleBackground();
  for (const auto &en : background_stars_) {
    DrawPoint(en.Position(), en.ColorUc());
  }
  glutSwapBuffers();
  return 0;
}

int Engine::Recalc() {
  if (is_paused_) {
    return 1;
  }
  const auto kIntervalSec = (double)FLAGS_recalc_millisec / 1000;
  const auto kIntervalMillisecSquareHalf = kIntervalSec * kIntervalSec / 2;

  // delta[i][0-3] means entity[i]'s delta position at x/y, delta velocity at
  // x/y.
  std::vector<std::vector<double>> deltas(entites_.size(),
                                          std::vector<double>(4, 0.0));

  // calculate delta values.
  for (auto i = 0u; i < entites_.size(); ++i) {
    deltas[i][0] += entites_[i].velocity.force_axis_x_ * kIntervalSec;
    deltas[i][1] += entites_[i].velocity.force_axis_y_ * kIntervalSec;
    for (auto j = i + 1; j < entites_.size(); ++j) {

      auto forces = Gravity(entites_[i].p, entites_[j].p);
      auto diff_x = entites_[j].p.Position().x - entites_[i].p.Position().x;
      auto diff_y = entites_[j].p.Position().y - entites_[i].p.Position().y;
      forces.force_axis_x_ *= (diff_x < 0 ? -1 : 1);
      forces.force_axis_y_ *= (diff_y < 0 ? -1 : 1);

      auto delta_s_x = forces.force_axis_x_ * kIntervalMillisecSquareHalf /
                       entites_[i].p.Weight();
      auto delta_s_y = forces.force_axis_y_ * kIntervalMillisecSquareHalf /
                       entites_[i].p.Weight();
      auto delta_v_x =
          forces.force_axis_x_ * kIntervalSec / entites_[i].p.Weight();
      auto delta_v_y =
          forces.force_axis_y_ * kIntervalSec / entites_[i].p.Weight();

      deltas[i][0] += delta_s_x;
      deltas[i][1] += delta_s_y;
      deltas[i][2] += delta_v_x;
      deltas[i][3] += delta_v_y;

      forces.force_axis_x_ *= -1;
      forces.force_axis_y_ *= -1;
      delta_s_x = forces.force_axis_x_ * kIntervalMillisecSquareHalf /
                  entites_[j].p.Weight();
      delta_s_y = forces.force_axis_y_ * kIntervalMillisecSquareHalf /
                  entites_[j].p.Weight();
      delta_v_x = forces.force_axis_x_ * kIntervalSec / entites_[j].p.Weight();
      delta_v_y = forces.force_axis_y_ * kIntervalSec / entites_[j].p.Weight();

      deltas[j][0] += delta_s_x;
      deltas[j][1] += delta_s_y;
      deltas[j][2] += delta_v_x;
      deltas[j][3] += delta_v_y;
    }
  }

  // update positions and velocities by their delta values
  for (auto i = 0u; i < entites_.size(); ++i) {
    auto pos = entites_[i].p.Position();
    SPDLOG_TRACE("planet[{}] before update, loc[{:.3f},{:.3f}], "
                 "vel[{:.3f},{:.3f}]={:.3f}",
                 i, pos.x, pos.y, entites_[i].velocity.force_axis_x_,
                 entites_[i].velocity.force_axis_y_,
                 entites_[i].velocity.Force());

    pos.x += deltas[i][0];
    pos.y += deltas[i][1];
    entites_[i].p.Move(pos);
    entites_[i].velocity.force_axis_x_ += deltas[i][2];
    entites_[i].velocity.force_axis_y_ += deltas[i][3];

    auto bb_ret = BorderBounce(entites_[i]);
    SPDLOG_INFO(
        "planet[{}] now at[{:.3f},{:.3f}] velocity[{:.3f},{:.3f}]={:.3f}, "
        "dS=[{:.3f},{:.3f}], dV=[{:.3f},{:.3f}], border_bouce[{}]",
        i, pos.x, pos.y, entites_[i].velocity.force_axis_x_,
        entites_[i].velocity.force_axis_y_, entites_[i].velocity.Force(),
        deltas[i][0], deltas[i][1], deltas[i][2], deltas[i][3], bb_ret);
  }

  // calculate predictions
  for (auto i = 0u; i < entites_.size(); ++i) {
    predicts_[i].clear();
    auto future_entities = entites_;
    for (auto j = 0u; j < FLAGS_predict_cnt; ++j) {
      auto &ent_i = future_entities[i];
      double s_x = ent_i.velocity.force_axis_x_ * kIntervalSec;
      double s_y = ent_i.velocity.force_axis_y_ * kIntervalSec;
      double v_x = 0;
      double v_y = 0;
      for (auto k = 0u; k < entites_.size(); ++k) {
        if (k == i) {
          continue;
        }
        const auto &ent_j = future_entities[k];
        auto force = Gravity(ent_i.p, ent_j.p);
        auto diff_x = ent_j.p.Position().x - ent_i.p.Position().x;
        auto diff_y = ent_j.p.Position().y - ent_i.p.Position().y;
        force.force_axis_x_ *= (diff_x < 0 ? -1 : 1);
        force.force_axis_y_ *= (diff_y < 0 ? -1 : 1);

        auto tmp_sx = force.force_axis_x_ * kIntervalMillisecSquareHalf /
                      ent_i.p.Weight();
        auto tmp_sy = force.force_axis_y_ * kIntervalMillisecSquareHalf /
                      ent_i.p.Weight();

        auto tmp_vx = force.force_axis_x_ * kIntervalSec / ent_i.p.Weight();
        auto tmp_vy = force.force_axis_y_ * kIntervalSec / ent_i.p.Weight();

        s_x += tmp_sx;
        s_y += tmp_sy;
        v_x += tmp_vx;
        v_y += tmp_vy;
      }

      auto pos = ent_i.p.Position();
      pos.x += s_x;
      pos.y += s_y;
      predicts_[i].push_back(pos);
      ent_i.p.Move(pos);
      ent_i.velocity.force_axis_x_ += v_x;
      ent_i.velocity.force_axis_y_ += v_y;
    }
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
  auto pos = entity.p.Position();
  if (pos.x < -kHalfX) {
    SPDLOG_TRACE("hit left border[x({:.3f})<left_bder({})], speed[{:.3f}]",
                 pos.x, -kHalfX, entity.velocity.force_axis_x_);
    pos.x = -size_x - pos.x;
    entity.p.Move(pos);
    if (entity.velocity.force_axis_x_ < 0) {
      entity.velocity.force_axis_x_ *= -1;
    }
    return 0;
  } else if (pos.x > kHalfX) {
    SPDLOG_TRACE("hit right border[x({:.3f})>right_bder({})], speed[{:.3f}]",
                 pos.x, kHalfX, entity.velocity.force_axis_x_);
    pos.x = size_x - pos.x;
    entity.p.Move(pos);
    if (entity.velocity.force_axis_x_ > 0) {
      entity.velocity.force_axis_x_ *= -1;
    }
    return 2;
  } else if (pos.y < -kHalfY) {
    SPDLOG_TRACE("hit top border[y({:.3f})<top_bder({})], speed[{:.3f}]", pos.y,
                 -kHalfY, entity.velocity.force_axis_y_);
    pos.y = -size_y - pos.y;
    entity.p.Move(pos);
    if (entity.velocity.force_axis_y_ < 0) {
      entity.velocity.force_axis_y_ *= -1;
    }
    return 1;
  } else if (pos.y > kHalfY) {
    SPDLOG_TRACE("hit bottom border[y({:.3f})>bottom_bder({})], speed[{:.3f}]",
                 pos.y, kHalfY, entity.velocity.force_axis_y_);
    pos.y = size_y - pos.y;
    entity.p.Move(pos);
    if (entity.velocity.force_axis_y_ > 0) {
      entity.velocity.force_axis_y_ *= -1;
    }
    return 3;
  }

  return -1;
}
