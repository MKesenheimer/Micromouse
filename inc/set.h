/**
 * Set representation
 */

#ifndef _SET_H_
#define _SET_H_

#include "common.h"

#define SET_SIZE	32
#define SET_DATATYPE_BITS 8

typedef struct {
	U8 data[SET_SIZE]; // 8*32 = 256
} set_t;

void setClearSet(set_t* set);
void setSetBit(set_t* set, U8 pos);
void setClearBit(set_t* set, U8 pos);
BOOL setIsSet(set_t* set, U8 pos);
BOOL setIsEmpty(set_t* set);
U8 setGetFirstAndClear(set_t* set);

#endif
