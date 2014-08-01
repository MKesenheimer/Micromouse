/***************************************************************************
 *   Copyright (C) 2008 by Eddy Chan
 *   edc529@gmail.com
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../inc/common.h"
#include "../inc/maze2.h"

/// \brief Set the bit to indicate the presence of a partition
//! \param *b A block_t array. (Either bitmapH or bitmapV)
//! \param partition_num The partition number. (between 0 and PARTITIONS_MAX/2 - 1)
//! \param value 0 (FALSE) - clear wall, TRUE or any other number sets the wall
//! \retval TRUE on success.
//! \retval FALSE on error
PRIVATE bool_t _setWall(block_t* b, U8 partition_num, bool_t value) {
	/// \internal (partition_num < 0) isn't checked because of unsigned
	if (partition_num > ((PARTITIONS_MAX/2) -1))
		return FALSE;
	if (value)
		b[partition_num / BLOCK_SIZE] |= (1 << (partition_num % BLOCK_SIZE));
	else
		b[partition_num / BLOCK_SIZE] &= ~(1 << (partition_num % BLOCK_SIZE));
	return TRUE;
}

/// \brief set or clear a partition
/// \param *bitmap the bitmap structure
/// \param y row
/// \param x column
/// \param value see \ref setWall(...)
/// \return see \ref setWall(...)
bool_t setWall(bitmap_t* bitmap, U8 y, U8 x, direction_t dir, bool_t value) {
	if ((y > 15) & (x > 15))
		return FALSE;
	switch (dir) {
	case SOUTH:
		if (y == 0) return 1; /// south outer walls always present
		return _setWall(bitmap->bitmapH, EQU_SOUTH(x,y), value);
	case EAST:
		if (x == 15) return 1; /// east outer walls always present
		return _setWall(bitmap->bitmapV, EQU_EAST(x,y), value);
	case NORTH:
		if (y == 15) return 1; /// north outer walls always present
		return _setWall(bitmap->bitmapH, EQU_NORTH(x,y), value);
	case WEST:
		if (x == 0) return 1; /// west outer walls always present
		return _setWall(bitmap->bitmapV, EQU_WEST(x,y), value);
	default:
		return ERROR;
	}
	return ERROR;
}

/// \brief check whether partition exist given a block_t (uni-directional) array and partition number
//! \param *b the block_t array, either the uni-directional bitmapV or bitmapH block_t array
//! \param partition_num the partition number. Range 0 to PARTITIONS_MAX/2-1
//! \retval >0 if wall (partition) is present.
//! \retval 0 if wall is not present.
//! \retval -1 if an error occurs
PRIVATE bool_t _getWall(const block_t* b, U8 partition_num) {
	if (partition_num > ((PARTITIONS_MAX/2) -1))
		return ERROR;
	return (b[partition_num / BLOCK_SIZE] & (1 << (partition_num % BLOCK_SIZE)));
}

/// \brief check whether a wall exist given the coordinate
//! \param *bitmap the bitmap structure
//! \param y the row position
//! \param x the column position
//! \return see \ref getWall(...)
bool_t getWall(const bitmap_t* bitmap, U8 y, U8 x, direction_t dir) {
	if ((y > 15) | (x > 15))
		return ERROR;
	switch (dir) {
	case SOUTH:
		if (y == 0) return TRUE; /// south outer walls always present
		return _getWall(bitmap->bitmapH, EQU_SOUTH(x,y));
	case EAST:
		if (x == 15) return TRUE; /// east outer walls always present
		return _getWall(bitmap->bitmapV, EQU_EAST(x,y));
	case NORTH:
		if (y == 15) return TRUE; /// north outer walls always present
		return _getWall(bitmap->bitmapH, EQU_NORTH(x,y));
	case WEST:
		if (x == 0) return TRUE; /// west outer walls always present
		return _getWall(bitmap->bitmapV, EQU_WEST(x,y));
	default:
		return ERROR;
	}
	return ERROR;
}

/// \brief internal function to read vertical walls
PRIVATE void readVWall(FILE* f, bitmap_t* bitmap, U8 y)
{
	char buffer[35];
	U8 x;

	assert(f);


	fgets(buffer, 3, f); /// discard outer wall partition and space
	for (x = 0; x < 15; x++) {
		switch(fgetc(f)) {
		case '|':
			setWall(bitmap, y, x, EAST, 1); break;
		case ' ':
			setWall(bitmap, y, x, EAST, 0); break;
		default:
			assert(0);
		}
		fgetc(f); /// discard peg
	}
	fgets(buffer, 3, f); /// discard outer wall partition
}

/// \brief internal function to read horizontal walls
PRIVATE void readHWall(FILE* f, bitmap_t* bitmap, U8 y)
{
	char buffer[35]; /// buffer used to store discarded characters
	U8 x;

	assert(f);


	fgetc(f); /// discard outer peg
	for (x = 0; x < 16; x++) {
		switch(fgetc(f)) {
		case '-':
			setWall(bitmap, y, x, SOUTH, 1); break;
		case ' ':
			setWall(bitmap, y, x, SOUTH, 0); break;
		default:
			assert(0);
		}
		fgetc(f); /// discard peg
	}
	fgets(buffer, 3, f); /// discard newline
}

/// \brief Import a maze map represented in ASCII format to a bitmap.\n
//! Maps should be 33 lines by 34 characters including newlines
//! \param *fs file stream pointer
//! \param *bitmap pointer to bitmap structure
//! \return Nothing. \n \e bitmap will be loaded.
void importMap(FILE* fs, bitmap_t* bitmap)
{
	char buffer[35];
	short i;

	assert(fs && bitmap);

	fgets(buffer, 35, fs); // skip first line, top outer wall
	for (i = 15; i >= 0; i--) { // count backward since we are reading from top
		readVWall(fs, bitmap, (U8)i); // read in vertical walls
		if (i) readHWall(fs, bitmap, (U8)i); // then read in horizontal walls
	}
	if (fs) fclose(fs);
}

bool_t exportMap(FILE* fs, const bitmap_t* bitmap)
{
#ifndef __MM_COMP_AVR__
	U8 x = 0, y = 0, yy = 0;
	FILE* file_desc = stdout;

	assert(bitmap);

	if (fs)
		file_desc = fs;

	fprintf(file_desc,"+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	for (yy = 0; yy < 16; yy++) {
		y = 15 - yy;
		fprintf(file_desc,"|");
		for (x = 0; x < 16; x++) {
			fprintf(file_desc," ");
			if (getWall(bitmap, y, x, EAST))
				fprintf(file_desc,"|");
			else
				fprintf(file_desc," ");
		}
		fprintf(file_desc,"\n+");
		for (x = 0; x < 16; x++) {
			if (getWall(bitmap, y, x, SOUTH))
				fprintf(file_desc,"-");
			else
				fprintf(file_desc," ");
			fprintf(file_desc,"+");
		}
		fprintf(file_desc,"\n");
	}
	fprintf(file_desc,"\n");

	if (file_desc && (file_desc != stdout))
		fclose(file_desc);
#endif
	return TRUE;
}

void clearMap(bitmap_t* bitmap) {
	memset(bitmap, 0, sizeof(bitmap_t));
//	U8 i;
//	for (i = 0; i < BLOCK_ARRAY_SIZE; i++) {
//		bitmap->bitmapH[i] = 0x0;
//		bitmap->bitmapV[i] = 0x0;
//	}
}
