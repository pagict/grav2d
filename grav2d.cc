#include "engine.h"

int main(int argc, char **argv) {
  extern Engine engine;
  engine.EngineInit(argc, argv);
  engine.Run();
}