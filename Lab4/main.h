//===============================================================================
// Dragon12 test program.
//
// same basic features as test.asm
// this is the CW12 v4.5 tool chain version.
//
// This assumes a vector forwardng table at $3E00 in
// DBG12 format.
//
//===============================================================================

#define MAIN_H

#define GCC_TOOLCHAIN 1
#ifdef GCC_TOOLCHAIN
#define __interrupt   __attribute__((interrupt))
#define __far         __attribute__((far))
#include "vectors12.h"
#include "hcs12.h"
#endif


#ifndef GCC_TOOLCHAIN
#include <mc9s12dp512.h>
#include "dbug12.h"
#endif

__interrupt void RealTimeInterrupt(void);
__interrupt void Timer6Interrupt(void);
__interrupt void Timer5Interrupt(void);
__interrupt void Timer4Interrupt(void);
__interrupt void Timer3Interrupt(void);
__interrupt void SCI0Interrupt(void);

#define _DBUG12   1
#ifndef byte
#define byte unsigned char
#endif

#ifndef word
#define word unsigned short
#endif





