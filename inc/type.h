/**
 * Type definitions
 */

#ifndef _TYPE_H_
#define _TYPE_H_

#if defined(__GCC__) || defined(__AVR__)
#	include <stdint.h>
#endif

#ifdef __AVR__
	#include <inttypes.h>
#endif

#if defined(__MM_COMP_VC__) || defined(__MM_COMP_GCC__)
	typedef unsigned char	U8;
	typedef char			S8;
	typedef unsigned short	U16;
	typedef short			S16;
	typedef unsigned long	U32;
	typedef long			S32;
	typedef U8				BOOL;
	typedef S8				C8;
#elif defined(__MM_COMP_AVR__)
	typedef uint8_t			U8;
	typedef int8_t			S8;
	typedef uint16_t		U16;
	typedef int16_t			S16;
	typedef uint32_t		U32;
	typedef int32_t			S32;
	typedef U8				BOOL;
	typedef S8				C8;
#endif

#endif
