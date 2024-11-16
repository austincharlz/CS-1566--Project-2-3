LIBS = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL

project01: project01.c initShader.o myLib.o
	gcc -o project01 project01.c initShader.o myLib.o $(LIBS)

initShader.o: initShader.c initShader.h
	gcc -c initShader.c

tempLib.o: myLib.h myLib.c
	gcc -c myLib.c
