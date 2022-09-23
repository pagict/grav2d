#include "engine.h"

#include "opengl_utility.h"
#include "planet_2d.h"
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <tuple>

Engine engine;

void EngineDisplay() { engine.Display(); }
void EngineTimer(int val) { engine.TimerFlush(); }

int Engine::EngineInit(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(kSizeX, kSizeY);
  glutInitWindowPosition(0, 0);
  glutCreateWindow(kWindowTitle);

  glClearColor(0.0, 0.0f, 0.0f, 1.0f);
  glColor3f(0.0f, 1.0f, 0.0f);
  glPointSize(1.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  const auto kMargin = 36u;
  gluOrtho2D(-(double)(kSizeX) / 2, (double)(kSizeX) / 2, -(double)kSizeY / 2 - kMargin,
             (double)(kSizeY) / 2 + kMargin);

  glutDisplayFunc(EngineDisplay);
  glutTimerFunc(kFlushIntervalMillisec, EngineTimer, 0);
  return 0;
}

int Engine::Display() {
  for (const auto en : entites_) {
    DrawCircle(en.p.origin, en.p.radius, std::make_tuple(1.0, 1.0, 0.0, 1.0));
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
      entites_[i].p.origin.x += forces.force_axis_x_ * kIntervalMillisecSquareHalf / entites_[i].p.weight +
                                entites_[i].velocity.force_axis_x_ * kIntervalSec;
      entites_[i].p.origin.y += forces.force_axis_y_ * kIntervalMillisecSquareHalf / entites_[i].p.weight +
                                entites_[i].velocity.force_axis_y_ * kIntervalSec;

      forces.force_axis_x_ *= -1;
      forces.force_axis_y_ *= -1;
      entites_[j].p.origin.x += forces.force_axis_x_ * kIntervalMillisecSquareHalf / entites_[j].p.weight +
                                entites_[j].velocity.force_axis_x_ * kIntervalSec;
      entites_[j].p.origin.y += forces.force_axis_y_ * kIntervalMillisecSquareHalf / entites_[j].p.weight +
                                entites_[j].velocity.force_axis_y_ * kIntervalSec;
    }
  }

  glutPostRedisplay();
  glutTimerFunc(kFlushIntervalMillisec, EngineTimer, 0);
  return 0;
}