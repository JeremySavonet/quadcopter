/*
 * Simple demo program.
 *
 * Description:
 * Basic hello world that blink a led and minimal CLI
 *
 * Author: Jeremy S.
 * Date: 2016-05-11
 * Version: V1.0
 */

#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "system.h"

#include "color.h"
#include "comm/debug_manager.h"
#include "comm/microshell.h"

/*===========================================================================*/
/* Defines                                                                   */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE( 2048 )

/*===========================================================================*/
/* Application threads                                                       */
/*===========================================================================*/

//Shell thread
static THD_WORKING_AREA( wa_cli, 2048 );
static THD_FUNCTION( CLI, arg )
{
    (void) arg;
    chRegSetThreadName( "CLI" );
    start_shell();
}

int main( void )
{
    static thread_t *shelltp = NULL;

    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();

    // Init system
    system_init();

    // Global main loop
    while( true )
    {
        if( !shelltp && ( SDU2.config->usbp->state == USB_ACTIVE ) )
        {
            shelltp = chThdCreateStatic( wa_cli,
                                         sizeof( wa_cli ),
                                         NORMALPRIO,
                                         CLI,
                                         NULL );
        }
        else if( chThdTerminatedX( shelltp ) )
        {
            chThdRelease( shelltp );  // Recovers memory of the previous shell.
            shelltp = NULL;           // Triggers spawning of a new shell.
        }

        chThdSleepMilliseconds( 100 ); // Iddle thread
    }
}
