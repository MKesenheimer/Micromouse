#include <string.h>
#include <avr/sleep.h>
#include <avr/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../inc/common.h"
#include "../inc/exploration.h"
#include "../inc/sensor.h"
#include "../inc/motion.h"
#include "../inc/maze2.h"
#include "../inc/set.h"
#include "../inc/floodfill.h"
#include "../inc/eeprom.h"

extern DiffSteer_t DS;
extern Sensor_t sensors[3];

void findMinPath(U8 *n, U8 *e, U8 *s, U8 *w, U8 x, U8 y, set_t* visits);

/* return whether a cell is visited or not */
BOOL isVisited(set_t* visits, U8 y, U8 x)
{
	return visits->data[ffGetPos(y, x)/8] & (1 << (ffGetPos(y, x) % 8));
}

/* return the left and right direction based on current direction */
void getLeftRightDir(direction_t facing, direction_t* left, direction_t* right)
{
	switch(facing)
	{
	case NORTH:
		*left = WEST; *right = EAST; break;
	case EAST:
		*left = NORTH; *right = SOUTH; break;
	case SOUTH:
		*left = EAST; *right = WEST; break;
	case WEST:
		*left = SOUTH; *right = NORTH; break;
	}
}

/* detect and set walls on both side and in front */
void detectWalls(direction_t facing, bitmap_t* exmap, U8 y, U8 x)
{
	direction_t left, right;
	U8 ay = y, ax = x;	// y and x coordinates of cell in front

	getLeftRightDir(facing, &left, &right);

	switch(facing)	// get coordinate of next cell
	{
	case NORTH: ay++; break;
	case EAST: ax++; break;
	case SOUTH: ay--; break;
	case WEST: ax--; break;
	}

	if (!getWall(exmap, y, x, facing))
	{
		if (averageADC(&sensors[SENSOR_LEFT]) > SENSOR_WALLDETECT_LEFT) // if there is a wall
		{
			setWall(exmap, ay, ax, right, 1);
			if (averageADC(&sensors[SENSOR_LEFT]) > 125)
				DSTurn(&DS, 20, DS_LEFT);
		}
		else
			setWall(exmap, ay, ax, right, 0);

		if (averageADC(&sensors[SENSOR_CENTER]) > SENSOR_WALLDETECT_CENTER)
			setWall(exmap, ay, ax, facing, 1);
		else
			setWall(exmap, ay, ax, facing, 0);

		if (averageADC(&sensors[SENSOR_RIGHT]) > SENSOR_WALLDETECT_RIGHT)
		{
			setWall(exmap, ay, ax, left, 1);
			if (averageADC(&sensors[SENSOR_RIGHT]) > 125)
				DSTurn(&DS, 20, DS_RIGHT);
		}
		else
			setWall(exmap, ay, ax, left, 0);
	}

	while (!DS.A.done);	// wait for correcting turn to finish

}

void makeTurn(U8* n, U8* e, U8* s, U8* w, direction_t* facing)
{
	if (*n) {
		/* set n = 0 if determined we need to turn, so coordinate won't
		 * change while we're turning. we will move into next cell when
		 * the loop runs again */
		*n = 0;
		if (*facing == EAST)
			DSTurn(&DS, DS_TURN90, DS_LEFT);
		else if (*facing == WEST)
			DSTurn(&DS, DS_TURN90, DS_RIGHT);
		else if (*facing == SOUTH)
			DSTurn(&DS, DS_TURN180, DS_LEFT);
		else
			*n = 1;	// no turning required, move into the next cell
		*facing = NORTH;
	}
	else if (*e) {
		*e = 0;
		if (*facing == SOUTH)
			DSTurn(&DS, DS_TURN90, DS_LEFT);
		else if (*facing == NORTH)
			DSTurn(&DS, DS_TURN90, DS_RIGHT);
		else if (*facing == WEST)
			DSTurn(&DS, DS_TURN180, DS_LEFT);
		else
			*e = 1;
		*facing = EAST;
	}
	else if (*s)	{
		*s = 0;
		if (*facing == WEST)
			DSTurn(&DS, DS_TURN90, DS_LEFT);
		else if (*facing == EAST)
			DSTurn(&DS, DS_TURN90, DS_RIGHT);
		else if (*facing == NORTH)
			DSTurn(&DS, DS_TURN180, DS_LEFT);
		else
			*s = 1;
		*facing = SOUTH;
	}
	else if (w) {
		*w = 0;
		if (*facing == NORTH)
			DSTurn(&DS, DS_TURN90, DS_LEFT);
		else if (*facing == SOUTH)
			DSTurn(&DS, DS_TURN90, DS_RIGHT);
		else if (*facing == EAST)
			DSTurn(&DS, DS_TURN180, DS_LEFT);
		else
			*w = 1;
		*facing = WEST;
	}

	while (!DS.A.done) sleep_mode();	// wait until turning done
}

void optimizePath(direction_t facing, U8* n, U8* e, U8* s, U8* w, U8* y, U8* x,
		U8* ncells, BOOL* speedrun, bitmap_t* exmap, set_t* visits, floodfill_t* values)
{
	U8 x2 = *x;
	U8 y2 = *y;

	while (isVisited(visits, *y, *x) && !getWall(exmap, *y, *x, facing) && *ncells < 4)	// no wall in front
	{
		if (*n) y2++;
		else if (*e) x2++;
		else if (*s) y2--;
		else if (*w) x2--;

		// next cell has a lower value
		if (isVisited(visits, y2, x2) && (values->data[ffGetPos(y2, x2)] < values->data[ffGetPos(*y, *x)]))
		{
			// set position into next cell
			*x = x2;
			*y = y2;
			*ncells += 1;
			*speedrun = TRUE;
		}
		else
		{
			break;
		}
	}
}

void errorCorrect(direction_t facing, U8 y, U8 x, bitmap_t* exmap, BOOL speedrun)
{
	U8 currentReading;	// for current sensor reading
	direction_t left, right;

	getLeftRightDir(facing, &left, &right);

	// Front correction
	if (sensors[SENSOR_CENTER].history[2] - sensors[SENSOR_CENTER].history[3] > 20)
	{
		if (sensors[SENSOR_CENTER].history[3] <= SENSOR_ECC_FRONT) // too close, stops
		{
			cli();	// turn off interrupts
			DS.L.D = 0;
			DS.L.a_max = DS.L.a.freq = DS.L.v_target = DS.L.v.freq = 0;
			DS.v_l.freq = 0;
			DS.v_r.freq = 0;
			DS.L.done = TRUE;
			sei();
		}
	}

	if (getWall(exmap, y, x, left) | getWall(exmap, y, x, right))
	{
		if (!DS.correctionMode) { // not in correction mode
			// check for need of correction
			// Left error correction
			currentReading = sensors[SENSOR_LEFT].history[3];
			if(currentReading > SENSOR_WALLDETECT_LEFT)
			{
				if (currentReading <= SENSOR_ECC_MEDIAN_LEFT-5) {	// close to left side
					// add angular movement toward right side
					DS.v_r.freq += (speedrun)?40:30;
					DS.correctionMode = TRUE;
				} else if (currentReading >= SENSOR_ECC_MEDIAN_LEFT+5) { // close to right side
					// add angular movement toward left side
					DS.v_l.freq += (speedrun)?40:30;
					DS.correctionMode = TRUE;
				}
			}
			else // right
			{
				// Right error correction
				currentReading = sensors[SENSOR_RIGHT].history[3];
				if (currentReading > SENSOR_WALLDETECT_RIGHT) {	// has a wall
					if (currentReading <= SENSOR_ECC_MEDIAN_RIGHT-5) {	// close to right side
						// add angular movement toward left side
						DS.v_l.freq += (speedrun)?40:30;
						DS.correctionMode = TRUE;
					} else if (currentReading >= SENSOR_ECC_MEDIAN_RIGHT+5) { // close to left side
						// add angular movement toward right side
						DS.v_r.freq += (speedrun)?40:30;
						DS.correctionMode = TRUE;
					}
				}
			}
		} else {	// in correction mode, monitor correction
			// Left error correction
			currentReading = sensors[SENSOR_LEFT].history[3];
			if (currentReading > SENSOR_ECC_MEDIAN_LEFT-5
					&& currentReading < SENSOR_ECC_MEDIAN_LEFT+5)
			{	// restore speeds
				if (DS.v_l.freq > DS.v_r.freq)
					DS.v_l.freq = DS.v_r.freq;
				else
					DS.v_r.freq = DS.v_l.freq;
				DS.correctionMode = FALSE;
			} else { // check right sensor
				currentReading = sensors[SENSOR_RIGHT].history[3];
				if (currentReading > SENSOR_ECC_MEDIAN_RIGHT-5
						&& currentReading < SENSOR_ECC_MEDIAN_RIGHT+5)
				{
					if (DS.v_l.freq > DS.v_r.freq)
						DS.v_l.freq = DS.v_r.freq;
					else
						DS.v_r.freq = DS.v_l.freq;
					DS.correctionMode = FALSE;
				}
			}
		}
	}
}

void exportData()
{
//	if (!fullSpeedRun)
//	{
//		// export map to eeprom
//		for (i = 0; i < BLOCK_ARRAY_SIZE; i++) {
//			EEPROM_write(i+60, exmap.bitmapV[i]);
//			EEPROM_write(i+90, exmap.bitmapH[i]);
//		}
//
//		// export visits to eeprom
//		for (i = 0; i < SET_SIZE; i++) {
//			EEPROM_write(i+120, visits.data[i]);
//		}
//	}
}

void explore()
{
	U8 x, y, facing = NORTH;
	U8 ncells;	// for optimization
	BOOL speedrun = FALSE;
	set_t dest; // for destination cells
	set_t visits;	// store visited cells
	floodfill_t values; // store the ff values
	bitmap_t exmap; // exploration map

	/* initializations */
	x = y = 0;	// set start position
	clearMap(&exmap);	// clear exploration map
	memset(&visits, 0, sizeof(set_t));	// clear visited cells
	setWall(&exmap, 0, 0, EAST, 1);	// set starting right wall
	visits.data[0] = 1;	// starting cell is visited

	/* start of exploration */
	while (1)
	{
		setClearSet(&dest);	// clear destination set
		setSetBit(&dest, ffGetPos(7,7)); // set destination cells
		setSetBit(&dest, ffGetPos(8,7));
		setSetBit(&dest, ffGetPos(7,8));
		setSetBit(&dest, ffGetPos(8,8));

		floodFill(&exmap, &values, &dest);	// initial floodfill

		/* our main exploration loop */
		while (values.data[ffGetPos(y, x)] != 0)
		{
			U8 n,e,s,w;	// used to determine direction to move into
			n = e = s = w = 255;	// initially set to high ff value

			// detect and set all the walls in next cell
			detectWalls(facing, &exmap, y, x);

			// recalculate floodfill
			floodFill(&exmap, &values, &dest);

			// retrieve ff values of adjacent cells relative to current cell
			if (!getWall(&exmap, y, x, EAST) && (x+1 < 16))
				e = values.data[ffGetPos(y, x+1)];
			if (!getWall(&exmap, y, x, NORTH) && (y+1 < 16))
				n = values.data[ffGetPos(y+1, x)];
			if (!getWall(&exmap, y, x, WEST) && (x != 0))
				w = values.data[ffGetPos(y, x-1)];
			if (!getWall(&exmap, y, x, SOUTH) && (y != 0))
				s = values.data[ffGetPos(y-1, x)];

			// find shortest path
			findMinPath(&n, &e, &s, &w, x, y, &visits);

			// turn to direction and move to lower-value cell
			makeTurn(&n, &e, &s, &w, &facing);

			if (n | e | s | w)	// if there is a path to go
			{
				if (n) y++;
				else if (e) x++;
				else if (s) y--;
				else if (w) x--;
				ncells = 1;

				/* optimized path for known cells */
				optimizePath(facing, &n, &e, &s, &w, &y, &x, &ncells, &speedrun, &exmap, &visits, &values);

				DSNextCells(&DS, ncells);	// set cell destination

				/* move to cell */
				DS.correctionMode = FALSE;
				while (!DS.L.done)
				{
					errorCorrect(facing, y, x, &exmap, speedrun);	// correction while moving
				}

				/* front error correction if too far from front wall */
				if (getWall(&exmap, y, x, facing))	// if there is a wall in front
				{
					if (sensors[SENSOR_CENTER].history[3] >= 126)	// too far from wall
					{
						DSMoveForward(&DS, 70);	// step 70 more steps
						while(!DS.L.done);
					}
				}
			}

			speedrun = FALSE;	// end speedrun

			// set cell as visited
			visits.data[ffGetPos(y, x)/8] |= (1 << (ffGetPos(y, x) % 8));

			// exportData();

			// delay a bit
			_delay_ms(100);

			if (values.data[ffGetPos(y, x)] == 0) {	// explore back to starting point
				setClearSet(&dest);	// clear destination set
				setSetBit(&dest, ffGetPos(0,0)); // set destination cell
				floodFill(&exmap, &values, &dest);
			}
		}
	}
}

U8 min(U8 x, U8 y)
{
	if (x < y)
		return x;
	else
		return y;
}

void findMinPath(U8 *n, U8 *e, U8 *s, U8 *w, U8 x, U8 y, set_t* visits)
{
	U8 o = min(*n, min(*e, min(*s, *w)));
	U8 *dirValues[4];
	U8 i, j, ix, jx, iy, jy;
	ix = jx = x;
	iy = jy = y;
	U8 repeats = 0;

	dirValues[NORTH] = n;
	dirValues[EAST] = e;
	dirValues[SOUTH] = s;
	dirValues[WEST] = w;

	// look for repeated values
	if (o != 255) {	// 255 = wall, no two cells can be both 255
		if (o == *n) repeats++;
		if (o == *e) repeats++;
		if (o == *s) repeats++;
		if (o == *w) repeats++;
	} else {	// a dead end maze, no way we can reach destination
		// don't go anywhere
		*n = *e = *s = *w = 0;
		return;
	}

	if (repeats > 1) {	// found repeated values
		// find the two adjacent cells that have same value
		for (i = 0; i < 4; i++) {
			for (j = i + 1; j < 4; j++) {
				if (*dirValues[i] == o && *dirValues[i] == *dirValues[j]) {
					// get coords of the two adjacent cells
					iy = (i == NORTH) ? iy+1 : (i == SOUTH) ? iy-1 : iy;
					ix = (i == EAST) ? ix+1 : (i == WEST) ? ix-1 : ix;
					jy = (j == NORTH) ? jy+1 : (j == SOUTH) ? jy-1 : jy;
					jx = (j == EAST) ? jx+1 : (j == WEST) ? jx-1 : jx;
					// check if a cell is not visited
					*dirValues[NORTH] = *dirValues[EAST] = 0;
					*dirValues[SOUTH] = *dirValues[WEST] = 0;
					if (!(visits->data[ffGetPos(iy, ix)/8] & (1 << (ffGetPos(iy, ix)%8))))
						*dirValues[i] = 1;	// go to first path
					else if (!(visits->data[ffGetPos(jy, jx)/8] & (1 << (ffGetPos(jy, jx)%8))))
						*dirValues[j] = 1;	// go to second path
					else	// both visited
						*dirValues[i] = 1;	// default to first path
					return;
				}
			}
		}
	} else { // no repeated value, go to lower value cell
		if (o == *n) {
			*n = 1;	*e = *s = *w = 0;
		} else if (o == *e) {
			*e = 1;	*n = *s = *w = 0;
		} else if (o == *s) {
			*s = 1;	*n = *e = *w = 0;
		} else if (o == *w) {
			*w = 1;	*n = *s = *e = 0;
		}
	}
}
