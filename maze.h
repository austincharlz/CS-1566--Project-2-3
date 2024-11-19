#ifndef _MAZE_H_

#define _MAZE_H_

int xLength, yLength;					// specified maze dimensions
int totalMazeWidth, totalMazeHeight;	// ACTUAL TOTAL DIMENSIONS including walls and corners
extern int **maze;						// believe it or not, the maze.

int randInRange(int n1, int n2);
void initializeMaze();
void generateMaze(int xLeft, int xRight, int yUp, int yDown);
void renderMaze();
void printMaze();
int** getMaze(int x, int y);

#endif