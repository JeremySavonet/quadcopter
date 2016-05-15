/*
 * Micro shell is a shell wrapped around microrl lib.
 * It allows autocomplete, history commands and color.
 *
 * Originally created by Marian Such
 * Modified by Jeremy S
 */

#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "chprintf.h"
#include "hal.h"
#include "test.h"

#include "../color.h"

#include "config.h"
#include "microrl.h"
#include "microshell.h"

#define TEST_WA_SIZE    THD_WORKING_AREA_SIZE( 256 )

extern SerialUSBDriver SDU2;

// array for completion
char * compl_world [32];

void print( const char * str )
{
    chprint( "%s", str );
}

/*===========================================================================*/
/* CLI functions here                                                        */
/* TODO : Nice to have little macro to add new cli function                  */
/*===========================================================================*/

static void usage( char *p )
{
    chprint( KYEL "Usage: %s\r\n", p );
}

static void cmd_systime( int argc, char * argv[] )
{
    (void)argv;
    if( argc > 0 )
    {
        usage( "systime" );
        return;
    }
    chprint( "%lu\r\n", (unsigned long)chVTGetSystemTime() );
}

static void cmd_mem( int argc, char *argv[] )
{
    size_t n, size;

    (void)argv;
    if( argc > 0 )
    {
        usage( "mem" );
        return;
    }

    n = chHeapStatus( NULL, &size );
    chprint( "core free memory : %u bytes\r\n", chCoreGetStatusX() );
    chprint( "heap fragments   : %u\r\n", n );
    chprint( "heap free total  : %u bytes\r\n", size );
}

static void cmd_threads( int argc, char *argv[] )
{
    static const char *states[] = { CH_STATE_NAMES };
    thread_t *tp;

    (void)argv;
    if( argc > 0 )
    {
        usage( "threads" );
        return;
    }

    chprint( "    addr    stack prio refs     state time\r\n" );
    tp = chRegFirstThread();
    do
    {
        chprint( "%08lx %08lx %4lu %4lu %9s\r\n",
                  (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
                  (uint32_t)tp->p_prio, (uint32_t)( tp->p_refs - 1 ),
                  states[tp->p_state]);

        tp = chRegNextThread( tp );
    } while( tp != NULL );
}

static void cmd_benchmark( int argc, char *argv[] )
{
    thread_t *tp;

    (void)argv;
    if( argc > 0 )
    {
        usage( "benchmark" );
        return;
    }

    tp = chThdCreateFromHeap( NULL,
                              TEST_WA_SIZE,
                              chThdGetPriorityX(),
                              TestThread,
                              &SDU2 );
    if( tp == NULL )
    {
        chprint( "out of memory\r\n" );
        return;
    }

    chThdWait( tp );
}

void cmd_info( int argc, char * argv[] )
{
    (void)argv;
    if( argc > 0 )
    {
        usage( "info" );
        return;
    }
    chprint( "Kernel:       %s\r\n", CH_KERNEL_VERSION );
#ifdef CH_COMPILER_NAME
    chprint( "Compiler:     %s\r\n", CH_COMPILER_NAME );
#endif
    chprint( "Architecture: %s\r\n", PORT_ARCHITECTURE_NAME );
#ifdef CH_CORE_VARIANT_NAME
    chprint( "Core Variant: %s\r\n", CH_CORE_VARIANT_NAME );
#endif
#ifdef CH_PORT_INFO
    chprint( "Port Info:    %s\r\n", CH_PORT_INFO );
#endif
#ifdef PLATFORM_NAME
    chprint( "Platform:     %s\r\n", PLATFORM_NAME );
#endif
#ifdef BOARD_NAME
    chprint( "Board:        %s\r\n", BOARD_NAME );
#endif
#ifdef __DATE__
#ifdef __TIME__
    chprint( "Build time:   %s%s%s\r\n", __DATE__, " - ", __TIME__ );
#endif
#endif
}

static ShellCommand local_commands[] = {
    { "info",    cmd_info },
    { "mem",     cmd_mem },
    { "systime", cmd_systime },
    { "threads", cmd_threads },
    { "benchmark", cmd_benchmark },
    { NULL, NULL }
};

static void list_commands( const ShellCommand *scp )
{
    while( NULL != scp->sc_name )
    {
        chprint( KYEL "%s ", scp->sc_name );
        scp++;
    }
}

static int cmdexec( ShellCommand *scp,
                    char *name, int argc, char * argv[] )
{
    while( scp->sc_name != NULL )
    {
        if( strcasecmp( scp->sc_name, name ) == 0 )
        {
            scp->sc_function( argc, argv );
            return FALSE;
        }
        else if( strcasecmp( name, "help" ) == 0 )
        {
            if( argc > 0 )
            {
                usage( "help" );
                return FALSE;
            }
            list_commands( local_commands );
            chprint( "\r\n" );
            return FALSE;
        }
        scp++;
    }
    return TRUE;
}

static int exec( int argc, char ** argv )
{
    if( ( cmdexec( local_commands, argv[0],
                   argc - 1,
                   (char **)&argv[1] ) ) )
    {
        chprint( KRED "Command not found: %s\r\n",
                 argv[0] );
    }
    return 0;
}

void sigint( microrl_t * this )
{
    mlab_sigint( this );
}

#ifdef _USE_COMPLETE
//*****************************************************************************
// completion callback for microrl library
char ** complete( int argc, char ** argv )
{
    int j = 0;

    compl_world [0] = NULL;
    ShellCommand * scp;

    // if there is token in cmdline
    if( argc == 1 )
    {
        // get last entered token
        char * bit = (char *)argv[0];
        // iterate through our available token and match it
        scp = local_commands;
        for( j = 0; scp->sc_name != NULL; scp++ )
        {
            // if token is matched (text is part of our token starting from 0 char)
            if( strstr( scp->sc_name, bit ) == scp->sc_name )
            {
                // add it to completion set
                compl_world[j++] = (char *)scp->sc_name;
            }
        }
    }
    else
    {   // if there is no token in cmdline, just print all available token
        /* list system default commands */
        scp = local_commands;
        for( j = 0; scp->sc_name != NULL; scp++, j++ )
        {
            compl_world[j] = (char *)scp->sc_name;
        }
    }

    // note! last ptr in array always must be NULL!!!
    compl_world [j] = NULL;
    // return set of variants
    return compl_world;
}
#endif

void start_shell( void )
{
    uint8_t c;
    microrl_t rl;

    microrl_init( &rl, print );
    microrl_set_execute_callback( &rl, exec );
    microrl_set_complete_callback( &rl, complete );
    microrl_set_sigint_callback( &rl, sigint );

    while( true ) 
    {
        chSequentialStreamRead( (BaseSequentialStream *) &SDU2, &c, 1 );
        microrl_insert_char( &rl, c );  
    }
}
