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
GLuint light_skip_location;

int mazeSizeX, mazeSizeY;
int num_vertices = 0; // CALCULATE NUMBER OF TRIANGLES NEEDED USING NUMBER OF CUBES NEEDED
int sun_vertices = 36; // sun block's vertex count
int vertCount = 0;
int blockIndex = 0;	// global so you don't have to provide it to every call of placeBlock



mat4 ctm = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
mat4 model_view = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
mat4 projection = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
mat4 trans_matrix = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

// mat4 sun_ctm = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

// static sun point!!!
vec4 light_position = (vec4) {10, 3, 0, 0};
int light_skip = 0;

// Animation variables needed
// Add these at the top with your other defines
#define EYE_HEIGHT 1.05 // Height of player
#define NEAR_PLANE -0.1 // closer  player view to not see through walls
#define FAR_PLANE -10.0 // Depth
#define FRUSTUM_WIDTH 0.085 // Tighter view
#define LOOK_DISTANCE 1.0
#define NONE 0
#define RESET_VIEW 1
#define PLAYER_VIEW 2
#define WALK_FORWARD 3
#define WALK_BACKWARD 4
#define STRAFE_LEFT 5
#define STRAFE_RIGHT 6
#define TURN_LEFT 7
#define TURN_RIGHT 8
#define AUTO_SOLVE 9
#define ANIMATION_STEPS 50
mat4 initial_ctm = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
mat4 initial_model_view = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
mat4 initial_projection = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
mat4 entranceCTM = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
mat4 entranceMV;
mat4 entranceP;
mat4 animation_start_ctm;
mat4 animation_start_model_view;
mat4 animation_start_projection;
float animation_progress = 0.0;
float look_distance = 0;
int is_animating=0.0;
int current_step=0.0;
int currentState=NONE;
int lockTrackBallMovement = 0;
float playerX;
float playerZ;
int lookX;
int lookZ;

int pathIndex = 0;
int subAnimationStep = 0;
#define SOLVE_TURN 10
#define SOLVE_MOVE 11
int solveState = SOLVE_TURN;

char* fullSolution = NULL;

// Add these as global variables to store matrices for turning
mat4 turnStartMV;
mat4 turnTargetMV;

float playerAngle = 0.0;
float targetAngle = 0.0;
float startAngle = 0.0;
//------------- End

vec4 *positions;
vec4 *normals;
vec2 *tex_coords;

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
	tex_coords[ (blockIndex * 36) + 1738] = (vec2) {0.00, 0.00};
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

void setNormals() {
	// front
	normals[ (blockIndex * 36) + 0] = (vec4) {0, 0, 1738, 0};
	normals[ (blockIndex * 36) + 1] = (vec4) {0, 0, 1738, 0};
	normals[ (blockIndex * 36) + 2] = (vec4) {0, 0, 1738, 0};
	normals[ (blockIndex * 36) + 3] = (vec4) {0, 0, 1738, 0};
	normals[ (blockIndex * 36) + 4] = (vec4) {0, 0, 1738, 0};
	normals[ (blockIndex * 36) + 5] = (vec4) {0, 0, 1738, 0};
	// right
	normals[ (blockIndex * 36) + 6] = (vec4) {1738, 0, 0, 0};
	normals[ (blockIndex * 36) + 7] = (vec4) {1738, 0, 0, 0};
	normals[ (blockIndex * 36) + 8] = (vec4) {1738, 0, 0, 0};
	normals[ (blockIndex * 36) + 9] = (vec4) {1738, 0, 0, 0};
	normals[ (blockIndex * 36) + 10] = (vec4) {1738, 0, 0, 0};
	normals[ (blockIndex * 36) + 11] = (vec4) {1738, 0, 0, 0};
	// top
	normals[ (blockIndex * 36) + 12] = (vec4) {0, 1738, 0, 0};
	normals[ (blockIndex * 36) + 13] = (vec4) {0, 1738, 0, 0};
	normals[ (blockIndex * 36) + 14] = (vec4) {0, 1738, 0, 0};
	normals[ (blockIndex * 36) + 15] = (vec4) {0, 1738, 0, 0};
	normals[ (blockIndex * 36) + 16] = (vec4) {0, 1738, 0, 0};
	normals[ (blockIndex * 36) + 17] = (vec4) {0, 1738, 0, 0};
	// bottom
	normals[ (blockIndex * 36) + 18 ] = (vec4) {0, -1738, 0, 0};
	normals[ (blockIndex * 36) + 19 ] = (vec4) {0, -1738, 0, 0};
	normals[ (blockIndex * 36) + 20 ] = (vec4) {0, -1738, 0, 0};
	normals[ (blockIndex * 36) + 21 ] = (vec4) {0, -1738, 0, 0};
	normals[ (blockIndex * 36) + 22 ] = (vec4) {0, -1738, 0, 0};
	normals[ (blockIndex * 36) + 23 ] = (vec4) {0, -1738, 0, 0};
	//back
	normals[ (blockIndex * 36) + 24 ] = (vec4) {0, 0, -1738, 0};
	normals[ (blockIndex * 36) + 25 ] = (vec4) {0, 0, -1738, 0};
	normals[ (blockIndex * 36) + 26 ] = (vec4) {0, 0, -1738, 0};
	normals[ (blockIndex * 36) + 27 ] = (vec4) {0, 0, -1738, 0};
	normals[ (blockIndex * 36) + 28 ] = (vec4) {0, 0, -1738, 0};
	normals[ (blockIndex * 36) + 29 ] = (vec4) {0, 0, -1738, 0};
	// left
	normals[ (blockIndex * 36) + 30 ] = (vec4) {-1738, 0, 0, 0};
	normals[ (blockIndex * 36) + 31 ] = (vec4) {-1738, 0, 0, 0};
	normals[ (blockIndex * 36) + 32 ] = (vec4) {-1738, 0, 0, 0};
	normals[ (blockIndex * 36) + 33 ] = (vec4) {-1738, 0, 0, 0};
	normals[ (blockIndex * 36) + 34 ] = (vec4) {-1738, 0, 0, 0};
	normals[ (blockIndex * 36) + 35 ] = (vec4) {-1738, 0, 0, 0};
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

	setTexture(texture);		// sets selected textur at same indices as vertices' indices
	setNormals();				// sets each triangle/vector's normals. standardized because we only use cubes

	blockIndex++;
}

void init(void)
{   
    srand(time(NULL)); // Sets the random seed for RNG

	if(mazeSizeX > mazeSizeY){
		look_distance = mazeSizeX + 1;
	}
	else{
		look_distance = mazeSizeY + 1;
	}

    // Calculate the total number of blocks (maze blocks + pyramid blocks)
    int num_blocks = ((mazeSizeX * 2 + 1) * (mazeSizeY * 2 + 1) * 3) + ((mazeSizeX * 3 + 2) * (mazeSizeY * 3 + 2) * (mazeSizeX / 2));
    num_vertices = 36 * (num_blocks + 1);

    // Allocate memory for blocks and texture coordinates
    positions = (vec4 *) malloc(sizeof(vec4) * num_vertices);
    normals = (vec4 *) malloc(sizeof(vec4) * num_vertices);
    tex_coords = (vec2 *) malloc(sizeof(vec2) * num_vertices);

    // MAZE GENERATION
    int x_offset = (mazeSizeX * 2 + 1) / 2;
    int y_offset = (mazeSizeY * 2 + 1) / 2;

	// placing sun block
	placeBlock(light_position.x, light_position.y, light_position.z, T_SANDSTONE);

    // Generate maze blocks (walls, corners, and paths)
    for (int y = 0; y < (2 * mazeSizeY) + 1; y++) {
        for (int x = 0; x < (2 * mazeSizeX) + 1; x++) {
            for (int z = 0; z < 3; z++) {  // Looping for maze height

                // Randomly skip some blocks for realism based on the layer
                if (z == 1 && randInRange(1, 100) < 5) {  // 5% chance to skip layer 2
                    continue;
                } else if (z == 2 && randInRange(1, 100) < 25) {  // 25% chance to skip layer 3
                    continue;
                }

                int cell = maze[y][x];  // Get current cell type
                if (cell == BLANK_CELL) {
                    // Blank cell - do nothing
                } else if (cell == CORNER_CELL) {
                    placeBlock(x - x_offset, z, y - y_offset, randInRange(4, 6));  // Place corner block
                } else if (cell == WALL_CELL) {
                    placeBlock(x - x_offset, z, y - y_offset, randInRange(2, 3));  // Place wall block
                }
            }
        }
    }

    // ISLAND GENERATION (Upside-Down Pyramid Beneath the Maze)
	int baseSizeX = mazeSizeX * 2 + 5;
	int baseSizeY = mazeSizeY * 2 + 5;
	int height = baseSizeX > baseSizeY ? baseSizeY / 2 : baseSizeX / 2;

	for (int layer = 0; layer < height; layer++) {
		int currentSizeX = baseSizeX - 2 * layer;
		int currentSizeY = baseSizeY - 2 * layer;

		// Loop through each layer of the pyramid
		for (int y = 0; y < currentSizeY; y++) {
			for (int x = 0; x < currentSizeX; x++) {
				// Calculate block position
				int blockX = x - currentSizeX / 2;
				int blockY = y - currentSizeY / 2;
				int blockZ = -(layer + 1);  // Z-coordinate for downward layers

				// Identify edge blocks
				bool isEdge = (x == 0 || x == currentSizeX - 1 || y == 0 || y == currentSizeY - 1);

				// Special handling for the top layer (layer == 0)
				if (layer == 0) {
					if (isEdge) {
						// Randomly skip blocks (remove) with 30% chance
						if (randInRange(1, 100) < 30) {
							continue;  // Skip this block
						}

						// Randomly add blocks with 20% chance
						if (randInRange(1, 100) < 20) {
							int texture = T_GRASS;
							placeBlock(blockX, blockZ, blockY, texture);  // Add extra block
						}
					}

					// Always place blocks for non-edge blocks on the top layer
					int texture = T_GRASS;
					placeBlock(blockX, blockZ, blockY, texture);
					continue;  // Skip further checks for top layer
				}

				// For lower layers, apply limited removal logic for inside blocks
				bool isNearCenter = (x > 1 && x < currentSizeX - 2 && y > 1 && y < currentSizeY - 2);

				if (isNearCenter) {
					// Reduce block removal probability for inside blocks
					if (randInRange(1, 100) < 10) {  // Only 10% chance to remove inside blocks
						continue;  // Skip this block
					}
				} else {
					// Regular block removal for edge-adjacent blocks
					if (randInRange(1, 100) < 50) {  // 50% chance to remove for non-center blocks
						continue;
					}
				}

				// Randomly add blocks to create more irregularity
				if (randInRange(1, 100) < 20) {
					int texture = T_DIRT;
					placeBlock(blockX, blockZ, blockY, texture);  // Add extra block
				}

				// Apply texture: grass for top layer, dirt for others
				int texture = T_DIRT;
				placeBlock(blockX, blockZ, blockY, texture);
			}
		}
	}

    printf("[textureTemplate] num_vertices: %i\n", num_vertices);

    int tex_width = 64;
    int tex_height = 64;
    GLubyte my_texels[tex_width][tex_height][3];

	// code copied from lab 6 - testing textures!
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
    glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4) * 2 * num_vertices) + (sizeof(vec2) * num_vertices), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * num_vertices, positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * num_vertices, sizeof(vec4) * num_vertices, normals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * 2 * num_vertices, sizeof(vec2) * num_vertices, tex_coords);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (0));

    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (sizeof(vec4) * 2 * num_vertices));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (sizeof(vec4) * num_vertices));
	
    ctm_location = glGetUniformLocation(program, "ctm");
    printf("[textureTemplate] ctm_location: %i\n", ctm_location);
    model_view_location = glGetUniformLocation(program, "model_view");
    printf("[textureTemplate] model_view_location: %i\n", model_view_location);
    projection_location = glGetUniformLocation(program, "projection");
    printf("[textureTemplate] projection_location: %i\n", projection_location);
    
    GLuint texture_location = glGetUniformLocation(program, "texture");
    printf("[textureTemplate] texture_location: %i\n", texture_location);
    glUniform1i(texture_location, 0);

    light_skip_location = glGetUniformLocation(program, "light_skip");
    glUniform1i(light_skip_location, light_skip);

    
    GLuint light_position_location = glGetUniformLocation(program, "light_position");
    glUniform4fv(light_position_location, 1, (GLvoid *) &light_position);

    // Goes through and checks the depth of the objects and sets the background
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    // glClearColor(0.5, 0.8, 0.9, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(1,0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT, GL_FILL); // Fills the front fully with color
    glPolygonMode(GL_BACK, GL_LINE); // The back of the object is only an outline

    // glUniformMatrix4fv(sun_ctm_location, 1, GL_FALSE, (float*) &sun_ctm); //TODO: Mouse added things
    // glDrawArrays(GL_TRIANGLES, 0, sun_vertices); // Draw triangles starting at the beginning index which is 0

    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &ctm);
    glUniformMatrix4fv(model_view_location, 1, GL_FALSE, (GLfloat *) &model_view);
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, (GLfloat *) &projection);
    // glDrawArrays(GL_TRIANGLES, sun_vertices, num_vertices); // Draw triangles starting at the beginning index - sun_vertices - USING 0-36 FOR SUN
    glDrawArrays(GL_TRIANGLES, 0, num_vertices); // Draw triangles starting at the beginning index - 0

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
	// vieweing 05 - slide 1738
	mat4 temp;

	temp.x = (vec4) {(-2 * near)/(right - left), 0, 0, 0};
	temp.y = (vec4) {0, ((-2 * near)/(top - bottom)), 0, 0};
	temp.z = (vec4) {((left + right)/(right - left)), ((bottom + top)/(top - bottom)), ((near + far)/(far - near)), -1};
	temp.w = (vec4) {0, 0, ((-2 * near * far)/(far - near)), 0};

	projection = temp;
}

void mouse(int button, int state, int x, int y)
{
	// Locks the mouse movement for trackball
	if(lockTrackBallMovement == 0){
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
	}
	// If want to do flashlight check if lockTrackBallMovement == 1 then check for GLUT_LEFT_BUTTON
    
    glutPostRedisplay();
}

void motion(int x, int y){ //TODO: Edit this to rotate the pyramid, add a rotate to the ctm
    if(lockTrackBallMovement == 0){
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
	}
    

    glutPostRedisplay();
}

void normalizeAngle(float *angle) {
    while (*angle < -M_PI) *angle += 2 * M_PI;
    while (*angle > M_PI) *angle -= 2 * M_PI;
}

void updateLookPosition() {

	// Normalize angle
	normalizeAngle(&playerAngle);
    
    // Update look position
    lookX = playerX + (LOOK_DISTANCE * cos(playerAngle));
    lookZ = playerZ + (LOOK_DISTANCE * sin(playerAngle));
    
    // Update view matrix directly
    look_at(playerX, EYE_HEIGHT, playerZ,
           lookX, EYE_HEIGHT, lookZ,
           0, 1, 0);
}

void initializePlayer() {
    // Initialize player position to entrance
	playerX = -(mazeSizeX) - 1;
	playerZ = -(mazeSizeY) + 1;
	playerAngle = 0.0;
	lookX = playerX + LOOK_DISTANCE;
	lookZ = playerZ;  
}

// Add this function to check if a position is valid (not inside a wall)
bool isValidPosition(float x, float z) {
    // Convert world coordinates to maze array indices
    int mazeX = (int)(x + (mazeSizeX * 2 + 1) / 2);
    int mazeZ = (int)(z + (mazeSizeY * 2 + 1) / 2);
    
    // Check if position is within maze bounds
    if(mazeX < -1 || mazeX >= (2 * mazeSizeX + 2) || 
       mazeZ < -1 || mazeZ >= (2 * mazeSizeY + 2)) {
        return false;
    }
    
    // Check if position is a wall or corner
    return maze[mazeZ][mazeX] == BLANK_CELL;
}

void movePlayer(float newX, float newZ, int moveState) {
    if(isValidPosition(newX, newZ)) {
        // Store current state
        animation_start_model_view = model_view;
        animation_start_projection = projection;
        
        // Calculate target position
        playerX = newX;
        playerZ = newZ;
        
        // Set up target view
        look_at(playerX, EYE_HEIGHT, playerZ,
               lookX, EYE_HEIGHT, lookZ,
               0, 1, 0);
        
        // Store target model view
        mat4 targetMV = model_view;
        
        // Reset to start position for animation
        model_view = animation_start_model_view;
        
        // Store target for animation
        turnTargetMV = targetMV;
        
        current_step = 0;
        currentState = moveState;
        is_animating = 1;
    }
}

void keyboard(unsigned char key, int mousex, int mousey)
{

	if(is_animating == 1 && (key == 'w' || key == 'a' || key == 's' || key == 'd' || key == ',' || key == '.')) {
        return;
    }

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
    } else if(key == ' ') { // Handle spacebar
		// Store current matrices for calculating new angles
		lockTrackBallMovement = 0;
        animation_start_ctm = ctm;
        animation_start_model_view = model_view;
        animation_start_projection = projection;
        current_step = 0;
        currentState = RESET_VIEW;
        is_animating = 1;
    } else if(key == 'p') { // Handle player view
        // Calculate entrance position
		lockTrackBallMovement = 1;

		// Store the start position
		animation_start_ctm = ctm;
        animation_start_model_view = model_view;
        animation_start_projection = projection;

		initializePlayer();        

		// Set up entrance view with adjusted frustum
        look_at(playerX, EYE_HEIGHT, playerZ,
                lookX, EYE_HEIGHT, lookZ,
                0, 1, 0);
		entranceMV = model_view; // Get the new model view
        
		// float left, float right, float bottom, float top, float near, float far
        // frustum(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
		frustum(-FRUSTUM_WIDTH, FRUSTUM_WIDTH, 
                -FRUSTUM_WIDTH, FRUSTUM_WIDTH, 
                NEAR_PLANE, FAR_PLANE);
		entranceP = projection; // Get the new projection view

		// Go back to starting positions
		model_view = animation_start_model_view;
		projection = animation_start_projection;
		ctm = animation_start_ctm;

		current_step = 0;
		currentState = PLAYER_VIEW;
		is_animating = 1;
    } else if(key == 'w' && lockTrackBallMovement == 1){ // WALK FORWARDS
		animation_start_model_view = model_view;
    
		// Calculate next position
		float moveX = playerX + cos(playerAngle);
		float moveZ = playerZ + sin(playerAngle);
		
		if(isValidPosition(moveX, moveZ)) {
			playerX = moveX;
			playerZ = moveZ;
			updateLookPosition();  // This will set both look position and view matrix
			turnTargetMV = model_view;
			model_view = animation_start_model_view;
			
			current_step = 0;
			currentState = WALK_FORWARD;
			is_animating = 1;
		}
	} else if(key == 's' && lockTrackBallMovement == 1){ // WALK BACKWARDS
		animation_start_model_view = model_view;
    
		// calculate next position
		float moveX = playerX - cos(playerAngle);
		float moveZ = playerZ - sin(playerAngle);
		
		if(isValidPosition(moveX, moveZ)) {
			playerX = moveX;
			playerZ = moveZ;
			updateLookPosition();  // This will set both look position and view matrix
			turnTargetMV = model_view;
			model_view = animation_start_model_view;
			
			current_step = 0;
			currentState = WALK_BACKWARD;
			is_animating = 1;
		}
	} else if(key == 'd' && lockTrackBallMovement == 1){
		animation_start_model_view = model_view;
		
		// Calculates the perpendicular movemnet
		float moveX = playerX - sin(playerAngle);
		float moveZ = playerZ + cos(playerAngle);
		
		if(isValidPosition(moveX, moveZ)) {
			playerX = moveX;
			playerZ = moveZ;
			updateLookPosition();  // This will set both look position and view matrix
			turnTargetMV = model_view;
			model_view = animation_start_model_view;
			
			current_step = 0;
			currentState = STRAFE_RIGHT;
			is_animating = 1;
		}
	} else if(key == 'a' && lockTrackBallMovement == 1){
		animation_start_model_view = model_view;
    
		// Calculates the perpendicular movemnet
		float moveX = playerX + sin(playerAngle);
		float moveZ = playerZ - cos(playerAngle);
		
		if(isValidPosition(moveX, moveZ)) {
			playerX = moveX;
			playerZ = moveZ;
			updateLookPosition();  // This will set both look position and view matrix
			turnTargetMV = model_view;
			model_view = animation_start_model_view;
			
			current_step = 0;
			currentState = STRAFE_LEFT;
			is_animating = 1;
		}
	} else if(key == ',' && lockTrackBallMovement == 1){ // Turn left
		animation_start_model_view = model_view;
    
		// Calculate new angle
		targetAngle = playerAngle - M_PI/2;
		
		float targetLookX = playerX + LOOK_DISTANCE * cos(targetAngle);
		float targetLookZ = playerZ + LOOK_DISTANCE * sin(targetAngle);

		// Store current angle for animation
		look_at(playerX, EYE_HEIGHT, playerZ,
				targetLookX, EYE_HEIGHT, targetLookZ,
				0, 1 , 0);
		
		// Set target position and matrix
		turnTargetMV = model_view;
		model_view = animation_start_model_view;
		
		current_step = 0;
		currentState = TURN_LEFT;
		is_animating = 1;
	} else if(key == '.' && lockTrackBallMovement == 1){ // Turn right

		animation_start_model_view = model_view;
    
		// Calculate new angle
		targetAngle = playerAngle + M_PI/2;
		
		// Store the exact target look position based on 90-degree turn
		float targetLookX = playerX + LOOK_DISTANCE * cos(targetAngle);
		float targetLookZ = playerZ + LOOK_DISTANCE * sin(targetAngle);


		// Calculate exact target view matrix
		look_at(playerX, EYE_HEIGHT, playerZ,
				targetLookX, EYE_HEIGHT, targetLookZ,
				0, 1, 0);
		
		// Set target position and matrix
		turnTargetMV = model_view;
		model_view = animation_start_model_view;
		
		current_step = 0;
		currentState = TURN_RIGHT;
		is_animating = 1;
	} else if(key == 'g' && lockTrackBallMovement == 1) {
		animation_start_model_view = model_view;
		animation_start_projection = projection;
		animation_start_ctm = ctm;
		pathIndex = 0;
		current_step = 0;
		solveState = SOLVE_TURN;
		currentState = AUTO_SOLVE;
		is_animating = 1;
	} else if (key == 'h') {	// RESET LIGHTS
		light_skip = 0;
		glUniform1i(light_skip_location, light_skip);
		glutPostRedisplay();
	} else if (key == 'j') {	// SKIP AMBIENCE
		light_skip = 1;
		glUniform1i(light_skip_location, light_skip);
		glutPostRedisplay();
	} else if (key == 'k') {	// SKIP SPECULAR
		light_skip = 2;
		glUniform1i(light_skip_location, light_skip);
		glutPostRedisplay();
	} else if (key == 'l') {	// SKIP DIFFUSION
		light_skip = 3;
		glUniform1i(light_skip_location, light_skip);
		glutPostRedisplay();
	}
    // glutPostRedisplay();
}

mat4 calculateNewCameraAngle(mat4 start, mat4 end, float progress) {
    mat4 result;
    // For each element in the 4x4 matrix
	// calc x vector (first row)
    result.x.x = start.x.x * (1.0 - progress) + end.x.x * progress;
    result.x.y = start.x.y * (1.0 - progress) + end.x.y * progress;
    result.x.z = start.x.z * (1.0 - progress) + end.x.z * progress;
    result.x.w = start.x.w * (1.0 - progress) + end.x.w * progress;
    
    // calc y vector (second row)
    result.y.x = start.y.x * (1.0 - progress) + end.y.x * progress;
    result.y.y = start.y.y * (1.0 - progress) + end.y.y * progress;
    result.y.z = start.y.z * (1.0 - progress) + end.y.z * progress;
    result.y.w = start.y.w * (1.0 - progress) + end.y.w * progress;
    
    // calc z vector (third row)
    result.z.x = start.z.x * (1.0 - progress) + end.z.x * progress;
    result.z.y = start.z.y * (1.0 - progress) + end.z.y * progress;
    result.z.z = start.z.z * (1.0 - progress) + end.z.z * progress;
    result.z.w = start.z.w * (1.0 - progress) + end.z.w * progress;
    
    // calc w vector (fourth row)
    result.w.x = start.w.x * (1.0 - progress) + end.w.x * progress;
    result.w.y = start.w.y * (1.0 - progress) + end.w.y * progress;
    result.w.z = start.w.z * (1.0 - progress) + end.w.z * progress;
    result.w.w = start.w.w * (1.0 - progress) + end.w.w * progress;
    
    return result;
}

void idle(void){
	if(is_animating == 1) {
        if(currentState == NONE) {
            is_animating = 0;
			current_step = 0;
        }
        else if(currentState == RESET_VIEW) {
            if(current_step == ANIMATION_STEPS) {
                // Animation complete
                ctm = initial_ctm;
                model_view = initial_model_view;
                projection = initial_projection;
                currentState = NONE;
                is_animating = 0;
				current_step = 0;
            } else {
                // Calculate step progress
                float progress = (float)current_step / ANIMATION_STEPS;
                
                // calc all matrices
                ctm = calculateNewCameraAngle(animation_start_ctm, initial_ctm, progress);
                model_view = calculateNewCameraAngle(animation_start_model_view, initial_model_view, progress);
                projection = calculateNewCameraAngle(animation_start_projection, initial_projection, progress);
                current_step++;
            }
        }
		else if(currentState == PLAYER_VIEW){
			if(current_step == ANIMATION_STEPS) {
				//Animation is done
				ctm = entranceCTM;
				model_view = entranceMV;
				projection = entranceP;
				currentState = NONE;
				is_animating = 0;
				current_step = 0;
			}else{
				float progress = (float)current_step/ANIMATION_STEPS;
				// calc all matrices
                ctm = calculateNewCameraAngle(animation_start_ctm, entranceCTM, progress);
                model_view = calculateNewCameraAngle(animation_start_model_view, entranceMV, progress);
                projection = calculateNewCameraAngle(animation_start_projection, entranceP, progress);
                current_step++;
			}
		}
		else if(currentState == WALK_FORWARD || currentState == WALK_BACKWARD || 
                currentState == STRAFE_LEFT || currentState == STRAFE_RIGHT) {
            if(current_step == ANIMATION_STEPS) {
				// Use stored target matrix directly
				model_view = turnTargetMV;
				currentState = NONE;
				is_animating = 0;
				current_step = 0;
			} else {
				float progress = (float)current_step / ANIMATION_STEPS;
				// Animate to target matrix
				model_view = calculateNewCameraAngle(animation_start_model_view, turnTargetMV, progress);
				current_step++;
			}
        }
		else if(currentState == TURN_LEFT || currentState == TURN_RIGHT) {
			if(current_step == 100) {
				// Set exact final angle
				playerAngle = targetAngle;
				
				// Normalize angle to keep it in valid range
				while (playerAngle < -M_PI) playerAngle += 2 * M_PI;
				while (playerAngle > M_PI) playerAngle -= 2 * M_PI;
				
				// Set exact final position with normalized angle
				lookX = playerX + LOOK_DISTANCE * cos(playerAngle);
				lookZ = playerZ + LOOK_DISTANCE * sin(playerAngle);
				
				// Set exact final view matrix
				look_at(playerX, EYE_HEIGHT, playerZ,
						lookX, EYE_HEIGHT, lookZ,
						0, 1, 0);
				
				currentState = NONE;
				is_animating = 0;
				current_step = 0;
			} else {
				float progress = (float)current_step / 100;
				model_view = calculateNewCameraAngle(animation_start_model_view, turnTargetMV, progress);
				current_step++;
			}

        }else if(currentState == AUTO_SOLVE) {
			// Check if we're done with the path
			if(fullSolution[pathIndex] == '\0') {
				currentState = NONE;
				is_animating = 0;
				return;
			}
			
			// Get current instruction
			char currentMove = fullSolution[pathIndex];
			
			if(solveState == SOLVE_TURN) {
				// If straight movement, skip turn phase
				if(currentMove == 'S') {
					solveState = SOLVE_MOVE;
					return;
				}
				
				// Handle turn animation
				if(current_step == 0) {
					// Set up turn
					float newAngle = (currentMove == 'L') ? playerAngle - M_PI/2 : playerAngle + M_PI/2;
					float targetLookX = playerX + cos(newAngle);
					float targetLookZ = playerZ + sin(newAngle);
					
					look_at(playerX, EYE_HEIGHT, playerZ,
						targetLookX, EYE_HEIGHT, targetLookZ,
						0, 1, 0);
					turnTargetMV = model_view;
					model_view = animation_start_model_view;
				}
				
				// Complete turn
				if(current_step == 30) {
					if(currentMove == 'L') {
						playerAngle -= M_PI/2;
					} else {
						playerAngle += M_PI/2;
					}
					lookX = playerX + cos(playerAngle);
					lookZ = playerZ + sin(playerAngle);
					
					// Prepare for forward movement
					animation_start_model_view = model_view;
					current_step = 0;
					solveState = SOLVE_MOVE;
				} else {
					float progress = (float)current_step / 30;
					model_view = calculateNewCameraAngle(animation_start_model_view, turnTargetMV, progress);
					current_step++;
				}
			}

			else if(solveState == SOLVE_MOVE) {
				// Handle forward movement
				if(current_step == 0) {
					float moveX = playerX + cos(playerAngle);
					float moveZ = playerZ + sin(playerAngle);
					
					if(isValidPosition(moveX, moveZ)) {
						playerX = moveX;
						playerZ = moveZ;
						lookX = playerX + cos(playerAngle);
						lookZ = playerZ + sin(playerAngle);
						
						look_at(playerX, EYE_HEIGHT, playerZ,
							lookX, EYE_HEIGHT, lookZ,
							0, 1, 0);
					}
				}
				
				if(current_step == ANIMATION_STEPS) {
					// Movement complete
					look_at(playerX, EYE_HEIGHT, playerZ,
						lookX, EYE_HEIGHT, lookZ,
						0, 1, 0);
						
					// Reset for next instruction
					animation_start_model_view = model_view;
					current_step = 0;
					solveState = SOLVE_TURN;
					pathIndex++;
				} else {
					float progress = (float)current_step / ANIMATION_STEPS;
					model_view = calculateNewCameraAngle(animation_start_model_view, model_view, progress);
					current_step++;
				}
			}
		}
		
    }
	glutPostRedisplay();
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

	initializePlayer();
	look_at(0, 0, (2 * mazeSizeX) + 1, 0, 0, 0, 0, 1, 0);
	frustum(-5, 5, -5, 5, -5, 25);
	mat4 scale_matrix = scale(0.6, 0.6, 0.6);
    ctm = matMult(scale_matrix, ctm);

	initial_model_view = model_view;
	initial_projection = projection;

    glutKeyboardFunc(keyboard); // Checks for any keyboard inputs
    glutMouseFunc(mouse); // Checks for the mouse
    glutMotionFunc(motion);
	glutIdleFunc(idle);
    glutMainLoop(); // Makes the whole main function wait so the program does not terminate

    return 0;
}
