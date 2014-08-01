/**
 * Floodfill implementation
 */
#include <assert.h>
#include "../inc/common.h"
#include "../inc/floodfill.h"

/**
 *
 */
PRIVATE void ffInitValues(floodfill_t* ffvalues)
{
	U16 i;
	for(i = 0; i < FF_SIZE; i++)
		ffvalues->data[i] = INVALID;
}

U8 ffGetPos(S8 y, S8 x) {
//	assert((y >= 0) && (y < MAZE_HEIGHT) && (x >= 0) && (x < MAZE_WIDTH));
	return x*MAZE_WIDTH+y;
}

void ffGetCoords(U8 position, S8* y, S8* x) {
	*x = position / MAZE_WIDTH;
	*y = position % MAZE_HEIGHT;
}

void floodFill(const bitmap_t* maze, floodfill_t* ffvalues, set_t* setS)
{
	set_t setSSaved = *setS; // save set S to be restored later
	set_t setT;
	set_t* Sptr = setS; // pointer to current set S
	set_t* Tptr = &setT; // pointer to current set T
	U8 value = 0;

	setClearSet(Tptr);
	ffInitValues(ffvalues);

	while (!setIsEmpty(Sptr))
	{
		set_t* tmp = Sptr;

		while(!setIsEmpty(Sptr))
		{

			U8 pos = setGetFirstAndClear(Sptr); // get a cell pos in S
			S8 x,y;

			ffGetCoords(pos, &y, &x); // retrieve coord of cell pos

			if (ffvalues->data[pos] == INVALID) { // if cell doesn't have value

				ffvalues->data[pos] = value; // cell is assigned to current value

				// check each neighbor by checking for a wall and see if a value exist

				if (getWall(maze, y, x, NORTH) == 0 // if north neighbor reachable
						&& ffvalues->data[ffGetPos(y+1,x)] == INVALID) // and has no value
					setSetBit(Tptr, ffGetPos(y+1,x)); // put it in set T
				if (getWall(maze, y, x, EAST) == 0
						&& ffvalues->data[ffGetPos(y,x+1)] == INVALID)
					setSetBit(Tptr, ffGetPos(y,x+1));
				if (getWall(maze, y, x, SOUTH) == 0
						&& ffvalues->data[ffGetPos(y-1,x)] == INVALID)
					setSetBit(Tptr, ffGetPos(y-1,x));
				if (getWall(maze, y, x, WEST) == 0
						&& ffvalues->data[ffGetPos(y,x-1)] == INVALID)
					setSetBit(Tptr, ffGetPos(y,x-1));

			}
		}
		// swap the set pointers, set S should be empty before swap
		Sptr = Tptr;
		Tptr = tmp;
		value++; // increment value for neighbor cells
	}
	*setS = setSSaved;	// restore set S
}

void exportFF(FILE* fs, const bitmap_t* maze, const floodfill_t* ffvalues, const set_t* visits, const U8 curPos) {
#ifndef __MM_COMP_AVR__
	U8 x = 0, y = 0, yy = 0;
	FILE* file_desc = stdout;

	assert(maze && ffvalues);

	if (fs)
		file_desc = fs;

	fprintf(file_desc,"+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+\n");
	for (yy = 0; yy < 16; yy++) {
		y = 15 - yy;
		fprintf(file_desc,"|");
		for (x = 0; x < 16; x++) {

			if (visits->data[ffGetPos(y,x) / 8] & (1 << (ffGetPos(y,x) % 8))) {
				if (ffGetPos(y,x) == curPos)
					fprintf(file_desc, "\e[34m\e[1m%3d\e[0m", ffvalues->data[ffGetPos(y,x)]);
				else
					fprintf(file_desc, "\e[32m%3d\e[0m", ffvalues->data[ffGetPos(y,x)]);
			} else
				fprintf(file_desc, "\e[31m%3d\e[0m", ffvalues->data[ffGetPos(y,x)]);

			if (getWall(maze, y, x, EAST))
				fprintf(file_desc,"|");
			else
				fprintf(file_desc," ");
		}
		fprintf(file_desc,"\n+");
		for (x = 0; x < 16; x++) {
			if (getWall(maze, y, x, SOUTH))
				fprintf(file_desc,"---");
			else
				fprintf(file_desc,"   ");
			fprintf(file_desc,"+");
		}
		fprintf(file_desc,"\n");
	}
	fprintf(file_desc,"\n");

	if (file_desc && (file_desc != stdout))
		fclose(file_desc);
#endif
}
