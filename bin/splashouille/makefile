CC=g++
CFLAGS=-g -W -Wall -ansi -DSDL=1
INCLUDES=-Isrc -I/usr/local/include -I/usr/include -I/usr/include/SDL
LDFLAGS=-L/usr/local/lib -ldl -rdynamic -lSDL -lSDL_image -lSDL_mixer -lconfig++ -lsplashouille 
EXEC=splashouille

all: $(EXEC)

$(EXEC): obj/Player.o
	$(CC) -o $@ $^ $(LDFLAGS)

obj/Player.o : src/Player.cpp src/Player.hpp
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

clean:
	@rm -f obj/*
	@rm -f splashouille
 
install:
	@cp splashouille /usr/local/bin
