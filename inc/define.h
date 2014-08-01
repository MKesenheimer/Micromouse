/**
 * Definitions
 */

#ifndef _DEFINE_H_
#define _DEFINE_H_

#define BIT_0		1
#define BIT_1		2
#define BIT_2		4
#define BIT_3		8
#define BIT_4		16
#define BIT_5		32
#define BIT_6		64
#define BIT_7		128

#define TRUE		1
#define FALSE		0

#ifndef NULL
	#define NULL		((void *)0)
#endif

#define PRIVATE		static

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/* UART baud rate */
#define UART_BAUD  9600

#endif
