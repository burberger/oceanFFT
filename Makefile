all: shader-load ocean

LIBS = -lGL -lGLU -lglut -lGLEW

shader-load: shader-load.cpp shader-load.h
	$(CXX) -g -Wall -c shader-load.cpp $(LIBS)

ocean: ocean.cpp shader-load.o shader-load.h
	$(CXX) -g -Wall ocean.cpp shader-load.o -o ocean $(LIBS) 

.PHONY:
clean :
	rm -f ocean shader-load.o
