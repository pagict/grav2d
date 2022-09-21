#include "engine.h"
#include "opengl_utility.h"
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <tuple>

Engine engine;

void EngineDisplay() { engine.Display(); }

int Engine::EngineInit(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
  glutInitWindowSize(kSizeX, kSizeY);
  glutInitWindowPosition(0, 0);
  glutCreateWindow(kWindowTitle);

  glClearColor(0.0, 0.0f, 0.0f, 1.0f);
  glColor3f(0.0f, 0.0f, 0.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(-kSizeX / 2, kSizeX / 2, -kSizeY / 2, kSizeY / 2);

  glutDisplayFunc(EngineDisplay);
  return 0;
}

int Engine::Display() {
  for (const auto en : entites_) {
    DrawCircle(en.p.origin, en.p.radius, std::make_tuple(1.0, 0.5, 0.4, 0.3));
  }
  return 0;
}