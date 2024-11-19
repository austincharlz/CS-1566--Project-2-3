#include "maze.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int xLength, yLength;					// specified maze dimensions
int totalMazeWidth, totalMazeHeight;	// ACTUAL TOTAL DIMENSIONS including walls and corners
int **maze;								// believe it or not, the maze.
int **solution;

void initializeMaze() {
	// seeding the random number generator
	// yes this is a stupid seed but i felt sus about time(NULL) bc it didnt seem to be actually random sooooo cope <3
	srand(time(NULL) * time(NULL));
    // calculate actual maze dimensions including walls and corners from specified xLength and yLength
    totalMazeWidth = xLength * 2 + 1;
    totalMazeHeight = yLength * 2 + 1;
    
    // malloc()ate entire maze
    maze = (int **)malloc(totalMazeHeight * sizeof(int *));
    for (int i = 0; i < totalMazeHeight; i++) {
        maze[i] = (int *)malloc(totalMazeWidth * sizeof(int));
    }

    // initialize all cells to 0 first
    for (int y = 0; y < totalMazeHeight; y++) {
        for (int x = 0; x < totalMazeWidth; x++) {
            maze[y][x] = 0;
        }
    }

    // set outer walls (top, bottom, left, right) to 2
    // top and bottom walls
    for (int x = 0; x < totalMazeWidth; x++) {
        maze[0][x] = 2;           					// top wall
        maze[totalMazeHeight-1][x] = 2; 			// bottom wall
    }
    // left and right walls
    for (int y = 0; y < totalMazeHeight; y++) {
        maze[y][0] = 2;          					// left wall
        maze[y][totalMazeWidth-1] = 2; 				// right wall
	}
	// set all corner points to 1
    for (int y = 0; y < totalMazeHeight; y += 2) {
        for (int x = 0; x < totalMazeWidth; x += 2) {
            maze[y][x] = 1;
        }
    }

    // set entrance and exit
    maze[1][0] = 0;  								// entrance
    maze[totalMazeHeight-2][totalMazeWidth-1] = 0;  // exit
}

void formatMaze() {
    // set outer walls (top, bottom, left, right) to 2
    // top and bottom walls
    for (int x = 0; x < totalMazeWidth; x++) {
        maze[0][x] = 2;           					// top wall
        maze[totalMazeHeight-1][x] = 2; 			// bottom wall
    }
    // left and right walls
    for (int y = 0; y < totalMazeHeight; y++) {
        maze[y][0] = 2;          					// left wall
        maze[y][totalMazeWidth-1] = 2; 				// right wall
    }

	// set all corner points to 1
    for (int y = 0; y < totalMazeHeight; y += 2) {
        for (int x = 0; x < totalMazeWidth; x += 2) {
            maze[y][x] = 1;
        }
    }

    // set entrance and exit
    maze[1][0] = 0;  								// entrance
    maze[totalMazeHeight-2][totalMazeWidth-1] = 0;  // exit
}

int randInRange(int n1, int n2) {
	// return n1 if the range is 0
	if (n1 == n2) {
		return n1;
	}

    // swap if necessary
    if (n1 > n2) {
        int temp = n1;
        n1 = n2;
        n2 = temp;
    }
    return n1 + rand() % (n2 - n1 + 1);
}

void printMaze() {
	// printf("\nY: X:0 1 2 3 4 5 6 7 8 9 1011122314151617181920\n");
    for (int y = 0; y < totalMazeHeight; y++) {
		// printf("%.2d   ", y);
        for (int x = 0; x < totalMazeWidth; x++) {
            switch(maze[y][x]) {
                case 0:					// BLANK CELL
                    printf("  "); 
                    break;
                case 1:					// CORNER CELL
                    printf("██"); 
                    break;
                case 2:					// WALL CELL
                    printf("██"); 
                    break;
            }
        }
        printf("\n");
    }
}

void generateMaze(int xLeft, int xRight, int yUp, int yDown) {
	// defining base cases - idek if these are correct but it works so idc
	if (xRight - xLeft <= 1 || yDown - yUp <= 1) {
		return;
	}
	if (xRight <= xLeft || yDown <= yUp) {
		return;
	}
	
	// gen random point within provided bounds
	int randX = xLeft;
	int randY = xRight;

	int i = 0;
	while (randX <= (xLeft * 2) || randX >= (xRight * 2) || randX % 2 != 0 ) {
		randX = randInRange((xLeft * 2), (xRight * 2));
	}
	i = 0;
	while (randY <= (yUp * 2) || randY >= (yDown * 2) || randY % 2 != 0) {
		randY = randInRange((yUp * 2), (yDown * 2));
	}
	
	// picking which section to delete for each wall and which wall to skip deletion for
	int nDel = randY, sDel = randY, eDel = randX, wDel = randX;
	int skipDel = randInRange(0,3);
	char dir[4] = {'N', 'S', 'E', 'W'};

	// calculating nDel, sDel, eDel, and wDel (these are the indices to delete for each of the n/s/e/w walls)
	while (nDel <= (yUp * 2) || nDel >= randY || nDel % 2 == 0) {
		nDel = randInRange((yUp * 2), randY);
	}
	while (sDel <= randY || sDel >= (yDown * 2) || sDel % 2 == 0) {
		sDel = randInRange(randY, (yDown * 2));
	}
	while (eDel <= randX || eDel >= (xRight * 2) || eDel % 2 == 0) {
		eDel = randInRange(randX, (xRight * 2));
	}
	while (wDel <= (xLeft * 2) || wDel >= randX || wDel % 2 == 0) {
		wDel = randInRange((xLeft * 2), randX);
	}

	// drawing horizontal walls, ignoring the skipped direction as needed
	for (int x = xLeft * 2 + 1; x < 2 * xRight + 1; x++) {
		if (dir[skipDel] == 'E') {
			if (x != wDel) { maze[randY][x] = 2; }	// ignoring eDel
		} else if (dir[skipDel] == 'W') {
			if (x != eDel) { maze[randY][x] = 2; }	// ignoring wDel
		} else {
			if (x != eDel && x != wDel) { maze[randY][x] = 2; }
		}
	}

	// drawing vertical walls, ignoring the skipped direction as needed
	for (int y = yUp * 2 + 1; y < 2 * yDown + 1; y++) {
		if (dir[skipDel] == 'N') {
			if (y != nDel) { maze[y][randX] = 2; }	// ignoring sDel
		} else if (dir[skipDel] == 'S') {
			if (y != sDel) { maze[y][randX] = 2; }	// ignoring nDel
		} else {
			if (y != sDel && y != nDel) { maze[y][randX] = 2; }
		}
	}

	generateMaze(xLeft, (randX / 2), yUp, (randY / 2));		// top left quadrant
	generateMaze((randX / 2), xRight, yUp, (randY / 2));	// top right quadrant
	generateMaze(xLeft, (randX / 2), (randY / 2), yDown);	// bottom left quadrant
	generateMaze((randX / 2), xRight, (randY / 2), yDown);	// bottom right quadrant
}

int** getMaze(int x, int y) {
	// initialize xLength and yLength
	xLength = x;
	yLength = y;

	// init the maze walls and pillars etc
	initializeMaze();

	// actually generate the maze's internals
    generateMaze(0, xLength, 0, yLength);

	// redo maze walls and corners to get correct format for pillars, walls, etc
	formatMaze();

	solveMaze(1);

	return maze;
}

// -------------------------- MAZE SOLVER ----------------------------------

void solveMazeUtil(int x, int y, int currentDir, int **visited, int directions[4][2]) {
    // Check if out of bounds or at the exit
    if (x < 0 || x >= totalMazeWidth || y < 0 || y >= totalMazeHeight || visited[y][x]) {
        return;
    }

    visited[y][x] = 1;
    solution[y][x] = 0;

    // If at the exit, stop recursion
    if (x == totalMazeWidth - 1 && y == totalMazeHeight - 2) {
        return;
    }

    // Try directions in order: left, straight, right
    for (int try = 0; try < 4; try++) {
        int testDir = (currentDir - 1 + try + 4) % 4;
        int newX = x + directions[testDir][1];
        int newY = y + directions[testDir][0];

        // Check if new cell is open and not visited
        if (newX >= 0 && newX < totalMazeWidth && newY >= 0 && newY < totalMazeHeight && maze[newY][newX] == 0 && !visited[newY][newX]) {
            solveMazeUtil(newX, newY, testDir, visited, directions);

            // If we found a solution (end point reached), no need to backtrack
            if (solution[totalMazeHeight - 2][totalMazeWidth - 2] == 0) {
                return;
            }
        }
    }

    // Mark as failed path if no valid move found
    solution[y][x] = 2;
}

// Input 1 to have it default from start of maze
// Input any other number to start at a different point
void solveMaze(int normal) {
    // Create solution maze, same dimensions as original
    solution = (int **)malloc(totalMazeHeight * sizeof(int *));
    for (int i = 0; i < totalMazeHeight; i++) {
        solution[i] = (int *)calloc(totalMazeWidth, sizeof(int));
    }

    // Initialize the solution with 1s (wall)
    for (int y = 0; y < totalMazeHeight; y++) {
        for (int x = 0; x < totalMazeWidth; x++) {
            solution[y][x] = 1;
        }
    }

    // Track visited cells
    int **visited = (int **)malloc(totalMazeHeight * sizeof(int *));
    for (int i = 0; i < totalMazeHeight; i++) {
        visited[i] = (int *)calloc(totalMazeWidth, sizeof(int));
    }

    // Directions: right, down, left, up
    int directions[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};


    // Start solving from the entrance
    if(normal == 1){
        // x, y, curDirection, visited int array, direction array
        solveMazeUtil(0, 1, 0, visited, directions);
    }
    else{
        int yInput, xInput;
        printf("Enter the row (y): \n");
        scanf("%d", &yInput);
        printf("Now enter the col (x): \n");
        scanf("%d", &xInput);
        solveMazeUtil(xInput, yInput, 0, visited, directions);
    }

    // x, y, curDirection
    int yInput, xInput;
    
    // Free visited array
    for (int i = 0; i < totalMazeHeight; i++) {
        free(visited[i]);
    }
    free(visited);
}



void printSolution() {
	// printf("\nY: X:0 1 2 3 4 5 6 7 8 9 1011122314151617181920\n");
    for (int y = 0; y < totalMazeHeight; y++) {
		// printf("%.2d   ", y);
        for (int x = 0; x < totalMazeWidth; x++) {
            switch(solution[y][x]) {
                case 0:					// BLANK CELL
                    printf("  "); 
                    break;
                case 1:					// CORNER CELL
                    printf("██"); 
                    break;
                case 2:					// WALL CELL
                    printf("██"); 
                    break;
            }
        }
        printf("\n");
    }
}