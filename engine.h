#ifndef _GRAV2D_ENGINE_H_
#define _GRAV2D_ENGINE_H_
#include "planet_2d.h"
#include "vector_force_2d.h"
#include <GLUT/glut.h>
#include <vector>

#include "opengl_utility.h"

class Engine {
public:
  int EngineInit(int argc, char **argv);

  inline int Run() {
    glutMainLoop();
    return 0;
  }

public:
  int Display();
  int TimerFlush();

private:
  using MovingEntity = struct {
    Planet2D p;
    VectorForce2D velocity;
  };

  std::vector<MovingEntity> entites_ = {
      {.p = {.origin = {.x = 300, .y = 100}, .radius = 10, .weight = 3},
       .velocity = {0, 0}},
      {.p = {.origin = {.x = 0, .y = 0}, .radius = 50, .weight = 10},
       .velocity = {10, 10}}};

  const int kSizeX = 1360;
  const int kSizeY = 768;
  const unsigned kFlushIntervalMillisec = 100;
  const char *kWindowTitle = "title";
};

extern Engine engine;
#endif // !_GRAV2D_ENGINE_H_