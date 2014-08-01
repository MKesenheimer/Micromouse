/**
 * Floodfill algorithm
 */

#ifndef _FLOODFILL_H_
#define _FLOODFILL_H_

#include "common.h"
#include "set.h"
#include "maze2.h"

/**
 * (x,y) cartesian coord
 * y = row, x = col
 * |......|.......|..
 * |2(0,2)|18(1,2)|..
 * |1(0,1)|17(1,1)|..
 * |0(0,0)|16(1,0)|..
 */

#define INVALID		255
#define FF_SIZE		256

typedef struct {
	U8 data[FF_SIZE];
} floodfill_t;

U8 ffGetPos(S8 y, S8 x);	// get cell # from on x,y coords
void ffGetCoords(U8 position, S8* y, S8* x);	// get x,y coords from cell #
void floodFill(const bitmap_t* maze, floodfill_t* ffvalues, set_t* setS);
void exportFF(FILE* fs, const bitmap_t* maze, const floodfill_t* ffvalues, const set_t* visits, const U8 curPos);

#endif
