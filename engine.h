#ifndef _GRAV2D_ENGINE_H_
#define _GRAV2D_ENGINE_H_
#include <vector>

#include <GLUT/glut.h>

#include "opengl_utility.h"
#include "planet_2d.h"
#include "vector_force_2d.h"

class Engine {
public:
  int EngineInit(int sx, int sy, RGBAf bgcolor);

  inline int Run() {
    glutMainLoop();
    return 0;
  }

  int AddPlanet(Planet2D &&p2d, VectorForce2D &&v2d);

public:
  int Display();
  int TimerFlush();

private:
  using MovingEntity = struct {
    Planet2D p;
    VectorForce2D velocity;
  };

  std::vector<MovingEntity> entites_;

  int size_x = 1360;
  int size_y = 768;
  const unsigned kFlushIntervalMillisec = 100;
  const char *kWindowTitle = "grav2d";
};

extern Engine engine;
#endif // !_GRAV2D_ENGINE_H_