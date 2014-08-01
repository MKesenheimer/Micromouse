#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/maze2.h"
#include "../inc/set.h"
#include "../inc/floodfill.h"
#include "../inc/motion.h"
#include "../inc/sensor.h"
#include "../inc/exploration.h"
#include "../inc/tests.h"

U8 BitCount(U8 n)
{
    U8 count = 0;
    while (n != 0)
    {
        count++;
        n &= (n - 1);
    }
    return count;
}

#ifndef __MM_COMP_AVR__

FILE* getFStream(const char *filename)
{
	FILE* f = fopen(filename, "r");

	printf("Reading file %s\n", filename);

	if (!f || ferror(f)) {
		perror("Error opening file");
		return NULL;
	}

	return f;
}

void TestSet()
{
	set_t setS;
	U16 i = 0;

	fprintf(stdout, "Testing Set\n");
	setClearSet(&setS);

	for (; i < 255; i+=2)
		setSetBit(&setS, (U8)i);

	while (!setIsEmpty(&setS)) {
		printf("%d\n", setGetFirstAndClear(&setS));
	}
}

void TestFF() {

	bitmap_t maze2; // create our maze
	bitmap_t maze; // import existing maze

	fprintf(stdout, "Testing FloodFill\n");

	clearMap(&maze2);
	setWall(&maze2, 0, 0, EAST, 1);
	setWall(&maze2, 7, 8, EAST, 1);
	setWall(&maze2, 8, 8, EAST, 1);
	setWall(&maze2, 8, 8, NORTH, 1);
	setWall(&maze2, 8, 7, NORTH, 1);
	setWall(&maze2, 8, 7, WEST, 1);
	setWall(&maze2, 7, 7, WEST, 1);

	exportMap(stdout, &maze2);

	clearMap(&maze);
	importMap(fopen("../../../../mazes/1stworld.maze", "r"), &maze);

	exportMap(stdout, &maze);

	{
		set_t dest; // set the destination cells
		set_t visits;
		floodfill_t values; // get the ff values

		setClearSet(&visits);
		setClearSet(&dest);
		setSetBit(&dest, 119);
		setSetBit(&dest, 120);
		setSetBit(&dest, 135);
		setSetBit(&dest, 136);

		floodFill(&maze, &values, &dest);

		exportFF(stdout, &maze, &values, &visits, 0);
	}

}

C8 SkipWhitespaces(FILE *fp)
{
	C8 c;
	fread(&c, sizeof(C8), 1, fp);
	while(IS_SPACE(c))
		fread(&c, sizeof(C8), 1, fp);
	return c;
}

BOOL ReadMaze(bitmap_t* bitmap)
{
	C8 buf[200];
	FILE *fp;

	printf("Input maze file name: ");
	scanf("%s", buf);

	if(!strncmp(buf, "quit", 200))
	{
		return FALSE;
	}

	fp = getFStream(buf);

	if(fp == NULL)
	{
		return FALSE;
	}
	importMap(fp, bitmap);
	return TRUE;
}

void TestExploration(const char* file)
{
	U8 x, y;
	bitmap_t map; // existing map
	bitmap_t exmap; // exploration map

	clearMap(&map);
	clearMap(&exmap);	// clear exploration map
	importMap(fopen(file, "r"), &map);
	setWall(&exmap, 0, 0, EAST, 1);	// set starting wall
	exportMap(stdout, &exmap);

	{
		x = y = 0;	// set start position
		set_t dest; // for destination cells
		floodfill_t values; // get the ff values
		set_t visits;	// store visited cells
		U8 stopVar;		// for scanf
		memset(&visits, 0, sizeof(set_t));	// clear visited cells
		visits.data[0] = 1;	// starting cell is visited

		while (1)
		{
			setClearSet(&dest);	// clear destination set
			setSetBit(&dest, ffGetPos(7,7)); // set destination cells
			setSetBit(&dest, ffGetPos(8,7));
			setSetBit(&dest, ffGetPos(7,8));
			setSetBit(&dest, ffGetPos(8,8));

			floodFill(&exmap, &values, &dest);
			exportFF(stdout, &exmap, &values, &visits, 0);

			while (values.data[ffGetPos(y, x)] != 0)
			{
				U8 n,e,s,w;
				n = e = s = w = 255;

				scanf("%c", &stopVar);	// pause for user

				// set all the walls
				if (getWall(&map, y, x, EAST))
					setWall(&exmap, y, x, EAST, 1);
				else if (x+1 < 16)
						e = values.data[ffGetPos(y, x+1)];
				if (getWall(&map, y, x, NORTH))
					setWall(&exmap, y, x, NORTH, 1);
				else if (y+1 < 16)
						n = values.data[ffGetPos(y+1, x)];
				if (getWall(&map, y, x, WEST))
					setWall(&exmap, y, x, WEST, 1);
				else if (x != 0)
						w = values.data[ffGetPos(y, x-1)];
				if (getWall(&map, y, x, SOUTH))
					setWall(&exmap, y, x, SOUTH, 1);
				else if (y != 0)
						s = values.data[ffGetPos(y-1, x)];

				// recalculate floodfill
				floodFill(&exmap, &values, &dest);
				exportFF(stdout, &exmap, &values, &visits, ffGetPos(y, x)); // draw floodfill map

				// find shortest path
				findMinPath(&n, &e, &s, &w, x, y, &visits);

				// move to lower value cell
				if (n) y++;
				else if (e) x++;
				else if (s)	y--;
				else if (w) x--;

				// set cell as visited
				visits.data[ffGetPos(y, x)/8] |= (1 << (ffGetPos(y, x) % 8));

				if (values.data[ffGetPos(y, x)] == 0) {	// explore back to starting point
					scanf("%c", &stopVar);
					setClearSet(&dest);	// clear destination set
					setSetBit(&dest, ffGetPos(0,0)); // set destination cell
					floodFill(&exmap, &values, &dest);
					exportFF(stdout, &exmap, &values, &visits, ffGetPos(y, x));
				}
			}
		}
	}
}

#endif

void TestSensors()
{

}
