all: shader-load ocean

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT -lGLEW
else
	LIBS = -lGL -lGLU -lglut -lGLEW
endif

shader-load: shader-load.cpp shader-load.h
	$(CXX) -g -Wall -c shader-load.cpp $(LIBS)

ocean: ocean.cpp shader-load.o shader-load.h
	$(CXX) -g -Wall ocean.cpp shader-load.o -o ocean $(LIBS) 

.PHONY:
clean :
	rm -f ocean shader-load.o
