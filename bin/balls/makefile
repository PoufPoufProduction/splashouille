CC=g++
CFLAGS=-g -W -Wall -ansi -DSDL=1
INCLUDES=-Isrc -I/usr/local/include -I/usr/include -I/usr/include/SDL
LDFLAGS=-L/usr/local/lib -ldl -rdynamic -lSDL -lSDL_image -lSDL_mixer -lsplashouille
EXEC=balls

all: $(EXEC)

$(EXEC): obj/Balls.o
	$(CC) -o $@ $^ $(LDFLAGS)

obj/Balls.o : src/Balls.cpp src/Balls.hpp
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

clean:
	@rm -f obj/*
	@rm -f balls
 
