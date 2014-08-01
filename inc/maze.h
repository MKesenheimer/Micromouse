/**
 * Interface functions for manipulating the maze
 */

#ifndef _MAZE_H_
#define _MAZE_H_

#include "common.h"

#define MAZE_HEIGHT				16
#define MAZE_WIDTH				16
#define MAZE_SIZE				60

/**
 * Enumeration for directions
 */
typedef enum
{
	NORTH = 0,
	SOUTH,
	EAST,
	WEST
} DIRECTION;

/**
 * Public interface functions
 */
void InitMaze();
void SetWall(U8, U8, DIRECTION);
void ClearWall(U8, U8, DIRECTION);
BOOL IsBorder(U8, U8, DIRECTION);
BOOL IsWallSet(U8, U8, DIRECTION);
void PrintMaze();

#endif
