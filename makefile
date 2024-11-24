# LIBS = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL -D GL_SILENCE_DEPRECATION

# project01: project01.c initShader.o myLib.o
# 	gcc -o project01 project01.c initShader.o myLib.o $(LIBS)

# initShader.o: initShader.c initShader.h
# 	gcc -c initShader.c

# tempLib.o: myLib.h myLib.c
# 	gcc -c myLib.c

# OPTIONS = -framework GLUT -framework OpenGL
# DEFINES = -D GL_SILENCE_DEPRECATION

LIBS = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL

project01: project01.c initShader.o myLib.o maze.o fshader.glsl
	gcc -o project01 -g project01.c initShader.o myLib.o maze.o $(LIBS)

initShader.o: initShader.c initShader.h
	gcc -c initShader.c

myLib.o: myLib.h myLib.c
	gcc -c myLib.c 

maze.o: maze.h maze.c
	gcc -c maze.c
