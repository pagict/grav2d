SRC = $(wildcard *.cc)
HDRS = $(wildcard *.h)
OBJS = $(SRC:.cc=.o)

FRAMEWORK := -framework glut -framework opengl -lgflags
FRAMEWORK += -lspdlog -lfmt -L/opt/homebrew/lib
CXXFLAGS := -std=c++17
CXXFLAGS += -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
CFLAGS := -O0 -g3
CXXFLAGS += -I/opt/homebrew/include/
CXXFLAGS += ${CFLAGS}
grav2d: ${OBJS} ${HDRS}
	g++ ${OBJS} -o $@ ${CFLAGS} ${CXXFLAGS} ${FRAMEWORK} ${INCL}

clean:
	rm -f grav2d *.o
