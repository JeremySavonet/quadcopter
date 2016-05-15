/*
 * Manage board structure here
 *
 * Author: Jeremy S.
 * Date: 2016-05-11
 * Version: V1.0
 */

#include "system.h"
#include "color.h"

#include "comm/debug_manager.h"
#include "comm/usb_manager.h"

// Green LED blinker thread, times are in milliseconds.
static THD_WORKING_AREA( wa_alive, 128 );
static THD_FUNCTION( Alive, arg )
{
    (void)arg;
    chRegSetThreadName( "alive" );
    while( true )
    {
        palTogglePad( GPIOC, GPIOC_LED );
        chThdSleepMilliseconds( 500 );
    }
}

void system_print_boot_msg( void )
{
    //Display boot sys info:
    DPRINT( 1, KNRM "__   __                          _     _   _          ___    ___ \r\n" );
    DPRINT( 1, KNRM "\\ \\ / /  ___   _ _   ___  __ _  | |_  (_) | |  ___   / _ \\  / __|\r\n" );
    DPRINT( 1, KNRM " \\ V /  / -_) | '_| (_-< / _` | |  _| | | | | / -_) | (_) | \\__ \\\r\n" );
    DPRINT( 1, KNRM "  \\_/   \\___| |_|   /__/ \\__,_|  \\__| |_| |_| \\___|  \\___/  |___/\r\n" );

    DPRINT( 1, KGRN "Kernel:       %s\r\n", CH_KERNEL_VERSION );
    #ifdef CH_COMPILER_NAME
        DPRINT( 1, KGRN "Compiler:     %s\r\n", CH_COMPILER_NAME );
    #endif
     DPRINT( 1, KGRN "Architecture: %s\r\n", PORT_ARCHITECTURE_NAME );
    #ifdef CH_CORE_VARIANT_NAME
        DPRINT( 1, KGRN "Core Variant: %s\r\n", CH_CORE_VARIANT_NAME );
    #endif
    #ifdef CH_PORT_INFO
        DPRINT( 1, KGRN "Port Info:    %s\r\n", CH_PORT_INFO );
    #endif
    #ifdef PLATFORM_NAME
        DPRINT( 1, KGRN "Platform:     %s\r\n", PLATFORM_NAME );
    #endif
    #ifdef BOARD_NAME
        DPRINT( 1, KGRN "Board:        %s\r\n", BOARD_NAME );
    #endif
    #ifdef __DATE__
    #ifdef __TIME__
        DPRINT( 1, KGRN "Build time:   %s%s%s\r\n", __DATE__, " - ", __TIME__ );
    #endif
    #endif

    // Set color cursor to normal
    DPRINT( 1, KNRM "" );
}

// Init the system and all peripherals
void system_init( void )
{
    // Inits IOs
    palSetPadMode( GPIOC, GPIOC_LED, PAL_MODE_OUTPUT_PUSHPULL );

    // Inits debug
    debug_manager_init();

    // Welcome the user
    system_print_boot_msg();

    // Inits USB CLI
    usb_manager_init();
    DPRINT( 1, "[*] usb OTG system ready\r\n" );

    // Creates the blinker thread.
    chThdCreateStatic( wa_alive,
                       sizeof( wa_alive ),
                       NORMALPRIO,
                       Alive,
                       NULL );

    DPRINT( 1, "Welcome bitchessss !!!\r\n" );
}
