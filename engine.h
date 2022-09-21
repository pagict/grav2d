#ifndef _GRAV2D_ENGINE_H_
#define _GRAV2D_ENGINE_H_
#include "planet_2d.h"
#include "vector_force_2d.h"
#include <GLUT/glut.h>
#include <vector>
class Engine {
public:
  int EngineInit(int argc, char **argv);

  inline int Run() {
    glutMainLoop();
    return 0;
  }

  int Display();

  using MovingEntity = struct {
    Planet2D p;
    VectorForce2D velocity;
  };

  std::vector<MovingEntity> entites_;

  const int kSizeX = 760;
  const int kSizeY = 480;
  const char *kWindowTitle = "title";
};

extern Engine engine;
#endif // !_GRAV2D_ENGINE_H_