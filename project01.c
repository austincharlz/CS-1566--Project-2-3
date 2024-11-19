/*
 * template.c
 *
 *  Created on: September 10, 2024
 *      Author: Thumrongsak Kosiyatrakul
 */


#ifdef __APPLE__  // include Mac OS X verions of headers

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#else // non-Mac OS X operating systems

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#endif  // __APPLE__

#include "initShader.h"
#include "maze.h"
#include "myLib.h"
#include "stdlib.h"

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>


int num_vertices = 0; // CALCULATE NUMBER OF TRIANGLES NEEDED USING NUMBER OF CUBES NEEDED
mat4 current_transformation_matrix = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
float curScale = 1;
mat4 trans_matrix = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
GLuint ctm_location;
GLboolean cpGenerate = true;
float prevX=512, prevY=512, prevZ=512;
int vertCount = 0;
FILE *fptr;

int mazeSizeX, mazeSizeY;
// int **maze;

void init(void)
{
    
    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    srand(time(0)); // Sets the random seed for RNG

    vec4 *positions = (vec4 *) malloc(sizeof(vec4) * num_vertices); 

    float numTriangles = num_vertices/3.0;

    vec4 *colors = (vec4 *) malloc(sizeof(vec4) * num_vertices); // TODO: Change the number of colors

    // For loop to get the colors for each triangle
    for(int i = 0; i < numTriangles; i++){
        // Generates a random value for RGB
        float red = (float) rand() / RAND_MAX;
        float green = (float) rand() / RAND_MAX;
        float blue = (float) rand() / RAND_MAX;

        // Save the colors for the one triangle
        colors[i*3] = (vec4) {red, green, blue, 1.0};
        colors[(i*3)+1] = (vec4) {red, green, blue, 1.0};
        colors[(i*3)+2] = (vec4) {red, green, blue, 1.0};
    }
    
    GLuint vao;
	#ifndef __APPLE__
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
	#else
	glGenVertexArraysAPPLE(1, &vao);
    glBindVertexArrayAPPLE(vao);
	#endif

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * num_vertices + sizeof(vec4) * num_vertices, NULL, GL_STATIC_DRAW); // TODO: Might need to change amount of buffer data, should be amount of vertices and colors
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * num_vertices, positions); // TODO: Starts at index 0 and should be all the position vectors
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * num_vertices, sizeof(vec4) * num_vertices, colors); // TODO: Starts after all the position vectors and contains all the color vectors

    // Goes through and does all the vertex positions
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (0));

    ctm_location = glGetUniformLocation(program, "ctm");

    // Goes through and does all the vector colors
    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (sizeof(vec4) * num_vertices)); // TODO: change # of vecs

    // Goes through and checks the depth of the objects and sets the background
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0); // Pretty sure this is the background color
    glDepthRange(1,0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT, GL_FILL); // Fills the front fully with color
    glPolygonMode(GL_BACK, GL_LINE); // The back of the object is only an outline

    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (float*) &current_transformation_matrix); //TODO: Mouse added things
    
    glDrawArrays(GL_TRIANGLES, 0, num_vertices); // Draw triangles starting at the beginning index which is 0

    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON){
        prevX = (x*2.0/511.0)-1;
        prevY = -(y*2.0/511.0)+1;
        prevZ = sqrt(1-(prevX*prevX) - (prevY * prevY));
    }
    
    if(button == 3){ // Scroll wheel up
        mat4 scale_matrix = scale(1.02,1.02,1.02);
        current_transformation_matrix = matMult(scale_matrix, current_transformation_matrix);
    }
    if(button == 4){ // Scroll wheel down
        mat4 scale_matrix = scale(1/1.02,1/1.02,1/1.02);
        current_transformation_matrix = matMult(scale_matrix, current_transformation_matrix);
    }
    glutPostRedisplay();
}

void motion(int x, int y){ //TODO: Edit this to rotate the pyramid, add a rotate to the ctm
    
    // Calculate the new x,y,z values
    float x_coordinate = (x*2.0/511.0)-1;
    float y_coordinate = -(y*2.0/511.0)+1;
    float z_coordinate = sqrt(1-(x_coordinate * x_coordinate) - (y_coordinate * y_coordinate));

    // Get the two vectors
    vec4 origVec = (vec4){prevX, prevY, prevZ, 0};
    vec4 newVec = (vec4){x_coordinate, y_coordinate, z_coordinate, 0};

    // Calculate the angle between the two vectors
    // uses dot product to get angle between the vectors, then normalizes it 
    float theta = (180/M_PI) *acos(dotProduct(newVec, origVec)/(magOfVec(newVec)*magOfVec(origVec)));

    // if theta is NaN then the acos calculated incorrectly
    if(!isnan(theta)){
        // process of deriving the rotation matrix
        // used example in the "Geometric Objects and Transformations 05" slides as reference

        // calculate the about vector hen normalize it  
        vec4 aboutVec = crossProduct(origVec,newVec);
        float magOfAbout = magOfVec(aboutVec);
        aboutVec = (vec4){aboutVec.x/magOfAbout, aboutVec.y/magOfAbout, aboutVec.z/magOfAbout, 0};

        float dist = sqrt((aboutVec.y*aboutVec.y) + (aboutVec.z*aboutVec.z));

        // Get the default x rotation with 0 degrees
        // Then manually put in the rotation
        mat4 xRotation = rotateX(0);
        xRotation.y.y = aboutVec.z/dist;
        xRotation.y.z = -(aboutVec.y)/dist;
        xRotation.z.y = (aboutVec.y)/dist;
        xRotation.z.z = aboutVec.z/dist;

        // Get the transposition of the x rotation 
        mat4 xRotation_neg = matTrans(xRotation);

        // Default y rotation is inverted as OpenGL goes downward instead
        mat4 yRotation_neg = rotateY(0);
        yRotation_neg.x.x = dist;
        yRotation_neg.x.z = -aboutVec.x;
        yRotation_neg.z.x = aboutVec.x;
        yRotation_neg.z.z = dist;

        // Gets the correct y rotation the right way
        mat4 yRotation = matTrans(yRotation_neg);

        // matMults to do get the rotation matrix by going backwards on slide 12 of Geometric 05
        mat4 rot_matrix = matMult(xRotation,matMult(yRotation_neg,matMult(rotateZ(theta),matMult(yRotation, xRotation_neg))));

        // matMult to add to the transformation matrix
        current_transformation_matrix = matMult(rot_matrix, current_transformation_matrix);

    }

    // // Setting the prev coords to new ones just calculated
    prevX = x_coordinate;
    prevY = y_coordinate;
    prevZ = z_coordinate;

    glutPostRedisplay();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    // Causes the program to terminate if the user presses q
    if(key == 'q')
#ifndef __APPLE__
    	glutLeaveMainLoop();
#else
		exit(0);
#endif

    // glutPostRedisplay();
}

int main(int argc, char **argv)
{

    //TODO: Ask user if they want computer programmed shapes or to read in a file input
    // int num;
    // printf("Enter 1 for computer generated shapes, or enter 2 to enter a filename: \n");
    // scanf("%d", &num);

    // if(num == 2){ // Read in the user input for the filename
    //     cpGenerate = false;
    //     printf("Please enter the filename: \n");
    //     char* fileName = (char*)malloc(100*sizeof(char));
    //     scanf("%s", fileName);
    //     fptr = fopen(fileName, "r");
    //     if(fptr == NULL){ // If pointer invalid then file could not open so close and exit program
    //         printf("Invalid filename or just could not open %s\n", fileName);
    //         free(fileName);
    //         return 1;
    //     }
    //     else{ // Else read in the first line to set the num_vertices then continue as normal for now
    //         char* inputSize = (char*)malloc(100*sizeof(char));
    //         fgets(inputSize, 100, fptr);
    //         num_vertices = atoi(inputSize);
    //         // printf("num_vertices:%d\n",num_vertices);
    //         // return 0;
    //     }
    // }
    // else{
    //     printf("Controls: Press 1 to see sphere, 2 for donut, 3 for taurus, Q to exit\n");
    // }

	// default size will be overridden if correct args are provided
	mazeSizeX = 10;
	mazeSizeY = 10;
	if (argc == 3) {	// args are in the format of ./programName X Y
		mazeSizeX = atoi(argv[1]);
		mazeSizeY = atoi(argv[2]);	
	}

	maze = getMaze(mazeSizeX, mazeSizeY);
	printMaze();

    printf("\n------------------------------------ Break Between Mazes ------------------------------------\n\n");

    // Add solve maze function that returns a maze as well, and another one that returns a 2d int array
    printSolution();

    printf("\n------------------------------------ Break Between Mazes ------------------------------------\n\n");

    solveMaze(0);
    printSolution();    
	

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512); // Creates a 512x512 pixel window
    glutInitWindowPosition(100,100); // Makes the window first appear 100 pixels down and to the right of the top left corner of screen
    glutCreateWindow("Project 2/3 - Minecraft Maze");
#ifndef __APPLE__
    glewInit();
#endif
	init();
    glutDisplayFunc(display); 
    glutKeyboardFunc(keyboard); // Checks for any keyboard inputs
    glutMouseFunc(mouse); // Checks for the mouse
    glutMotionFunc(motion);
    glutMainLoop(); // Makes the whole main function wait so the program does not terminate

    return 0;
}
