/**
 * Set implementation
 */

#include "../inc/common.h"
#include "../inc/set.h"
#include <string.h>
#include <assert.h>

/**
 * Data Structure:
 * [ 7| 6| 5| 4| 3| 2|1|0]
 * [15|14|13|12|11|10|9|8]
 */

/**
 * Clears the set
 */
void setClearSet(set_t* set)
{
//	U8 i;
//	for (i = 0; i < SET_SIZE; i++) {
//		set->data[i] = 0;
//	}
	memset(set, 0, sizeof(set_t));
}

/**
 * Sets a bit in a set
 * set - the set data structure
 * pos - bit position
 */
void setSetBit(set_t* set, U8 pos)
{
	SET_BIT((set->data[pos / SET_DATATYPE_BITS]),(1 << pos % SET_DATATYPE_BITS));
}

void setClearBit(set_t* set, U8 pos)
{
	CLEAR_BIT((set->data[pos / SET_DATATYPE_BITS]),(1 << pos % SET_DATATYPE_BITS));
}

/**
 * return > 0 if bit is set
 * return 0 otherwise
 */
BOOL setIsSet(set_t* set, U8 pos)
{
	return IS_SET((set->data[pos / SET_DATATYPE_BITS]),(1 << pos % SET_DATATYPE_BITS));
}

/**
 * get the position of first set bit, and clear it
 */
U8 setGetFirstAndClear(set_t* set)
{
	U8 i;

//	assert(setIsEmpty(set) == FALSE); // make sure set isn't empty
	for (i = 0; i < SET_SIZE; i++) {
		if (set->data[i]) { // if set[i] not empty(0)
			U8 j;
			U8 mask = BIT_0; // start with rightmost bit set
			for (j = 0; j < SET_DATATYPE_BITS; j++) {
				if (set->data[i] & mask) { // if a bit is set
					U8 position = i * SET_DATATYPE_BITS + j;
					setClearBit(set, position);
					return position;
				}
				mask <<= 1; // shift the mask bit left
			}
		}
	}
//	assert(0); // shouldn't reach here if set isn't empty
	return 255;
}

BOOL setIsEmpty(set_t* set) {
	U8 i = 0;
	for(; i < SET_SIZE; i++)
		if (set->data[i])
			return FALSE; //not empty
	return TRUE; //empty
}


