/**
 * Macros
 */

#ifndef _MACRO_H_
#define _MACRO_H_

#define SET_BIT(flag, bit)				(flag |= bit)
#define CLEAR_BIT(flag, bit)			(flag &= ~bit)
#define IS_SET(flag, bit)				(flag & bit)
#define IS_SPACE(c)						((c == ' ') || (c == '\n') || (c == '\t') || (c == '\r'))


#endif
