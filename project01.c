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

FILE *fptr;

float curScale = 1;
float prevX=512, prevY=512, prevZ=512;

GLboolean cpGenerate = true;

GLuint ctm_location;
// GLuint sun_ctm_location;
GLuint model_view_location;
GLuint projection_location;

int mazeSizeX, mazeSizeY;
int num_vertices = 0; // CALCULATE NUMBER OF TRIANGLES NEEDED USING NUMBER OF CUBES NEEDED
int sun_vertices = 36; // sun block's vertex count
int vertCount = 0;
int blockIndex = 0;	// global so you don't have to provide it to every call of placeBlock

mat4 ctm = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
mat4 model_view = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
mat4 projection = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
// mat4 sun_ctm = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
mat4 trans_matrix = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
int is_animating;
int num_steps;
int step_counter;

vec2 *tex_coords;
vec4 *positions;
// int **maze;

#define BLANK_CELL 0
#define CORNER_CELL 1
#define WALL_CELL 2

#define T_GRASS 0
#define T_GRAVEL 1
#define T_COBBLE 2
#define T_COBBLE_MOSSY 3
#define T_STONE_BRICK 4
#define T_STONE_BRICK_CRACKED 5
#define T_STONE_BRICK_MOSSY 6
#define T_BRICK 7
#define T_GRANITE_POLISHED 8
#define T_GRANITE_CRACKED 9
#define T_SANDSTONE 10
#define T_PLANK 11
#define T_DIRT 12

void setGrass(int blockIndex) {
	// front
	tex_coords[ (blockIndex * 36) + 0] = (vec2) {0.75, 1.00};
	tex_coords[ (blockIndex * 36) + 1] = (vec2) {0.50, 0.75};
	tex_coords[ (blockIndex * 36) + 2] = (vec2) {0.50, 1.00};
	tex_coords[ (blockIndex * 36) + 3] = (vec2) {0.75, 1.00};
	tex_coords[ (blockIndex * 36) + 4] = (vec2) {0.75, 0.75};
	tex_coords[ (blockIndex * 36) + 5] = (vec2) {0.50, 0.75};

	// right
	tex_coords[ (blockIndex * 36) + 6] = (vec2) {0.75, 1.00};
	tex_coords[ (blockIndex * 36) + 7] = (vec2) {0.50, 0.75};
	tex_coords[ (blockIndex * 36) + 8] = (vec2) {0.50, 1.00};
	tex_coords[ (blockIndex * 36) + 9] = (vec2) {0.75, 1.00};
	tex_coords[ (blockIndex * 36) + 10] = (vec2) {0.75, 0.75};
	tex_coords[ (blockIndex * 36) + 11] = (vec2) {0.50, 0.75};

	// top
	tex_coords[ (blockIndex * 36) + 12] = (vec2) {0.25, 0.25};
	tex_coords[ (blockIndex * 36) + 13] = (vec2) {0.00, 0.00};
	tex_coords[ (blockIndex * 36) + 14] = (vec2) {0.00, 0.25};
	tex_coords[ (blockIndex * 36) + 15] = (vec2) {0.25, 0.25};
	tex_coords[ (blockIndex * 36) + 16] = (vec2) {0.25, 0.00};
	tex_coords[ (blockIndex * 36) + 17] = (vec2) {0.00, 0.00};

	// bottom
	tex_coords[ (blockIndex * 36) + 18] = (vec2) {1.00, 1.00};
	tex_coords[ (blockIndex * 36) + 19] = (vec2) {0.75, 0.75};
	tex_coords[ (blockIndex * 36) + 20] = (vec2) {0.75, 1.00};
	tex_coords[ (blockIndex * 36) + 21] = (vec2) {1.00, 1.00};
	tex_coords[ (blockIndex * 36) + 22] = (vec2) {1.00, 0.75};
	tex_coords[ (blockIndex * 36) + 23] = (vec2) {0.75, 0.75};

	// back
	tex_coords[ (blockIndex * 36) + 24] = (vec2) {0.75, 1.00};
	tex_coords[ (blockIndex * 36) + 25] = (vec2) {0.50, 0.75};
	tex_coords[ (blockIndex * 36) + 26] = (vec2) {0.50, 1.00};
	tex_coords[ (blockIndex * 36) + 27] = (vec2) {0.75, 1.00};
	tex_coords[ (blockIndex * 36) + 28] = (vec2) {0.75, 0.75};
	tex_coords[ (blockIndex * 36) + 29] = (vec2) {0.50, 0.75};

	// left
	tex_coords[ (blockIndex * 36) + 30] = (vec2) {0.75, 1.00};
	tex_coords[ (blockIndex * 36) + 31] = (vec2) {0.50, 0.75};
	tex_coords[ (blockIndex * 36) + 32] = (vec2) {0.50, 1.00};
	tex_coords[ (blockIndex * 36) + 33] = (vec2) {0.75, 1.00};
	tex_coords[ (blockIndex * 36) + 34] = (vec2) {0.75, 0.75};
	tex_coords[ (blockIndex * 36) + 35] = (vec2) {0.50, 0.75};
}

void setTexture(int texture) {
	// grass blocks are weird they need three separate textures, hard code that shit
	// bamboo blocks are too but we are realistically never gonna use them so whatever
	if (texture == T_GRASS) {
		setGrass(blockIndex);
		return;
	}

	float texX = 0;
	float texY = 0;
	switch (texture)
	{
	case T_GRAVEL:
		texX = 0.25;
		texY = 0.00;
		break;
	case T_COBBLE:
		texX = 0.50;
		texY = 0.00;
		break;
	case T_COBBLE_MOSSY:
		texX = 0.75;
		texY = 0.00;
		break;
	case T_STONE_BRICK:
		texX = 0.00;
		texY = 0.25;
		break;
	case T_STONE_BRICK_CRACKED:
		texX = 0.25;
		texY = 0.25;
		break;
	case T_STONE_BRICK_MOSSY:
		texX = 0.50;
		texY = 0.25;
		break;
	case T_BRICK:
		texX = 0.75;
		texY = 0.25;
		break;
	case T_GRANITE_POLISHED:
		texX = 0.00;
		texY = 0.50;
		break;
	case T_GRANITE_CRACKED:
		texX = 0.25;
		texY = 0.50;
		break;
	case T_SANDSTONE:
		texX = 0.50;
		texY = 0.50;
		break;
	case T_PLANK:
		texX = 0.75;
		texY = 0.50;
		break;
	case T_DIRT:
		texX = 0.75;
		texY = 0.75;
		break;
	
	default:
		break;
	}

	// front
	tex_coords[ (blockIndex * 36) + 0] = (vec2) {texX + 0.25, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 1] = (vec2) {texX, texY};
	tex_coords[ (blockIndex * 36) + 2] = (vec2) {texX, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 3] = (vec2) {texX + 0.25, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 4] = (vec2) {texX + 0.25, texY};
	tex_coords[ (blockIndex * 36) + 5] = (vec2) {texX, texY};

	// right
	tex_coords[ (blockIndex * 36) + 6] = (vec2) {texX + 0.25, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 7] = (vec2) {texX, texY};
	tex_coords[ (blockIndex * 36) + 8] = (vec2) {texX, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 9] = (vec2) {texX + 0.25, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 10] = (vec2) {texX + 0.25, texY};
	tex_coords[ (blockIndex * 36) + 11] = (vec2) {texX, texY};

	// top
	tex_coords[ (blockIndex * 36) + 12] = (vec2) {texX + 0.25, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 13] = (vec2) {texX, texY};
	tex_coords[ (blockIndex * 36) + 14] = (vec2) {texX, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 15] = (vec2) {texX + 0.25, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 16] = (vec2) {texX + 0.25, texY};
	tex_coords[ (blockIndex * 36) + 17] = (vec2) {texX, texY};

	// bottom
	tex_coords[ (blockIndex * 36) + 18] = (vec2) {texX + 0.25, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 19] = (vec2) {texX, texY};
	tex_coords[ (blockIndex * 36) + 20] = (vec2) {texX, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 21] = (vec2) {texX + 0.25, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 22] = (vec2) {texX + 0.25, texY};
	tex_coords[ (blockIndex * 36) + 23] = (vec2) {texX, texY};

	// back
	tex_coords[ (blockIndex * 36) + 24] = (vec2) {texX + 0.25, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 25] = (vec2) {texX, texY};
	tex_coords[ (blockIndex * 36) + 26] = (vec2) {texX, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 27] = (vec2) {texX + 0.25, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 28] = (vec2) {texX + 0.25, texY};
	tex_coords[ (blockIndex * 36) + 29] = (vec2) {texX, texY};

	// left
	tex_coords[ (blockIndex * 36) + 30] = (vec2) {texX + 0.25, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 31] = (vec2) {texX, texY};
	tex_coords[ (blockIndex * 36) + 32] = (vec2) {texX, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 33] = (vec2) {texX + 0.25, texY + 0.25};
	tex_coords[ (blockIndex * 36) + 34] = (vec2) {texX + 0.25, texY};
	tex_coords[ (blockIndex * 36) + 35] = (vec2) {texX, texY};
}

// To make this behave like WORLD x,y,z coordinates, swap the y and z coordinates when you call this!
void placeBlock(int blockX, int blockY, int blockZ, int texture) {
	// front
	positions[ (blockIndex * 36) + 0 ] = (vec4) { blockX + (0.5), blockY + (-0.5),  blockZ + (0.5),  1.0};
	positions[ (blockIndex * 36) + 1 ] = (vec4) { blockX + (-0.5), blockY + (0.5),  blockZ + (0.5),  1.0};
	positions[ (blockIndex * 36) + 2 ] = (vec4) { blockX + (-0.5), blockY + (-0.5),  blockZ + (0.5),  1.0};
	positions[ (blockIndex * 36) + 3 ] = (vec4) { blockX + (0.5), blockY + (-0.5),  blockZ + (0.5),  1.0};
	positions[ (blockIndex * 36) + 4 ] = (vec4) { blockX + (0.5), blockY + (0.5),  blockZ + (0.5),  1.0};
	positions[ (blockIndex * 36) + 5 ] = (vec4) { blockX + (-0.5), blockY + (0.5),  blockZ + (0.5),  1.0};
	// right
	positions[ (blockIndex * 36) + 6 ] = (vec4)  {  blockX + (0.5), blockY + (-0.5), blockZ + (-0.5),  1.0};
	positions[ (blockIndex * 36) + 7 ] = (vec4)  {  blockX + (0.5), blockY + (0.5), blockZ + (0.5),  1.0};
	positions[ (blockIndex * 36) + 8 ] = (vec4)  {  blockX + (0.5), blockY + (-0.5), blockZ + (0.5),  1.0};
	positions[ (blockIndex * 36) + 9 ] = (vec4)  {  blockX + (0.5), blockY + (-0.5), blockZ + (-0.5),  1.0};
	positions[ (blockIndex * 36) + 10 ] = (vec4) {  blockX + (0.5), blockY + (0.5), blockZ + (-0.5),  1.0};
	positions[ (blockIndex * 36) + 11 ] = (vec4) {  blockX + (0.5), blockY + (0.5), blockZ + (0.5),  1.0};

	// top
	positions[ (blockIndex * 36) + 12 ] = (vec4) {  blockX + (0.5), blockY + (0.5), blockZ + (0.5),  1.0};
	positions[ (blockIndex * 36) + 13 ] = (vec4) {  blockX + (-0.5), blockY + (0.5), blockZ + (-0.5),  1.0};
	positions[ (blockIndex * 36) + 14 ] = (vec4) {  blockX + (-0.5), blockY + (0.5), blockZ + (0.5),  1.0};
	positions[ (blockIndex * 36) + 15 ] = (vec4) {  blockX + (0.5), blockY + (0.5), blockZ + (0.5),  1.0};
	positions[ (blockIndex * 36) + 16 ] = (vec4) {  blockX + (0.5), blockY + (0.5), blockZ + (-0.5),  1.0};
	positions[ (blockIndex * 36) + 17 ] = (vec4) {  blockX + (-0.5), blockY + (0.5), blockZ + (-0.5),  1.0};	

	// bottom
	positions[ (blockIndex * 36) + 18 ] = (vec4) {  blockX + (0.5), blockY + (-0.5), blockZ + (-0.5),  1.0};
	positions[ (blockIndex * 36) + 19 ] = (vec4) {  blockX + (-0.5), blockY + (-0.5), blockZ + (0.5),  1.0};
	positions[ (blockIndex * 36) + 20 ] = (vec4) {  blockX + (-0.5), blockY + (-0.5), blockZ + (-0.5),  1.0};
	positions[ (blockIndex * 36) + 21 ] = (vec4) {  blockX + (0.5), blockY + (-0.5), blockZ + (-0.5),  1.0};
	positions[ (blockIndex * 36) + 22 ] = (vec4) {  blockX + (0.5), blockY + (-0.5), blockZ + (0.5),  1.0};
	positions[ (blockIndex * 36) + 23 ] = (vec4) {  blockX + (-0.5), blockY + (-0.5), blockZ + (0.5),  1.0};

	// back
	positions[ (blockIndex * 36) + 24 ] = (vec4) { blockX + (-0.5), blockY + (-0.5), blockZ + (-0.5), 1.0};
	positions[ (blockIndex * 36) + 25 ] = (vec4) { blockX + (0.5), blockY + (0.5), blockZ + (-0.5), 1.0};
	positions[ (blockIndex * 36) + 26 ] = (vec4) { blockX + (0.5), blockY + (-0.5), blockZ + (-0.5), 1.0};
	positions[ (blockIndex * 36) + 27 ] = (vec4) { blockX + (-0.5), blockY + (-0.5), blockZ + (-0.5), 1.0};
	positions[ (blockIndex * 36) + 28 ] = (vec4) { blockX + (-0.5), blockY + (0.5), blockZ + (-0.5), 1.0};
	positions[ (blockIndex * 36) + 29 ] = (vec4) { blockX + (0.5), blockY + (0.5), blockZ + (-0.5), 1.0};

	// left
	positions[ (blockIndex * 36) + 30 ] = (vec4) {  blockX + (-0.5), blockY + (-0.5), blockZ + (0.5), 1.0};
	positions[ (blockIndex * 36) + 31 ] = (vec4) {  blockX + (-0.5), blockY + (0.5), blockZ + (-0.5), 1.0};
	positions[ (blockIndex * 36) + 32 ] = (vec4) {  blockX + (-0.5), blockY + (-0.5), blockZ + (-0.5), 1.0};
	positions[ (blockIndex * 36) + 33 ] = (vec4) {  blockX + (-0.5), blockY + (-0.5), blockZ + (0.5), 1.0};
	positions[ (blockIndex * 36) + 34 ] = (vec4) {  blockX + (-0.5), blockY + (0.5), blockZ + (0.5), 1.0};
	positions[ (blockIndex * 36) + 35 ] = (vec4) {  blockX + (-0.5), blockY + (0.5), blockZ + (-0.5), 1.0};

	setTexture(texture);

	blockIndex++;
}

void init(void)
{   
    // GLuint program = initShader("vshader.glsl", "fshader.glsl");
    // glUseProgram(program);

    srand(time(NULL)); // Sets the random seed for RNG

	// numBlocks = actualMazeX * actualMazeY * (3 = mazeHeightZ)
	int num_blocks = (mazeSizeX * 2 + 1) * (mazeSizeY * 2 + 1) * 3 + 1;
	num_vertices = 36 * (num_blocks + 1);	// +1 for sun_block!

    // malloc()ate for blocks and texture coordinates
    positions = (vec4 *) malloc(sizeof(vec4) * num_vertices);
    tex_coords = (vec2 *) malloc(sizeof(vec2) * num_vertices);

	int x_offset = (mazeSizeX * 2 + 1) / 2;
	int y_offset = (mazeSizeY * 2 + 1) / 2;

	for (int y = 0; y < (2 * mazeSizeY) + 1; y++) {
		for (int x = 0; x < (2 * mazeSizeX) + 1; x++) {
			for (int z = 0; z < 3; z++) {	// looping to 3 for maze height

				// using RNG to skip blocks!
				if (z == 1) {			// layer 2 - very low likelihood of skipping block
					if (randInRange(1, 100) < 5 ) {	// 5% chance
						continue;
					}
				} else if (z == 2) {	// layer 3 - moderate likelihood to skip
					if (randInRange(1, 100) < 25 ) {	// 25% chance
						continue;
					}
				}

				int cell = maze[y][x]; 		// getting current cell
				if (cell == BLANK_CELL) {
					// blank cell - do nothing!
				} else if (cell == CORNER_CELL) {
					placeBlock(x - x_offset, z, y - y_offset, randInRange(4, 6));
				} else if (cell == WALL_CELL) {
					placeBlock(x - x_offset, z, y - y_offset, randInRange(2, 3));
				}
			}
		}
	}

    float numTriangles = num_vertices/3.0;

    printf("[textureTemplate] num_vertices: %i\n", num_vertices);

    int tex_width = 64;
    int tex_height = 64;
    GLubyte my_texels[tex_width][tex_height][3];

	// code copied from lab 6 - testing
    FILE *fp = fopen("textures02.raw", "r");
    if(fp != NULL)
	printf("[textureTemplate] Successfully open a texture file.\n");
    fread(my_texels, tex_width * tex_height * 3, 1, fp);
    fclose(fp);

    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLuint mytex[1];
    glGenTextures(1, mytex);
    glBindTexture(GL_TEXTURE_2D, mytex[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, my_texels);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    int param;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &param);
    
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * num_vertices + sizeof(vec2) * num_vertices, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * num_vertices, positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * num_vertices, sizeof(vec2) * num_vertices, tex_coords);

    // Goes through and does all the vertex positions
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (0));
	
	// same but for texture coords
    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (sizeof(vec4) * num_vertices));

	// assigning all important uniforms
    ctm_location = glGetUniformLocation(program, "ctm");
    printf("[textureTemplate] ctm_location: %i\n", ctm_location);
    // sun_ctm_location = glGetUniformLocation(program, "sun_ctm");
    // printf("[textureTemplate] sun_ctm_location: %i\n", sun_ctm_location);
    model_view_location = glGetUniformLocation(program, "model_view");
    printf("[textureTemplate] model_view_location: %i\n", model_view_location);
    projection_location = glGetUniformLocation(program, "projection");
    printf("[textureTemplate] projection_location: %i\n", projection_location);
    
    GLuint texture_location = glGetUniformLocation(program, "texture");
    printf("[textureTemplate] texture_location: %i\n", texture_location);
    glUniform1i(texture_location, 0);

    // Goes through and checks the depth of the objects and sets the background
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(1,0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT, GL_FILL); // Fills the front fully with color
    glPolygonMode(GL_BACK, GL_LINE); // The back of the object is only an outline

    // glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (float*) &sun_ctm); //TODO: Mouse added things
    // glDrawArrays(GL_TRIANGLES, 0, sun_vertices); // Draw triangles starting at the beginning index which is 0

    // glDrawArrays(GL_TRIANGLES, sun_vertices, num_vertices); // Draw triangles starting at the beginning index which is sun_vertices - USING 0 FOR SUN
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &ctm);
    glUniformMatrix4fv(model_view_location, 1, GL_FALSE, (GLfloat *) &model_view);
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, (GLfloat *) &projection);
    glDrawArrays(GL_TRIANGLES, 0, num_vertices); // Draw triangles starting at the beginning index which is sun_vertices - USING 0 FOR SUN

    glutSwapBuffers();
}

void look_at(float eye_x, float eye_y, float eye_z, float at_x, float at_y, float at_z, float up_x, float up_y, float up_z ) {
	/*
		ROTATION: 
			viewing 01 - sline 23
			viewing 02 - slide 19
			10-08-24 video
		TRANSLATION:
			to negation of eye_point
			mat4 look_translate = translate(-eye_x, -eye_y, -eye_z)???

		model_view = RT!!!
		model_view = matMult(R, T)!!!
	*/
	
	// constructing rotation matrix
	// USE 0 AT END FOR VECTORS AND 1 FOR POINTS IDIOT
	vec4 vpn = (vec4) {(eye_x - at_x), (eye_y - at_y), (eye_z - at_z), 0};
	vec4 n = normalize(vpn);

	vec4 v_up = (vec4) {up_x, up_y, up_z, 0};
	vec4 u_cross = crossProduct(v_up, n);
	vec4 u = normalize(u_cross);

	vec4 v_cross = crossProduct(n, u);
	vec4 v = normalize(v_cross);

	mat4 R = (mat4) {
		{u.x, v.x, n.x, 0},
		{u.y, v.y, n.y, 0},
		{u.z, v.z, n.z, 0},
		{0,   0,   0,   1}
	};

	// constructing translation matrix
	vec4 vrp = (vec4) {eye_x, eye_y, eye_z, 1};	// eye point
	mat4 T = translate(-vrp.x, -vrp.y, -vrp.z);

	// V = RT
	model_view = matMult(R, T);
}

void ortho(float left, float right, float bottom, float top, float near, float far) {
	mat4 temp;
	temp.x = (vec4) {(2/(right-left)), 0, 0, 0};
	temp.y = (vec4) {0, (2/(top - bottom)), 0, 0};
	temp.z = (vec4) {0, 0, (2/(near-far)), 0};
	temp.w = (vec4) {-((right+left)/(right-left)), -((top+bottom)/(top-bottom)), -((near+far)/(near-far)), 1};

	projection = temp;
}

void frustum(float left, float right, float bottom, float top, float near, float far) {
	// vieweing 05 - slide 13
	mat4 temp;

	temp.x = (vec4) {(-2 * near)/(right - left), 0, 0, 0};
	temp.y = (vec4) {0, ((-2 * near)/(top - bottom)), 0, 0};
	temp.z = (vec4) {((left + right)/(right - left)), ((bottom + top)/(top - bottom)), ((near + far)/(far - near)), -1};
	temp.w = (vec4) {0, 0, ((-2 * near * far)/(far - near)), 0};

	projection = temp;
}

void mouse(int button, int state, int x, int y)
{
	look_at(0, 0, (2 * mazeSizeX) + 1, 0, 0, 0, 0, 1, 0);
	// ortho(2.5, -2.5, 2.5, -2.5, 2.5, -2.5);
	frustum(-2.5, 2.5, -2.5, 2.5, -2.5, 2.5);

	// look_at(-mazeSizeX, 2.5, -mazeSizeY, -mazeSizeX + 1, 2.5, -mazeSizeY + 1, 0, 3, 0);
	// frustum(-2.5, 2.5, -2.5, 2.5, -2.5, 2.5);

    if(button == GLUT_LEFT_BUTTON){
        prevX = (x*2.0/511.0)-1;
        prevY = -(y*2.0/511.0)+1;
        prevZ = sqrt(1-(prevX*prevX) - (prevY * prevY));
    }
    
    if(button == 3){ // Scroll wheel up
        mat4 scale_matrix = scale(1.02,1.02,1.02);
        ctm = matMult(scale_matrix, ctm);
    }
    if(button == 4){ // Scroll wheel down
        mat4 scale_matrix = scale(1/1.02,1/1.02,1/1.02);
        ctm = matMult(scale_matrix, ctm);
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
        ctm = matMult(rot_matrix, ctm);

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
    if(key == 'q') {
		#ifndef __APPLE__
    	glutLeaveMainLoop();
	#else
		exit(0);
	#endif
	} else if(key == 'i'){ // Scroll wheel up - ALSO ADDING KEY 'i' BECAUSE MACOS SUCKS
        mat4 scale_matrix = scale(1.02,1.02,1.02);
        ctm = matMult(scale_matrix, ctm);
    } else if(key == 'o'){ // Scroll wheel down - ALSO ADDING KEY 'O' BECAUSE MACOS SUCKS
        mat4 scale_matrix = scale(1/1.02,1/1.02,1/1.02);
        ctm = matMult(scale_matrix, ctm);
    }

    // glutPostRedisplay();
}

void idle(void){
	// if(is_animating){
	// 	if(step_counter == num_step) {
	// 		triangle_position = target_position;
	// 		ctm = translate(target-position.x, target_position.y, target_position.z);
	// 		is_animating = 0;
	// 	}
	// 	else{
	// 		// vec4 move_vector = v4v4_subtraction(target_position, triangle_position);
	// 		// vec4 delta = sv4_multiplication(float step_counter / num_steps, move_vector);
	// 		// vec4 temp_position = v4v4_addition(traingle_position, delta);
	// 		// ctm = translate(temp_position.x, temp_position.y, temp_position.z);
	// 		step_counter++; 
	// 	}
	// 	glutPostRedisplay;
	// }
}

int main(int argc, char **argv)
{
	// default size will be overridden if correct args are provided
	mazeSizeX = 5;
	mazeSizeY = 5;
	if (argc == 3) {	// args are in the format of ./programName X Y
		mazeSizeX = atoi(argv[1]);
		mazeSizeY = atoi(argv[2]);	
	}

	maze = getMaze(mazeSizeX, mazeSizeY);

	printMaze();

    printf("\n------------------------------------ Break Between Mazes ------------------------------------\n\n");

    // Add solve maze function that returns a maze as well, and another one that returns a 2d int array
    printSolution();

    // printf("\n------------------------------------ Break Between Mazes ------------------------------------\n\n");

    // solveMaze(0);
    // printSolution();    
	

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
	glutIdleFunc(idle);
    glutMainLoop(); // Makes the whole main function wait so the program does not terminate

    return 0;
}
