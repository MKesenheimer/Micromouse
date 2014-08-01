/**
 * Compiler configuration file
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

// Processor definitions
#ifdef __AVR__
#	define __MM_PROC_AVR__
#elif defined(_WIN32)
#	define __MM_PROC_X86__
#else
#	error Processor not defined!
#endif

// Compiler definitions
#ifdef _MSC_VER
#	define __MM_COMP_VC__
#elif defined(__AVR__)
#	define __MM_COMP_AVR__
#elif defined(_GNUC_)
#	define __MM_COMP_GCC__
#else
#	error Compiler not defined!
#endif

// Configurations

#endif
