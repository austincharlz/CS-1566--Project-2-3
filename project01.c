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

#include "stdlib.h"

#include "initShader.h"

#include <time.h>
#include "myLib.h"
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

// Total 1 triangle
int num_vertices = 50760; // Default number of vertices needed for all 3 Computer generated shapes
mat4 current_transformation_matrix = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
float curScale = 1;
mat4 trans_matrix = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
GLuint ctm_location;
GLboolean cpGenerate = true;
float prevX=512, prevY=512, prevZ=512;
int vertCount = 0;
int shapeToDisplay = 0; // 0 is sphere, 1 is donut, 2 is taurus
FILE *fptr;

// Each shapes starting pos
int sphereOffset = 0;
int donutOffset = 0;
int taurusOffset = 0;

void drawSphere(vec4 *positions, int offset){ // Takes 3888 vertices, NOPE NOW 3672

    // vec4 *positions = (vec4 *) malloc(sizeof(vec4) * num_vertices); 

    // float numTriangles = num_vertices/3.0;
    // printf("vertCountSphereStart:%d\n",vertCount);
    positions[0+offset] = (vec4){0.0,0.0,1.0,1.0}; // Origin point (very first point of the sphere)
    positions[1+offset] = mvMult(rotateY(10),positions[0+offset]); // bottom right square
    positions[2+offset] = mvMult(rotateX(-10),positions[0+offset]); // top left of square
    positions[3+offset] = positions[2+offset];
    positions[4+offset] = positions[1+offset];
    positions[5+offset] = mvMult(matMult(rotateY(10),rotateX(-10)),positions[0+offset]);
    vertCount += 6;

    // Top part of sphere // TODO: combine with the for loop underneath
    for(int i = 1; i < 36; i++){ // Creates the first band
        positions[(i*6)+offset] = mvMult(rotateY(10),positions[((i-1)*6)+offset]);
        positions[(i*6)+1+offset] = mvMult(rotateY(10),positions[(i-1)*6 +1+offset]);
        positions[(i*6)+2+offset] = mvMult(rotateY(10),positions[(i-1)*6 +2+offset]);
        positions[(i*6)+3+offset] = mvMult(rotateY(10),positions[(i-1)*6 +3+offset]);
        positions[(i*6)+4+offset] = mvMult(rotateY(10),positions[(i-1)*6 +4+offset]);
        positions[(i*6)+5+offset] = mvMult(rotateY(10),positions[(i-1)*6 +5+offset]);
        vertCount += 6;
    }

    // Top part of sphere
    // for(int k = 1; k < 9; k++){
    for(int k = 1; k < 8; k++){
        positions[(k*216)+offset] = mvMult(rotateX(-10*k),positions[0+offset]);
        positions[(k*216)+1+offset] = mvMult(rotateY(10),positions[k*216+offset]); // bottom right square
        positions[(k*216)+2+offset] = mvMult(rotateX(-10),positions[k*216+offset]); // top left of square
        positions[(k*216)+3+offset] = positions[(k*216)+2];
        positions[(k*216)+4+offset] = positions[(k*216)+1];
        positions[(k*216)+5+offset] = mvMult(matMult(rotateY(10),rotateX(-10)),positions[k*216+offset]);
        vertCount += 6;

        for(int n = 1; n <36;n++){
            positions[n*6+(216*k)+offset] = mvMult(rotateY(10),positions[(n-1)*6+(216*k)+offset]);
            positions[(n*6)+1+(216*k)+offset] = mvMult(rotateY(10),positions[(n-1)*6 +1+(216*k)+offset]);
            positions[(n*6)+2+(216*k)+offset] = mvMult(rotateY(10),positions[(n-1)*6 +2+(216*k)+offset]);
            positions[(n*6)+3+(216*k)+offset] = mvMult(rotateY(10),positions[(n-1)*6 +3+(216*k)+offset]);
            positions[(n*6)+4+(216*k)+offset] = mvMult(rotateY(10),positions[(n-1)*6 +4+(216*k)+offset]);
            positions[(n*6)+5+(216*k)+offset] = mvMult(rotateY(10),positions[(n-1)*6 +5+(216*k)+offset]);
            vertCount += 6;
        }
    }

    // Bottom part of sphere
    int posOffset = vertCount;
    // for(int k = 0; k < 9; k++){
    for(int k = 0; k < 8; k++){
        positions[(k*216)+posOffset] = mvMult(rotateX(10*k),positions[0+offset]);
        positions[(k*216)+1+posOffset] = mvMult(rotateY(-10),positions[k*216+posOffset]); // bottom right square
        positions[(k*216)+2+posOffset] = mvMult(rotateX(10),positions[k*216+posOffset]); // top left of square
        positions[(k*216)+3+posOffset] = positions[(k*216)+2+posOffset];
        positions[(k*216)+4+posOffset] = positions[(k*216)+1+posOffset];
        positions[(k*216)+5+posOffset] = mvMult(matMult(rotateY(-10),rotateX(10)),positions[k*216+posOffset]);
        vertCount += 6;

        for(int n = 1; n <36;n++){
            positions[n*6+(216*k)+posOffset] = mvMult(rotateY(-10),positions[(n-1)*6+(216*k)+posOffset]);
            positions[(n*6)+1+(216*k)+posOffset] = mvMult(rotateY(-10),positions[(n-1)*6 +1+(216*k)+posOffset]);
            positions[(n*6)+2+(216*k)+posOffset] = mvMult(rotateY(-10),positions[(n-1)*6 +2+(216*k)+posOffset]);
            positions[(n*6)+3+(216*k)+posOffset] = mvMult(rotateY(-10),positions[(n-1)*6 +3+(216*k)+posOffset]);
            positions[(n*6)+4+(216*k)+posOffset] = mvMult(rotateY(-10),positions[(n-1)*6 +4+(216*k)+posOffset]);
            positions[(n*6)+5+(216*k)+posOffset] = mvMult(rotateY(-10),positions[(n-1)*6 +5+(216*k)+posOffset]);
            vertCount += 6;
        }
    }  

    // Now create the bottom and top part of the spheres
    vec4 * top = (vec4 *) malloc(sizeof(vec4)*36);
    vec4 * bottom = (vec4 *) malloc(sizeof(vec4)*36);
    top[0] = mvMult(rotateX(-70),positions[0+offset]);
    bottom[0] = mvMult(rotateX(80),positions[0+offset]);
    for(int i = 1; i < 36; i++){// Gets the array for both top and bottom parts of the sphere
        top[i] = mvMult(rotateY(10), top[i-1]); 
        bottom[i] = mvMult(rotateY(10), bottom[i-1]);
    }

    int offset2 = vertCount;
    for(int i = 0; i < 36; i++){
        if(i==35){
            positions[offset2+(i*3)] = top[i];
            positions[offset2+(i*3)+1] = top[0];
            positions[offset2+(i*3)+2] = (vec4){0.0,1.0,0.0,1.0};
            // printf("AfterCreatingCircle:%d",(offset+(i*3)+2));
        }
        else{
            positions[offset2+(i*3)] = top[i]; // Bottom left of triangle
            positions[offset2+(i*3)+1] = top[i+1]; // Bottom right of triangle
            positions[offset2+(i*3)+2] = (vec4){0.0,1.0,0.0,1.0}; // center of circle at the top
        }
        vertCount+=3;
    }

    offset2 = vertCount;
    for(int i = 0; i < 36; i++){
        if(i==35){
            positions[offset2+(i*3)+1] = bottom[i];
            positions[offset2+(i*3)] = bottom[0];
            positions[offset2+(i*3)+2] = (vec4){0.0,-1.0,0.0,1.0};
            // printf("AfterCreatingCircle:%d",(offset+(i*3)+2));
        }
        else{
            positions[offset2+(i*3)+1] = bottom[i]; // Bottom left of triangle
            positions[offset2+(i*3)] = bottom[i+1]; // Bottom right of triangle
            positions[offset2+(i*3)+2] = (vec4){0.0,-1.0,0.0,1.0}; // center of circle at the bottom
        }
        vertCount+=3;
    }

    // printf("VertCountSphereEnd:%d\n",vertCount);
}

void drawDonut(vec4 * positions, int offset){ // Takes 7776 vertices
    // printf("vertCountDonutStart:%d\n",vertCount);    

    vec4 *arr1 = (vec4 *) malloc(sizeof(vec4) * 36);
    vec4 *arr2 = (vec4 *) malloc(sizeof(vec4) * 36);

    float numTriangles = num_vertices/3.0;

    arr1[0] = (vec4){0.25,0.0,0.0,1.0}; // Origin point (very first point of the sphere)
    for(int i = 1; i < 36; i++){ // Rotate around to get one band of vertexes
        arr1[i] = mvMult(rotateZ(-10),arr1[i-1]);
    }
    for(int i = 0; i < 36; i++){ // now translate the whole array over
        arr1[i] = mvMult(translate(0.5,0.0,0.0),arr1[i]);
    }

    for(int i = 0; i < 36; i++){ // Now makes a second array to rotate 10 degrees from where the first points are created
        arr2[i] = mvMult(rotateY(10),arr1[i]);
    }

    // Connect the 2 arrays together to create the first band
    for(int i = 0; i < 36; i++){
        if(i==35){
            positions[i*6+offset] = arr1[i]; // Bottom left
            positions[(i*6)+2+offset] = arr2[i]; // Bottom right
            positions[(i*6)+1+offset] = arr1[0]; // Top left
            positions[(i*6)+3+offset] = arr2[i]; // bottom right
            positions[(i*6)+4+offset] = arr1[0]; // top left
            positions[(i*6)+5+offset] = arr2[0]; // Top right
            // double finalPos = (i*6)+5;
            // printf("Final position in the posArr = %f",finalPos);
        }
        else{
            positions[i*6+offset] = arr1[i]; // Bottom left
            positions[(i*6)+2+offset] = arr2[i]; // Bottom right
            positions[(i*6)+1+offset] = arr1[i+1]; // Top left
            positions[(i*6)+3+offset] = arr2[i]; // bottom right of square
            positions[(i*6)+4+offset] = arr1[i+1]; // top left of square
            positions[(i*6)+5+offset] = arr2[i+1]; // top right of square
        }
        vertCount+=6;
        
    }
    
    // Now rotate the whole entire band 360 degrees around to get a full donut
    for(int i = 1; i < 36; i++){

        for(int n = 0; n < 36; n++){
            positions[(216*i)+ (n*6)+offset] = mvMult(rotateY(10),positions[(216*(i-1)) + (n*6)+offset]);
            positions[(216*i)+ (n*6)+1+offset] = mvMult(rotateY(10),positions[(216*(i-1)) + (n*6) +1+offset]);
            positions[(216*i)+ (n*6)+2+offset] = mvMult(rotateY(10),positions[(216*(i-1)) + (n*6) +2+offset]);
            positions[(216*i)+ (n*6)+3+offset] = mvMult(rotateY(10),positions[(216*(i-1)) + (n*6) +3+offset]);
            positions[(216*i)+ (n*6)+4+offset] = mvMult(rotateY(10),positions[(216*(i-1)) + (n*6) +4+offset]);
            positions[(216*i)+ (n*6)+5+offset] = mvMult(rotateY(10),positions[(216*(i-1)) + (n*6) +5+offset]);
            vertCount+=6;
        }
    }
    // printf("vertCountDonutEnd:%d\n",vertCount);
}

void drawTaurus(vec4* positions, int offset){ // Takes about 39096 vertices

    // printf("vertCountTaurusStart:%d\n",vertCount);

    vec4 *arr1 = (vec4 *) malloc(sizeof(vec4) * 36);
    vec4 *arr2 = (vec4 *) malloc(sizeof(vec4) * 36);

    float numTriangles = num_vertices/3.0;

    arr1[0] = (vec4){0.25,0.0,0.0,1.0}; // Origin point (very first point of the sphere)
    for(int i = 1; i < 36; i++){ // Rotate around to get one band of vertexes
        arr1[i] = mvMult(rotateZ(-10),arr1[i-1]);
    }
    for(int i = 0; i < 36; i++){ // now translate the whole array over
        arr1[i] = mvMult(translate(0.5,-0.8,0.0),arr1[i]);
    }

    for(int i = 0; i < 36; i++){ // Now makes a second array to rotate 10 degrees from where the first points are created, but also cause it to translate up about 0.1 each time
        arr2[i] = mvMult(rotateY(10),arr1[i]);
        arr2[i] = mvMult(translate(0.0,0.0165,0.0),arr2[i]);
    }

    // Connect the 2 arrays together to create the first band
    for(int i = 0; i < 36; i++){
        if(i==35){
            positions[i*6+offset] = arr1[i]; // Bottom left
            positions[(i*6)+2+offset] = arr2[i]; // Bottom right
            positions[(i*6)+1+offset] = arr1[0]; // Top left
            positions[(i*6)+3+offset] = arr2[i]; // bottom right
            positions[(i*6)+4+offset] = arr1[0]; // top left
            positions[(i*6)+5+offset] = arr2[0]; // Top right
        }
        else{
            positions[i*6+offset] = arr1[i]; // Bottom left
            positions[(i*6)+2+offset] = arr2[i]; // Bottom right
            positions[(i*6)+1+offset] = arr1[i+1]; // Top left
            positions[(i*6)+3+offset] = arr2[i]; // bottom right of square
            positions[(i*6)+4+offset] = arr1[i+1]; // top left of square
            positions[(i*6)+5+offset] = arr2[i+1]; // top right of square
        }
        vertCount+=6;
        
    }
    
    // Now rotate the whole entire band 360 degrees around to get a full donut
    for(int i = 1; i < 36*5; i++){

        for(int n = 0; n < 36; n++){
            // Rotates the band once by 10 degrees
            positions[(216*i)+ (n*6)+offset] = mvMult(rotateY(10),positions[(216*(i-1)) + (n*6)+offset]);
            positions[(216*i)+ (n*6)+1+offset] = mvMult(rotateY(10),positions[(216*(i-1)) + (n*6) +1+offset]);
            positions[(216*i)+ (n*6)+2+offset] = mvMult(rotateY(10),positions[(216*(i-1)) + (n*6) +2+offset]);
            positions[(216*i)+ (n*6)+3+offset] = mvMult(rotateY(10),positions[(216*(i-1)) + (n*6) +3+offset]);
            positions[(216*i)+ (n*6)+4+offset] = mvMult(rotateY(10),positions[(216*(i-1)) + (n*6) +4+offset]);
            positions[(216*i)+ (n*6)+5+offset] = mvMult(rotateY(10),positions[(216*(i-1)) + (n*6) +5+offset]);

            vertCount+=6;
            // Then translate it up by 0.0165
            positions[(216*i)+ (n*6)+offset] = mvMult(translate(0.0,0.0165,0.0),positions[(216*i)+ (n*6)+offset]);
            positions[(216*i)+ (n*6)+1+offset] = mvMult(translate(0.0,0.0165,0.0),positions[(216*i)+ (n*6)+1+offset]);
            positions[(216*i)+ (n*6)+2+offset] = mvMult(translate(0.0,0.0165,0.0),positions[(216*i)+ (n*6)+2+offset]);
            positions[(216*i)+ (n*6)+3+offset] = mvMult(translate(0.0,0.0165,0.0),positions[(216*i)+ (n*6)+3+offset]);
            positions[(216*i)+ (n*6)+4+offset] = mvMult(translate(0.0,0.0165,0.0),positions[(216*i)+ (n*6)+4+offset]);
            positions[(216*i)+ (n*6)+5+offset] = mvMult(translate(0.0,0.0165,0.0),positions[(216*i)+ (n*6)+5+offset]);
            // if(n==35 & i==36*4){
                // double finalPos = (216*i)+(n*6)+5;
                // printf("n is 35 and i is 4Finalpos:%f\n",finalPos);
                // printVec(positions[(216*i)+(n*6)]);
                // printVec(positions[0]);
            // }
            // if(n==36 & i==36*5){
            //     double finalPos = (216*i)+(n*6)+5;
                // printf("n is 36 ad i is 5Finalpos:%f\n",finalPos);
            // }
            // if(n==35 & i==36*5){
            //     double finalPos = (216*i)+(n*6)+5;
                // printf("Finalpos:%f\n",finalPos);
            // }
        }
    }

    // printf("Count:%d\n",count); 

    // Now add the circles to fill in the ends of each side
    int offset2 = vertCount; // Found this by printing the very last point
    for(int i = 0; i < 36; i++){
        if(i==35){
            positions[offset2+(i*3)+1] = arr1[i];
            positions[offset2+(i*3)] = arr1[0];
            positions[offset2+(i*3)+2] = (vec4){0.5,-0.8,0.0,1.0};
            // printf("AfterCreatingCircle:%d",(offset+(i*3)+2));
        }
        else{
            positions[offset2+(i*3)+1] = arr1[i]; // Top right of triangle
            positions[offset2+(i*3)] = arr1[i+1]; // Top left of triangle
            positions[offset2+(i*3)+2] = (vec4){0.5,-0.8,0.0,1.0}; // center of circle
        }
        vertCount+=3;
    }

    // Update the offset2
    offset2 = vertCount;

    // Move the points from the first circle up to the top of the spring/taurs
    for(int i = 0; i<36; i++){
        arr1[i] = mvMult(translate(0.0,2.975,0.0),arr1[i]);
    }

    // Draw the second circle
    for(int i = 0; i < 36; i++){
        if(i==35){
            positions[offset2+(i*3)] = arr1[i];
            positions[offset2+(i*3)+1] = arr1[0];
            // positions[offset+(i*3)+2] = (vec4){0.5,2.2134,0.0,1.0};
            positions[offset2+(i*3)+2] = (vec4){0.5,2.15,0.0,1.0};
            // printf("AfterCreatingCircle:%d",(offset+(i*3)+2));
        }
        else{
            positions[offset2+(i*3)] = arr1[i]; // Top right of triangle
            positions[offset2+(i*3)+1] = arr1[i+1]; // Top left of triangle
            positions[offset2+(i*3)+2] = (vec4){0.5,2.15,0.0,1.0}; // center of circle
        }
        vertCount+=3;
    }

    // Translate and scale the spring/taurus down
    for(int i = offset; i < vertCount; i++){
        positions[i] = mvMult(matMult(translate(0.0,-0.35,0.0),scale(0.5,0.5,0.5)),positions[i]);
    }

    // printf("vertCountTaurusEnd:%d\n",vertCount);

}

// Generates a shape from file inputs
void fileGenerate(vec4 *positions){

    float xSum = 0.0, ySum = 0.0, zSum = 0.0; 

    GLboolean overOne = true;

    for(int i = 0; i < num_vertices; i++){ // Read in each file line and then create a vec4 for each line
        char* fileLine = (char*)malloc(100*sizeof(char));
        fgets(fileLine,  100, fptr);

        // After getting the fileLine now somehow split it up by the , and then parse into floats using atof()
        float x,y,z,w;
        sscanf(fileLine, "%f,%f,%f,%f", &x,&y,&z,&w); // Splits the line up by commas and then stores the floats
        // printf("Filelines:%f,%f,%f,%f\n", x,y,z,w);
        if(i==0 && abs(x) <= 1 && overOne){
            overOne = false;
        }

        
        // Divide by 100 to fit in the canonical view (1x1)
        if(overOne){
            x = x/100.0;
            y = y/100.0;
            z = z/100.0;
        }

        xSum+=x;
        ySum+=y;
        zSum+=z;

        positions[i] = (vec4){x,y,z,w}; // Create a vector with the float values
        // printVec(positions[i]);

    }

    // Averages the x, y, z values and then just moves those towards back to the origin
    float xAvg = xSum/num_vertices;
    float yAvg = ySum/num_vertices;
    float zAvg = zSum/num_vertices;

    for(int i = 0; i < num_vertices; i++){
        positions[i] = mvMult(translate(-xAvg, -yAvg, -zAvg), positions[i]);
    }

}

void init(void)
{
    
    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    srand(time(0)); // Sets the random seed for RNG

    vec4 *positions = (vec4 *) malloc(sizeof(vec4) * num_vertices); 

    float numTriangles = num_vertices/3.0;

    if(cpGenerate){
        drawSphere(positions,sphereOffset);
        donutOffset = vertCount;
        drawDonut(positions,donutOffset);
        taurusOffset = vertCount;
        drawTaurus(positions,taurusOffset); 
    }
    else{
        fileGenerate(positions);
    }    

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
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

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

    // TODO: Possibly just read in all shapes and then just calculate the offsets between all the computer generated shapes
    
    if(cpGenerate){
        if(cpGenerate && shapeToDisplay == 0){
            glDrawArrays(GL_TRIANGLES, sphereOffset, 3672);
        }
        if(cpGenerate && shapeToDisplay == 1){
            glDrawArrays(GL_TRIANGLES, donutOffset, 7776);
        }
        if(cpGenerate && shapeToDisplay == 2){
            glDrawArrays(GL_TRIANGLES, taurusOffset, 39096);
        } 
    }else{
        glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    }
    
    // glDrawArrays(GL_TRIANGLES, 0, num_vertices); // Draw triangles starting at the beginning index which is 0

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

    //if theta is NaN then the acos calculated incorrectly
    if(!isnan(theta)){
        //process of deriving the rotation matrix
        //used example in the "Geometric Objects and Transformations 05" slides as reference

        //calculate the about vector
        //Then normalize it  
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
    	glutLeaveMainLoop();

    if(cpGenerate && key == '1'){ // Sphere
        shapeToDisplay = 0;
        glutPostRedisplay();
    }
    if(cpGenerate && key == '2'){ // Donut
        shapeToDisplay = 1;
        glutPostRedisplay();
    }
    if(cpGenerate && key == '3'){ // Taurus
        shapeToDisplay = 2;
        glutPostRedisplay();
    }

    // glutPostRedisplay();
}

int main(int argc, char **argv)
{

    //TODO: Ask user if they want computer programmed shapes or to read in a file input
    int num;
    printf("Enter 1 for computer generated shapes, or enter 2 to enter a filename: \n");
    scanf("%d", &num);

    if(num == 2){ // Read in the user input for the filename
        cpGenerate = false;
        printf("Please enter the filename: \n");
        char* fileName = (char*)malloc(100*sizeof(char));
        scanf("%s", fileName);
        fptr = fopen(fileName, "r");
        if(fptr == NULL){ // If pointer invalid then file could not open so close and exit program
            printf("Invalid filename or just could not open %s\n", fileName);
            free(fileName);
            return 1;
        }
        else{ // Else read in the first line to set the num_vertices then continue as normal for now
            char* inputSize = (char*)malloc(100*sizeof(char));
            fgets(inputSize, 100, fptr);
            num_vertices = atoi(inputSize);
            // printf("num_vertices:%d\n",num_vertices);
            // return 0;
        }
    }
    else{
        printf("Controls: Press 1 to see sphere, 2 for donut, 3 for taurus, Q to exit\n");
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512); // Creates a 512x512 pixel window
    glutInitWindowPosition(100,100); // Makes the window first appear 100 pixels down and to the right of the top left corner of screen
    glutCreateWindow("Triangle");
    glewInit();
    init();
    glutDisplayFunc(display); 
    glutKeyboardFunc(keyboard); // Checks for any keyboard inputs
    glutMouseFunc(mouse); // Checks for the mouse
    glutMotionFunc(motion);
    glutMainLoop(); // Makes the whole main function wait so the program does not terminate

    return 0;
}
