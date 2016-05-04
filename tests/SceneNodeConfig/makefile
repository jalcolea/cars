CC=g++
CFLAGS=-c -Wall -I../../include `sdl-config --cflags` `pkg-config --cflags mxml` `pkg-config --cflags OGRE` -std=c++11
LDFLAGS=-L/usr/local/lib -lpthread -lSDL -lSDL_sound -lSDL_mixer -lmxml -lOgreMain -lboost_system 
SOURCES=../../src/SceneNodeConfig.cpp ./main.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=nodesConfig_tester

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
#	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) ../src/*.o *.o

