SRC = $(wildcard *.cc)
HDRS = $(wildcard *.h)
OBJS = $(SRC:.cc=.o)

FRAMEWORK := -framework glut -framework opengl -lgflags
FRAMEWORK += -lspdlog -lfmt -L/usr/local/lib/
CXXFLAGS := -std=c++14
CFLAGS := -Os
INCL := -I/usr/local/include
grav2d: ${OBJS} ${HDRS}
	g++ ${OBJS} -o $@ ${CFLAGS} ${CXXFLAGS} ${FRAMEWORK} ${INCL}

clean:
	rm -f grav2d *.o

dbg:
	echo ${OBJS}