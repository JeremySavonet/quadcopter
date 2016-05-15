/*
 * Debug manager module
 * Author: Jeremy S.
 */

#include "debug_manager.h"

// Struct to config serial module for debug
static SerialConfig uart_cfg =
{
    115200,
    0,
    0,
    0
};

void debug_manager_init( void )
{
    // Configure UART3 for debug 115200 8N1
    palSetPadMode( GPIOB, 10, PAL_MODE_ALTERNATE( 7 ) );
    palSetPadMode( GPIOB, 11, PAL_MODE_ALTERNATE( 7 ) );
    sdStart( &SD3, &uart_cfg );
}
