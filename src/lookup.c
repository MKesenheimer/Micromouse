/**
 * Lookup tables
 */

#include "../inc/lookup.h"

// Lookup table for bit masking, this is used to speed up the code,
// because less shifting operations are used for computing bit masks.
// Small lookup tables do not cause that much cache/page misses if the
// processor actually provides any.
//
// Since this is a constant table, be sure to store it inside the program
// space (ROM) rather than data space (RAM) for computers that use the
// harvard architecture.

const U8 tMask[NUM_MASK] = {
	1,
	2,
	4,
	8,
	16,
	32,
	64,
	128,
};
