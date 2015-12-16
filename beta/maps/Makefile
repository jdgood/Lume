#  Makefile for detab program assignment
#  Last Modified: Mon Jan  5 5:48:51 PST 2010
#  @author Jeffrey Good
CC=g++
CFLAGS=-lGL -lglut -lGLU
#-framework OpenGL -framework GLUT -framework Cocoa
LDFLAGS=

ALL=mapper

all:	$(ALL)

mapper: mapper.cpp SOIL.h ../lib/*.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf core* *.o *.gch $(ALL)


