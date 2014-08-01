/*
 * motion.h
 *
 *  Created on: Dec 21, 2008
 *      Author: Eddy
 */

#ifndef MOTION_H_
#define MOTION_H_

#include "common.h"

#define FREQ_MAIN 5000

#define DS_NEXTCELL 722	// number of steps from center of cell to adjacent cell
#define DS_TURN90 277
#define DS_TURN180 554
#define DS_LEFT 0
#define DS_RIGHT 1

typedef struct FreqDiv {
	S16 counter;
	S16 freq;
	void *action_param;
	void (*action)(S8,void*);
} FreqDiv_t;

typedef struct MotionControl {
	FreqDiv_t v;
	FreqDiv_t a;
	S32 D;
	S32 D_last;
	S16 v_target;
	S16 v_target_last;
	S16 a_max;
	S32 twoAD;
	S32 v_squared;
	volatile BOOL done;
} MotionControl_t;

typedef struct DiffSteer {
	struct MotionControl L; //linear
	struct MotionControl A; // angular
	struct FreqDiv v_l; // left wheel
	struct FreqDiv v_r; // right wheel
	BOOL correctionMode;
} DiffSteer_t;

void InitMotionISR();
void MotionInterruptHandler();

void FDInit(FreqDiv_t* fd);

void DSInit(DiffSteer_t* ds);
void DSMove(DiffSteer_t* ds);
void DSTurn(DiffSteer_t* ds, U16 n, U8 dir);		// turn in-place left or right
void DSNextCells(DiffSteer_t* ds, S8 ncells);	// move n cells from current cell
void DSMoveForward(DiffSteer_t* ds, S8 n);

#endif /* MOTION_H_ */
