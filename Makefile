all: executable

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CXXFLAGS=-g -Wall -Wextra -Wno-unused-parameter -D_REENTRANT -fPIC
LIBS=-lm -lglut -lGLEW -lGLU -lGL
endif


ifeq ($(UNAME), Darwin)
LIBS=-lm -framework GLUT -framework OpenGL -framework Cocoa
CXXFLAGS=-g -Wall -Wextra -Wno-unused-parameter -D_REENTRANT -fPIC -Wno-deprecated-declarations
endif


executable: main.o glm.o glutils.o tga.o triangle_index.o vec2.o vec3.o shader.frag shader.vert
	g++ main.o glm.o glutils.o tga.o triangle_index.o vec2.o vec3.o -o executable ${LIBS}


clean: 
	rm -f *.o *~ 
