#  Final Project
#  -------------------
#  Makefile 
#

CC=g++ 
CFLAGS= -lGL -lGLU -lglut

ALL= tex model

all:	$(ALL)

md5model:
	$(CC) $(CFLAGS) -o $@ md5mesh.cpp md5anim.cpp

model:
	$(CC) $(CFLAGS) -o $@ main.cpp model.cpp model.h

modelstand:
	md5model guy.md5mesh

boxrun:
	md5model box.md5mesh boxanim_.md5anim

guyidle:
	md5model guy.md5mesh guy_idle.md5anim

guywalk:
	md5model guy.md5mesh guy_walk.md5anim

guyjump:
	md5model guy.md5mesh guy_jump.md5anim

guykick:
	md5model guy.md5mesh guy_kick.md5anim

clean:
	rm -f core* *.o *.gch $(ALL)

