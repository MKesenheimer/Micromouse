/**
 * Maze manipulation functions
 */

#include <stdio.h>
#include "../inc/maze.h"
#include "../inc/lookup.h"

PRIVATE U8 tMazeHoriz[MAZE_SIZE / 2];
PRIVATE U8 tMazeVert[MAZE_SIZE / 2];

/**
 * Maze is organized as
 *
 * 15,0
 * ...
 * 2,0
 * 1,0   1,1   1,2   1,3
 * 0,0   0,1   0,2   0,3
 */

/**
 * Computes the index and bit position of a particular wall
 */
PRIVATE void _ComputeIndex(U8 row, U8 col, DIRECTION dir, U8 *index, U8 *bit)
{
	if(index == NULL || bit == NULL)
		return;

	switch(dir)
	{
		case NORTH:
			*index = (row << 1) + (col >> 3);
			*bit = col % 8;
			break;
		case SOUTH:
			*index = ((row - 1) << 1) + (col >> 3);
			*bit = col % 8;
			break;
		case EAST:
			*index = (col << 1) + (row >> 3);
			*bit = row % 8;
			break;
		case WEST:
			*index = ((col - 1) << 1) + (row >> 3);
			*bit = row % 8;			
			break;
	}
}

/**
 * Initializes the maze
 */
void InitMaze()
{
	U8 i;

	for(i = 0; i < MAZE_SIZE / 2; i++)
		tMazeHoriz[i] = 0;

	for(i = 0; i < MAZE_SIZE / 2; i++)
		tMazeVert[i] = 0;
}

/**
 * Sets the wall at a particular location
 */
void SetWall(U8 row, U8 col, DIRECTION dir)
{
	U8 index, bit;

	if(IsBorder(row, col, dir))
		return;

	_ComputeIndex(row, col, dir, &index, &bit);

	if(dir == NORTH || dir == SOUTH)
		SET_BIT(tMazeHoriz[index], tMask[bit]);
	else
		SET_BIT(tMazeVert[index], tMask[bit]);
}

/**
 * Clears the wall at a particular location
 */
void ClearWall(U8 row, U8 col, DIRECTION dir)
{
	U8 index, bit;

	if(IsBorder(row, col, dir))
		return;

	_ComputeIndex(row, col, dir, &index, &bit);

	if(dir == NORTH || dir == SOUTH)
		CLEAR_BIT(tMazeHoriz[index], tMask[bit]);
	else
		CLEAR_BIT(tMazeVert[index], tMask[bit]);
}

/**
 * Checks if the wall is the border wall
 */
BOOL IsBorder(U8 row, U8 col, DIRECTION dir)
{
	switch(dir)
	{
		case NORTH:
			if(row >= (MAZE_HEIGHT - 1))
				return TRUE;
			break;
		case SOUTH:
			if(row == 0)
				return TRUE;
			break;
		case EAST:
			if(col >= (MAZE_WIDTH - 1))
				return TRUE;
			break;
		case WEST:
			if(col == 0)
				return TRUE;
			break;
	}

	return FALSE;
}

/**
 * Checks if the wall at a particular location is set
 */
BOOL IsWallSet(U8 row, U8 col, DIRECTION dir)
{
	U8 index, bit;

	if(IsBorder(row, col, dir))
		return TRUE;

	_ComputeIndex(row, col, dir, &index, &bit);

	if(dir == NORTH || dir == SOUTH)
		return IS_SET(tMazeHoriz[index], tMask[bit]);

	return IS_SET(tMazeVert[index], tMask[bit]);
}

/**
 * Prints the maze
 */
void PrintMaze()
{
	U8 i, j;

	for(i = 0; i < MAZE_HEIGHT; i++)
	{
		U8 x, y;

		y = (MAZE_HEIGHT - 1) - i;

		for(j = 0; j < MAZE_WIDTH; j++)
		{
			x = j;

			printf("+");

			if(IsWallSet(y, x, NORTH))
				printf("-");
			else
				printf(" ");
		}

		printf("+");
		printf("\n");

		for(j = 0; j < MAZE_WIDTH; j++)
		{
			x = j;

			if(IsWallSet(y, x, WEST))
				printf("|");
			else
				printf(" ");

			printf(" ");
		}

		printf("|");
		printf("\n");
	}

	for(i = 0; i < MAZE_WIDTH; i++)
	{
		printf("+");
		printf("-");
	}

	printf("+");
	printf("\n");
}
