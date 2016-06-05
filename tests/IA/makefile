CC=g++
CFLAGS=-c -Wall -I../../include 
LDFLAGS=-L/usr/local/lib 
SOURCES=./main.cpp  ../../src/iamanager.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ia_tester

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
#	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE) ../src/*.o *.o

