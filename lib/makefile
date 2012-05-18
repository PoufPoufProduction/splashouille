CC=g++
CFLAGS=-g -W -Wall -ansi -DSDL_IMAGE=1
INCLUDES=-Iinc -I/usr/include -I/usr/include/SDL
LDFLAGS=-shared -lSDL -lSDL_mixer -lSDL_image -lconfig++
OBJS = obj/Engine.o obj/Object.o obj/Library.o obj/Event.o obj/Timeline.o obj/Crowd.o obj/Style.o obj/Fashion.o obj/Solid.o obj/Image.o obj/Animation.o obj/Sound.o obj/Map.o
DEP = inc/splashouille/Animation.hpp  inc/splashouille/Event.hpp    inc/splashouille/Object.hpp \
	  inc/splashouille/Crowd.hpp      inc/splashouille/Fashion.hpp  inc/splashouille/Solid.hpp   inc/splashouille/Timeline.hpp \
	  inc/splashouille/Defines.hpp    inc/splashouille/Image.hpp    inc/splashouille/Sound.hpp inc/splashouille/Map.hpp \
	  inc/splashouille/Engine.hpp     inc/splashouille/Library.hpp  inc/splashouille/Style.hpp \
	  inc/splashouilleImpl/Animation.hpp  inc/splashouilleImpl/Fashion.hpp  inc/splashouilleImpl/Solid.hpp \
	  inc/splashouilleImpl/Crowd.hpp      inc/splashouilleImpl/Image.hpp    inc/splashouilleImpl/Sound.hpp \
	  inc/splashouilleImpl/Map.hpp  \
	  inc/splashouilleImpl/Engine.hpp     inc/splashouilleImpl/Library.hpp  inc/splashouilleImpl/Style.hpp \
	  inc/splashouilleImpl/Event.hpp      inc/splashouilleImpl/Object.hpp   inc/splashouilleImpl/Timeline.hpp


all: libsplashouille.so libsplashouille.a

libsplashouille.so: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

libsplashouille.a: $(OBJS)
	ar rc $@ $^

obj/Engine.o : src/Engine.cpp $(DEP)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Object.o : src/Object.cpp $(DEP)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Library.o : src/Library.cpp $(DEP)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Timeline.o : src/Timeline.cpp $(DEP)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Event.o : src/Event.cpp $(DEP)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Crowd.o : src/Crowd.cpp $(DEP)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Style.o : src/Style.cpp $(DEP)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Fashion.o : src/Fashion.cpp $(DEP)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Solid.o : src/Solid.cpp $(DEP)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Image.o : src/Image.cpp $(DEP)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Animation.o : src/Animation.cpp $(DEP)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Sound.o : src/Sound.cpp $(DEP)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Map.o : src/Map.cpp $(DEP)
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

clean:
	@rm -f obj/*
	@rm -f libsplashouille.so libsplashouille.a

install:
	@rm -rf /usr/local/include/splashouille
	@cp -rf inc/splashouille /usr/local/include
	@cp libsplashouille.* /usr/local/lib
	@chmod 755 /usr/local/include/splashouille
	
