/*
 * motion.c
 *
 *  Created on: Dec 21, 2008
 *      Author: Eddy
 */

#include <stdlib.h>
#include <string.h>
#include "../inc/common.h"
#include "../inc/motion.h"
#include "../inc/sensor.h"

#ifdef __MM_COMP_AVR__
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

DiffSteer_t DS;
extern Sensor_t sensors[3];

PRIVATE void LinAccAction(S8 dir,DiffSteer_t* ds);
PRIVATE void LinVelAction(S8 dir,DiffSteer_t* ds);
PRIVATE void AngAccDoubleLeftAction(S8 dir,DiffSteer_t* ds);
PRIVATE void AngAccDoubleRightAction(S8 dir,DiffSteer_t* ds);
PRIVATE void AngAccSingleLeftAction(S8 dir,DiffSteer_t* ds);
PRIVATE void AngAccSingleRightAction(S8 dir,DiffSteer_t* ds);
PRIVATE void AngVelAction(S8 dir,DiffSteer_t* ds);
PRIVATE void VLeftAction(S8 dir,DiffSteer_t* ds);
PRIVATE void VRightAction(S8 dir,DiffSteer_t* ds);

ISR(TIMER1_COMPA_vect)	// OC1A motion control interrupt handler
{
	PORTA |= 0x11;			// finish last step pulse
	DSMove(&DS);			// perform next step
	//PORTA ^= 0x11;
	OCR1A += 400;			// set for another interrupt
	//OCR1A += 100;
}

void InitMotionISR()
{
	DDRA |= 0x77;			// port A motor output
	TCCR1A = _BV(COM1A0);	// toggle output on OC1A
	TCCR1B = _BV(CS11);		// prescale 8
	OCR1A = 400;			// 5KHz compare period, 2.5kHz waveform
	DDRB = _BV(PB5);		// set output on OC1A
	TIMSK = _BV(OCIE1A);	// enable OC1A interrupt
}

void FDInit(FreqDiv_t* fd)
{
	fd->counter = 0;
	fd->freq = 0;
	fd->action_param = NULL;
	fd->action = NULL;
}

PRIVATE void FDTick(FreqDiv_t* fd)
{
	fd->counter += fd->freq;

	if (fd->counter >= FREQ_MAIN)
	{
		fd->counter -= FREQ_MAIN;
		fd->action(1,fd->action_param);
	}
	else if (fd->counter <= -FREQ_MAIN)
	{
		fd->counter += FREQ_MAIN;
		fd->action(-1,fd->action_param);
	}

}

PRIVATE void MCMove(DiffSteer_t *ds, MotionControl_t *mc)
{
	if (mc->v_target != mc->v_target_last)
	{
		if (mc->v.freq > mc->v_target) 		// decelerate
			mc->a.freq = -(mc->a_max);
		else if (mc->v.freq < mc->v_target) // accelerate
			mc->a.freq = mc->a_max;
		else								// constant velocity
			mc->a.freq = 0;
		mc->v_target_last = mc->v_target;
	}

	if (mc->D != mc->D_last)
	{
		if (mc->D > 0 && ((2 * (S32)mc->a.freq * (S32)mc->D) <= ((S32)mc->v.freq * (S32)mc->v.freq)))
			mc->v_target = 0;	// start deceleration
		else if (mc->D < 0 && (-(2 * (S32)mc->a.freq * (S32)mc->D) <= ((S32)mc->v.freq * (S32)mc->v.freq)))
			mc->v_target = 0;	// start acceleration
		else if (mc->D == 0)	// destination reached
		{
			mc->a_max = mc->a.freq = mc->v_target = mc->v.freq = 0;
			ds->v_l.freq = 0;
			ds->v_r.freq = 0;
			mc->done = TRUE;
		}
		mc->D_last = mc->D;
	}

	FDTick(&(mc->a));
	FDTick(&(mc->v));
	FDTick(&ds->v_l);
	FDTick(&ds->v_r);
}

void DSInit(DiffSteer_t* ds)
{
	memset(ds, 0, sizeof(DiffSteer_t));
	ds->v_l.action_param = (DiffSteer_t*)ds;	// left motor action
	ds->v_l.action = (void*)VLeftAction;
	ds->v_r.action_param = (DiffSteer_t*)ds;	// right motor action
	ds->v_r.action = (void*)VRightAction;
	ds->A.done = TRUE;
	ds->L.done = TRUE;
}

void DSMove(DiffSteer_t* ds)
{
	MCMove(ds, &ds->L);		// linear movement
	PORTA |= 0x11;			// finish last step pulse
	MCMove(ds, &ds->A); 	// angular movement
	PORTA |= 0x11;			// finish last step pulse
}

void DSNextCells(DiffSteer_t* ds, S8 ncells)
{
	ds->L.D += DS_NEXTCELL * (S32)ncells;		// Linear distance
	ds->L.a_max = 900;							// Constant linear acceleration
	ds->L.v_target = 1800;						// Max linear velocity
	ds->L.a.action_param = (DiffSteer_t*)ds;	// Linear acceleration action
	ds->L.a.action = (void*)LinAccAction;
	ds->L.v.action_param = (DiffSteer_t*)ds;	// Linear stepping action
	ds->L.v.action = (void*)LinVelAction;
	ds->L.done = FALSE;
}

void DSMoveForward(DiffSteer_t* ds, S8 n)
{
	ds->L.D += (S32)n;		// Linear distance
	ds->L.a_max = 900;							// Constant linear acceleration
	ds->L.v_target = 1800;						// Max linear velocity
	ds->L.a.action_param = (DiffSteer_t*)ds;	// Linear acceleration action
	ds->L.a.action = (void*)LinAccAction;
	ds->L.v.action_param = (DiffSteer_t*)ds;	// Linear stepping action
	ds->L.v.action = (void*)LinVelAction;
	ds->L.done = FALSE;
}

void DSTurn(DiffSteer_t* ds, U16 n, U8 dir)
{
	ds->A.D = (S32)n;							// Angular distance
	ds->A.a_max = 600;
	ds->A.v_target = 800;
	//ds->A.twoAD = ((S32)ds->A.a_max << 1) * (S32)ds->A.D;
	ds->A.a.action_param = (DiffSteer_t*)ds;	// Angular acceleration action
	ds->A.a.action = (dir == DS_LEFT) ? (void*)AngAccDoubleLeftAction : (void*)AngAccDoubleRightAction;
	ds->A.v.action_param = (DiffSteer_t*)ds;	// Angular stepping action
	ds->A.v.action = (void*)AngVelAction;
	ds->A.done = FALSE;
}

void DSErrorCorrectingTurn(DiffSteer_t* ds, U16 n, U8 dir)
{
	ds->A.D = (S32)n;							// Angular distance
	ds->A.a_max = 150;
	ds->A.v_target = 150;
	//ds->A.twoAD = ((S32)ds->A.a_max << 1) * (S32)ds->A.D;
	ds->A.a.action_param = (DiffSteer_t*)ds;	// Angular acceleration action
	ds->A.a.action = (dir == DS_LEFT) ? (void*)AngAccSingleLeftAction : (void*)AngAccSingleRightAction;
	ds->A.v.action_param = (DiffSteer_t*)ds;	// Angular stepping action
	ds->A.v.action = (void*)AngVelAction;
	ds->A.done = FALSE;
}

PRIVATE void LinAccAction(S8 dir, DiffSteer_t* ds)
{
	ds->L.v.freq += dir;
	ds->v_l.freq += dir;
	ds->v_r.freq += dir;
}

PRIVATE void LinVelAction(S8 dir, DiffSteer_t* ds)
{
	ds->L.D--;
}

PRIVATE void AngAccDoubleRightAction(S8 dir, DiffSteer_t* ds)
{
	ds->A.v.freq += dir;
	ds->v_l.freq -= dir;
	ds->v_r.freq += dir;
}

PRIVATE void AngAccDoubleLeftAction(S8 dir, DiffSteer_t* ds)
{
	ds->A.v.freq += dir;
	ds->v_l.freq += dir;
	ds->v_r.freq -= dir;
}

PRIVATE void AngAccSingleRightAction(S8 dir, DiffSteer_t* ds)
{
	ds->A.v.freq += dir;
//	ds->v_l.freq -= dir;
	ds->v_r.freq += dir;
}

PRIVATE void AngAccSingleLeftAction(S8 dir, DiffSteer_t* ds)
{
	ds->A.v.freq += dir;
	ds->v_l.freq += dir;
//	ds->v_r.freq -= dir;
}

PRIVATE void AngVelAction(S8 dir, DiffSteer_t* ds)
{
	ds->A.D--;
}

PRIVATE void VLeftAction(S8 dir, DiffSteer_t* ds)
{
#ifdef __MM_COMP_AVR__
	if (dir > 0)
		PORTA |= 0x20;
	else if (dir < 0)
		PORTA &= ~0x20;
	PORTA &= ~0x10;	// step
#endif
}

PRIVATE void VRightAction(S8 dir, DiffSteer_t* ds)
{
#ifdef __MM_COMP_AVR__
	if (dir > 0)
		PORTA &= ~0x02;
	else if (dir < 0)
		PORTA |= 0x02;
	PORTA &= ~0x01;	// step
#endif
}

