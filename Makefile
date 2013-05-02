all: shader-load ocean

shader-load: shader-load.cpp shader-load.h
	$(CXX) -g -Wall -c shader-load.cpp 

ocean: ocean.cpp shader-load.o shader-load.h
	$(CXX) -g -Wall ocean.cpp shader-load.o -o ocean -lGL -lGLU -lglut -lGLEW 

.PHONY:
clean :
	rm -f ocean shader-load.o
