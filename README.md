## Introduction
a naive universal gravitation simulator inspired by **Osmos**, a great iOS game, and **three body**, a famous fiction.

planets in the defined limited universe(more like a box) moving around by the force field of their neighbors.

To see the demo [video](res/demo.gif).
## Get Started
1. install dependencies

 - spdlog
 - opengl
 - gflags
 - rapidjson

2. make

#### the Makefile is currently only support MacOS(for the `-framework` compiler options), adjust your own platform if needed.**

## TODO

- [x] flickering when redraw
- [x] bounce back at border
- [x] use density instead of explicit weight
- [ ] glimmering ball
- [x] history tracks
- [ ] predict tracks
- [ ] bug: edge point between recalc gap. Two planets moving towards each other, velocity should NOT consecutive while swapping their locations.