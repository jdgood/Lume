#MakeFile - Mike Buerli
CC = g++
CFLAGS=-Wall -lglut -lGL -g -lGLU -O4
LDFLAGS=
SHADER=./lib/libGLEW.so.1.5.7 -pthread
SOUNDF=-L"/usr/lib" ./bin/linux-gcc/libIrrKlang.so -pthread
SOUNDI=-I"./include"
ALL=lume
all: $(ALL)

lume2: lib/*.o model.o importMap.o tex.o obj.o gobj.o mobj.o orb.o player.o cam.o script.o hud.o sound.o shader.o FBO.o sprite.o shadow.o  sdata.o debug.o frustum.o gameState.o particle.o robot.o main.o trigger.o objective.o level.o objimport.o tree.o billboard2d.o
	$(CC) $(CFLAGS) -DGL_GLEXT_PROTOTYPES $(SOUNDF) $(LDFLAGS) -o $@ $^

lume: lib/*.o model.o importMap.o tex.o obj.o gobj.o mobj.o orb.o player.o cam.o script.o hud.o sound.o shader.o FBO.o sprite.o shadow.o sdata.o debug.o frustum.o gameState.o particle.o robot.o main.o trigger.o objective.o level.o objimport.o tree.o billboard2d.o
	$(CC) $(CFLAGS) $(SHADER) $(SOUNDF) $(LDFLAGS) -o $@ $^

main.o: src/main.cpp src/*.h
	$(CC) $(CFLAGS) $(SOUNDI) $(LDFLAGS) -c $<

tex.o: src/tex.cpp src/tex.h src/SOIL.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $<

obj.o: src/obj.cpp src/obj.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $<

orb.o: src/orb.cpp src/orb.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOUNDI) -c $<

gobj.o: src/gobj.cpp src/gobj.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOUNDI) -c $<
	
mobj.o: src/mobj.cpp src/mobj.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOUNDI) -c $<

player.o: src/player.cpp src/player.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $<

particle.o: src/particle.cpp src/particle.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOUNDI) -c $<

robot.o: src/robot.cpp src/robot.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOUNDI) -c $<

cam.o: src/cam.cpp src/cam.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $<

sound.o: src/sound.cpp src/sound.h
	$(CC) $(CFLAGS) $(SOUNDI)  -c $< 

shader.o: src/shader.cpp src/shader.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< 
	
FBO.o: src/FBO.cpp src/FBO.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< 

importMap.o: src/importMap.cpp src/importMap.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOUNDI) -c $< 

model.o: src/model.cpp src/model.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< 

sprite.o: src/sprite.cpp src/sprite.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< 

shadow.o: src/shadow.cpp src/shadow.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< 

sdata.o: src/sdata.cpp src/sdata.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< 

hud.o: src/hud.cpp src/hud.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOUNDI) -c $< 

debug.o: src/debug.cpp src/debug.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOUNDI) -c $< 
	
frustum.o: src/frustum.cpp src/frustum.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOUNDI) -c $< 

gameState.o: src/gameState.cpp src/gameState.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOUNDI) -c $< 

script.o: src/script.cpp src/script.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOUNDI) -c $< 
	
trigger.o: src/trigger.cpp src/trigger.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< 

objective.o: src/objective.cpp src/objective.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< 

level.o: src/level.cpp src/level.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< 
	
objimport.o: src/objimport.cpp src/objimport.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< 

tree.o: src/tree.cpp src/tree.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $<
	
billboard2d.o: src/billboard2d.cpp src/billboard2d.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $<

clean:
	rm -rf core* *.o *.gch src/*.gch $(ALL)
