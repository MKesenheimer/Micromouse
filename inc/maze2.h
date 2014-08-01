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

/*! \mainpage libmaze Documentation
\htmlinclude main.html
*/
/*! \page mazepage Mazes
\htmlinclude mazes.html
*/

#ifndef _MAZE_H_
#define _MAZE_H_

#include "common.h"
#include "stdio.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef ERROR
#define ERROR -1
#endif

#define MAZE_WIDTH 16
#define MAZE_HEIGHT 16

/// \brief size (# of bits) of the block data type.
/*! It is 8 (bits) for now since we are using unsigned char here. We can change it to 16
 if we decide to use \e uint16 from avr-libc inttypes.h
*/
#define BLOCK_SIZE 8

/// \brief size of bitmap arrays in the bitmap_t struct.
/*! \e BLOCK_SIZE*BLOCK_ARRAY_SIZE should always be 240, since the block_t arrays bitmapV and
bitmapH holds 240 bits, for 240 partitions in each direction (horizontal,vertical) \n
For block size of 8 (char), \e BLOCK_ARRAY_SIZE should be 30. \n
Likewise if we decide to use \e uint16 (\e BLOCK_SIZE = 16), \e BLOCK_ARRAY_SIZE should be 15.
*/
#define BLOCK_ARRAY_SIZE 30

/// \brief maximum number of partitions.
#define PARTITIONS_MAX 480

#define EQU_SOUTH(X,Y) (X+(Y-1)*16)
#define EQU_EAST(X,Y) ((X*16)+Y)
#define EQU_NORTH(X,Y) ((Y*16)+X)
#define EQU_WEST(X,Y) (((X-1)*16)+Y)

/// A block_t is a data type element with a set number of bits to represent partitions. \n
/// It is defined this way so we can use uint16 from AVR libc instead of unsigned char
/// if we ever decide to port the code to AVR.
typedef unsigned char block_t;

typedef signed char bool_t;

/// Bitmap structure containing maps of vertical and horizontal partitions
typedef struct {
	block_t bitmapV[BLOCK_ARRAY_SIZE]; /// Vertical partitions
	block_t bitmapH[BLOCK_ARRAY_SIZE]; /// Horizontal partitions
} bitmap_t;

typedef enum {
	NORTH = 0,
	SOUTH,
	EAST,
	WEST
} direction_t;

void importMap(FILE* fs, bitmap_t* bitmap);
bool_t exportMap(FILE* fs, const bitmap_t* bitmap);
void clearMap(bitmap_t* bitmap);
bool_t setWall(bitmap_t* bitmap, U8 y, U8 x, direction_t dir, bool_t value);
bool_t getWall(const bitmap_t* bitmap, U8 y, U8 x, direction_t dir);

#endif // _MAZE_H_
