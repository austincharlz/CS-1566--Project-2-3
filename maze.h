#ifndef _MAZE_H_

#define _MAZE_H_

typedef struct {
    int northWall;
    int eastWall;
    int southWall;
    int westWall;
} Cell;

typedef struct{
    int rows;
    int columns;
    Cell **cells;
} Maze;

Maze* createMaze(int rows, int columns);
void genWalls();
void breakWalls();
void printMaze(Maze m);

