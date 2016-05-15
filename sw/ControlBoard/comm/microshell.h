/*
 * Micro shell is a shell wrapped around microrl lib.
 * It allows autocomplete, history commands and color.
 *
 * Originally created by Marian Such
 * Modified by Jeremy S
 */

#ifndef _MICROSHELL_H
#define _MICROSHELL_H

#include <stdbool.h>

extern SerialUSBDriver SDU2;

#define chprint(...) chprintf( ( BaseSequentialStream * )&SDU2, __VA_ARGS__ )

void start_shell( void );

typedef void (*shellcmd_t)( int argc, char * argv[] );

typedef struct 
{
    const char        *sc_name;
    shellcmd_t        sc_function;
} ShellCommand;

#endif // _MICROSHELL_H
