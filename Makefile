all: shader-load libs ocean

shader-load: shader-load.cpp shader-load.h
	$(CXX) -c shader-load.cpp 

libs: vector.cpp vector.h complex.cpp complex.h
	$(CXX) -c vector.cpp
	$(CXX) -c complex.cpp

ocean: ocean.cpp shader-load.o shader-load.h vector.o vector.h complex.o complex.h
	$(CXX) -g -Wall ocean.cpp shader-load.o vector.o complex.o -o ocean -lGL -lGLU -lglut -lGLEW -lgsl

.PHONY:
clean :
	rm -f ocean shader-load.o
