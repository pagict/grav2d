SRC = $(wildcard *.cc)
HDRS = $(wildcard *.h)

FRAMEWORK := -framework glut -framework opengl -lgflags
CXXFLAGS := -std=c++14
CFLAGS := -Os
INCL := -I/usr/local/include
grav2d: ${SRC} ${HDRS}
	g++ ${SRC} -o $@ ${CFLAGS} ${CXXFLAGS} ${FRAMEWORK} ${INCL}

clean:
	rm -f grav2d *.o