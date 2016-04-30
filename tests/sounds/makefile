CC=g++
CFLAGS=-c -Wall -I../../include `sdl-config --cflags` `pkg-config --cflags mxml`
#CFLAGS=-c -Wall -I../../../pacman/include `sdl-config --cflags` `pkg-config --cflags mxml`
#LDFLAGS=`sdl-config --libs` `pkg-config --libs mxml` 
LDFLAGS=-L/usr/local/lib -lpthread -lSDL -lSDL_sound -lSDL_mixer -lmxml
SOURCES=../../src/sounds.cpp ./main.cpp 
#SOURCES=../../../pacman/src/sounds.cpp ./main.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sounds_tester

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
#	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) ../src/*.o *.o

