SRC = $(wildcard *.cc)
HDRS = $(wildcard *.h)

FRAMEWORK := -framework glut -framework opengl
CXXFLAGS := -std=c++11 
CFLAGS := -g3 -O0
grav2d: ${SRC} ${HDRS}
	g++ ${SRC} -o $@ ${CFLAGS} ${CXXFLAGS} ${FRAMEWORK}
