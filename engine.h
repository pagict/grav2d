#ifndef _GRAV2D_ENGINE_H_
#define _GRAV2D_ENGINE_H_
#include <vector>

#include <GLUT/glut.h>

#include "opengl_utility.h"
#include "planet_2d.h"
#include "vector_force_2d.h"

enum TimerType {
  RECALC = 1,
  REDRAW = 2,
};

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
  int Recalc();
  int Redraw();
  int Keyboard(unsigned char c, int x, int y);
  int LeftClick(int x, int y);
  void Reshaped(int width, int height);

private:
  using MovingEntity = struct {
    Planet2D p;
    VectorForce2D velocity;
  };

  std::vector<MovingEntity> entites_;
  std::vector<std::vector<Position2D>> predicts_;

  std::vector<Planet2D> background_stars_;

  void GenerateBackgroundStars(unsigned count);
  void ShuffleBackground();

  int size_x = 1360;
  int size_y = 768;
  const char *kWindowTitle = "grav2d";

  // update the entity if border bouncing happens, and return
  // the border 0-3 correspond to left top right bottom
  // or -1 means no bouncing happened.
  int BorderBounce(decltype(entites_)::value_type &entity) const;

  bool is_paused_ = false;
};

extern Engine engine;
#endif // !_GRAV2D_ENGINE_H_