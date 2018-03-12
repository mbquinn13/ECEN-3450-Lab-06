/*
 *  This source code is part of the CEENBoT-API.
 *
 *  Copyright (C) 2011-2017 - University of Nebraska Board of Regents.
 *  Written and Created by: Jose Santos <ceenbot.api@digital-brain.info>
 *
 *  The CEENBoT-API is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Auth: Jose Santos
// Desc: Header file for the UART subsystem module.

#ifndef __UART324V221_H__
#define __UART324V221_H__

#include <stdio.h>
#include <stdarg.h>
#include <avr/io.h>
#include "sys324v221.h"
#include "utils324v221.h"
#include "tmrsrvc324v221.h"

// ============================== defines =================================== //
#define  UART_TIMEOUT       TIMER16

#define  UART_MAX_TIMEOUT   30  /* Specify maximum timeout in seconds. */

// Helper macros that target a specific UART device.
#ifndef __NO_UART_PRINTF

    #define UART0_printf( ... )     UART_printf( UART_UART0,   __VA_ARGS__  )
    #define UART0_transmit( ... )   UART_transmit( UART_UART0, __VA_ARGS__ )
    #define UART0_receive( ... )    UART_receive( UART_UART0,  __VA_ARGS__  )
    #define UART0_has_data()        UART_has_data( UART_UART0 )

    #define UART1_printf( ... )     UART_printf( UART_UART1,   __VA_ARGS__  )
    #define UART1_transmit( ... )   UART_transmit( UART_UART1, __VA_ARGS__ )
    #define UART1_receive( ... )    UART_receive( UART_UART1,  __VA_ARGS__  )
    #define UART1_has_data()        UART_has_data( UART_UART1 )

    #define UART0_printf_PGM( ... ) UART_printf_PGM( UART_UART0, __VA_ARGS__ )
    #define UART1_printf_PGM( ... ) UART_printf_PGM( UART_UART1, __VA_ARGS__ )

#else

    #define UART0_printf( ... )     UART_COMM_OK
    #define UART0_transmit( ... )   UART_COMM_OK
    #define UART0_receive( ... )    UART_COMM_OK
    #define UART0_has_data()        FALSE

    #define UART1_printf( ... )     UART_COMM_OK
    #define UART1_transmit( ... )   UART_COMM_OK
    #define UART1_receive( ... )    UART_COMM_OK
    #define UART1_has_data()        FALSE

    #define UART0_printf_PGM( ... )
    #define UART1_printf_PGM( ... )
    
#endif /* __NO_UART_PRINTF */

// ========================== type declarations ============================= //
// Enumerated type declaration to specify which MCU UART device to open up
// for use.
typedef enum UART_ID_TYPE {

    UART_UART0,
    UART_UART1

} UART_ID;

// Enumerated type declaration for specifying whether a UART is ENABLED or
// DISABLED.
typedef enum UART_STATE_TYPE {

    UART_DISABLE,
    UART_ENABLE

} UART_STATE;

// Type declaration for specifying the baud-rate.
typedef unsigned long int UART_BAUD;

// Enumerated type declaration for specifying the number of data-bits 
// (character size)
// for UART transmission/reception.
typedef enum UART_DBITS_TYPE {

    UART_5DBITS = 0,
    UART_6DBITS,
    UART_7DBITS,
    UART_8DBITS
    // UART_9BITS // NOTE: Presently this mode is NOT supported.

} UART_DBITS;

// Enumerated type declaration for specifying the number of stop bits
// to use for UART communications.
typedef enum UART_SBITS_TYPE {

    UART_1SBIT  = 0,
    UART_2SBITS

} UART_SBITS;

// Enumerated type declaration for specifying the parity mode for UART
// communication.
typedef enum UART_PARITY_TYPE {

    UART_NO_PARITY = 0,
    UART_EVEN_PARITY = 2,
    UART_ODD_PARITY  = 3

} UART_PARITY;

// Enumerated type declarationfor specifying the status of a 
// transmit or receive sequence.
typedef enum UART_COMM_RESULT_TYPE {

    UART_COMM_OK,       // Communication sequence OK.
    UART_COMM_ERROR,    // Unknown communication ERROR.
    UART_COMM_TX_FULL,  // Transmitter is not ready to accept new data.
    UART_COMM_TIMEOUT   // Communication timeout while either sending or
                        // receiving.

} UART_COMM_RESULT;

// Structure type declaration which holds internal state and other info
// for each of the UART devices.
typedef struct UART_PARAMS_TYPE {

    // Parameters applicable to UART0 only.
    struct {

        BOOL TX_enabled;
        BOOL RX_enabled;
        BOOL configured;
        
        UART_TIMEOUT timeout_sec;

    } uart0;

    // Parameters applicable to UART1 only.
    struct {

        BOOL TX_enabled;
        BOOL RX_enabled;
        BOOL configured;

        UART_TIMEOUT timeout_sec;

    } uart1;

} UART_PARAMS;

// ============================= prototypes ================================= //
// Input  Args: 'which' - Specifies which UART device.  Must be one of the 
//                        following enumerated constants:
//
//                          'UART_UART0' - UART0 Device.
//                          'UART_UART1' - UART1 Device.
//
// Returns: A value of type 'SUBSYS_STATUS', which will be one of the 
//          following enumerated constants:
//
//           'SUBSYS_CLOSED' - Indicates the subsystem is closed.
//
//           'SUBSYS_OPEN'   - The subsystem is successfully open.
//
//           'SUBSYS_ALREADY_OPEN' - Indicates is already open (and initialized)
//                                   by another client (subsystem).
//
//           'SUBSYS_IN_USE' - Indicates the subsystem [cannot be closed]
//                             because it is being used, or there is a 
//                             dependency that needs to be met.
//
//           'SUBSYS_NOT_AVAILABLE' - Attempting to open a subsystem whose
//                                    resource is not available.
//
//           'SUBSYS_ERROR' - Generic unknown error.
//
//           'SUBSYS_INIT_FAILED' - It means things started out good, but
//                                  something happened along the way (i.e.,
//                                  hardware dependency not responding).
//
//           'SUBSYS_DEPENDENCY_ERROR' - Could not open because a dependency
//                                       (on another subsystem) could not be
//                                       met b/c it is not availble or does
//                                       not exist.
//
//           'SUBSYS_DEPENDENCY_CONFLICT' - Could not open because an UNSHARABLE
//                                          resource needed by the subsystem is
//                                          already in use.
//
//          Generally speaking, users are normally interested and should check
//          if 'SUBSYS_OPEN' or 'SUBSYS_ALREADY_OPEN' was returned, while
//          all others should be treated as errors.
//
// Desc: Function opens and initializes the specified UART subsystem module.
//       Users should at least check to see if function returns 'SUBSYS_OPEN'
//       or 'SUBSYS_ALREADY_OPEN' and proceed accordingly.  The rest of these
//       errors are mostly for debugging purpses.  Thus, a typical 'open' 
//       sequence might go something like:
//
//          SUBSYS_STATUS opstat;
//
//          // Atttempt to open.
//          opstat = XXXXX_open();
//
//          // Proceed if successful.
//          if( ( opstat == SUBSYS_OPEN ) || ( opstat == SUBSYS_ALREADY_OPEN ) )
//          {
//                  /* ... Get to work! ... */
//          }
//          else
//          {
//                  /* ... Spit error message, etc. ... */
//          }
//
//       It is up to the user to determine if only 'SUBSYS_OPEN' is necessary
//       and whether or not a 'SUBSYS_ALREADY_OPEN' is permissible.  If the
//       user requires exclusive use of a subsystem module, then he/she must
//       want to be the 'first' to open the device.  At this point, then, 
//       the user will want to make sure only 'SUBYS_OPEN' is returned and
//       treat everything else (even 'SUBSYS_ALREADY_OPEN') as an error.
//
extern SUBSYS_STATUS UART_open( UART_ID which );
// -------------------------------------------------------------------------- //
// Input  Args: 'which' - Specifies which UART device.  Must be one of the 
//                        following enumerated constants:
//
//                          'UART_UART0' - UART0 Device.
//                          'UART_UART1' - UART1 Device.
//
// Returns: Nothing.
//
// DESC: Function closes and releases the specified UART subsystem module.
extern void UART_close( UART_ID which );
// -------------------------------------------------------------------------- //
// Input  Args: 'which' - Specifies which UART device.  Must be one of the 
//                        following enumerated constants:
//
//                          'UART_UART0' - UART0 Device.
//                          'UART_UART1' - UART1 Device.
//
//              'uart_state' - Must be one of the following enumerated
//                             constants:
//
//                           'UART_ENABLE'  - To enable the state.
//                           'UART_DISABLE' - To disable the state.
//
// Returns: Nothing.
//
// Desc: Function enables/disables the specified UART transmitter.  When 
//       the UART is ENABLED, the corresponding I/O communication pins are 
//       overriden and taken up by the UART device.
extern void UART_set_TX_state( UART_ID which, UART_STATE uart_state );
// -------------------------------------------------------------------------- //
// Input  Args: 'which' - Specifies which UART device.  Must be one of the 
//                        following enumerated constants:
//
//                          'UART_UART0' - UART0 Device.
//                          'UART_UART1' - UART1 Device.
//
//              'uart_state' - Must be one of the following enumerated
//                             constants:
//
//                           'UART_ENABLE'  - To enable the state.
//                           'UART_DISABLE' - To disable the state.
//
// Returns: Nothing.
//
// Desc: Function enables/disables the specified UART receiver.  When 
//       the UART is ENABLED, the corresponding I/O communication pins are
//       overriden and taken up by the UART device.
extern void UART_set_RX_state( UART_ID which, UART_STATE uart_state );
// -------------------------------------------------------------------------- //
// Input  Args: 'which' - Specifies which UART device.  Must be one of the 
//                        following enumerated constants:
//
//                          'UART_UART0' - UART0 Device.
//                          'UART_UART1' - UART1 Device.
//
//              'data_bits' - Specifies the 'character' size to be transmitted.
//                            It must be one of the following enumerated const-
//                            ants:
//
//                              'UART_5DBITS' - 5-Bits character size.
//                              'UART_6DBITS' - 6-Bits character size.
//                              'UART_7DBITS' - 7-Bits character size.
//                              'UART_8DBITS' - 8-Bits character size.
//
//              'stop_bits' - Specifies the number of 'stop bits' to use.  It
//                            must be one of the following enumerated constants:
//
//                              'UART_1SBIT'  - 1 Stop bit.
//                              'UART_2SBITS' - 2 Stop bits.
//
//              'parity' - Specifies the parity mode to use.  It must be one of
//                         the following enumerated constants:
//
//                              'UART_NO_PARITY'  - No Parity.
//                              'UART_EVEN_PARITY'- Use EVEN parity.
//                              'UART_ODD_PARITY' - Use ODD parity.
//
//              'baud_rate' - The baud rate in bits-per-second (bps).  The
//                            maximum baud rate is determined by the UART
//                            device.  The maximum theoretical speed is
//                            approximately 1.2Mbps.
//
// Returns: Nothing.
//
// Desc: Function *MUST* be called after 'UART_open()' to configure the
//       UART device with the appropriate settings.
extern void UART_configure( UART_ID     which,
                            UART_DBITS  data_bits,
                            UART_SBITS  stop_bits,
                            UART_PARITY parity,
                            UART_BAUD   baud_rate );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Specifies which UART device.  Must be one of the
//                       following enumerated constants:
//
//                          'UART_UART0' - UART0 Device.
//                          'UART_UART1' - UART1 Device.
//
//             'timeout_sec' - An integer representing the time in seconds.
//
// Desc: This function sets the 'time-out' that functions that read data from
//       the UART will wait for data reception.  After the timeout time
//       has elapsed, waiting for incoming data will be canceled and the
//       user will be notified of a time-out error.  Note that the timeout
//       is specified in units of 'seconds'.  The maximum timeout is 30.
//       The default timeout when the 'UART_open()' function is invoked is
//       5 seconds.
extern void UART_set_timeout( UART_ID which, UART_TIMEOUT timeout_sec );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Specifies which UART device.  Must be one of the
//                       following enumerated constants:
//
//                          'UART_UART0' - UART0 Device.
//                          'UART_UART1' - UART1 Device.
//
//              'data' - The data byte to send.
//
// Returns: Function returns a value of type 'UART_COMM_RESULT', which
//          can be equal to one of the following enumerated constants:
//
//              'UART_COMM_OK'      - Communication sequence OK.
//              'UART_COMM_ERROR'   - Unknown communication ERROR.
//              'UART_COMM_TX_FULL' - Transmitter is not ready to accept
//                                    new data.
//              'UART_COMM_TIMEOUT' - Communication timed out either while
//                                    sending or receiving.
//
// Desc: This function allows the user to transmit a single byte of data
//       via the specified UART device.
extern UART_COMM_RESULT UART_transmit( UART_ID which, unsigned char data );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Specifies which UART device.  Must be one of the
//                       following enumerated constants:
//
//                          'UART_UART0' - UART0 Device.
//                          'UART_UART1' - UART1 Device.
//
//              'pDest' - Pass to this argument _the address_ of a varible
//                        where the received byte will be stored.
//
// Output Args: 'pDest' - Received data will be written to the -address-
//                        'pointed to' by this variable.
//
// Returns: Function returns a value of type 'UART_COMM_RESULT', which
//          can be equal to one of the following enumerated constants:
//
//              'UART_COMM_OK'      - Communication sequence OK.
//              'UART_COMM_ERROR'   - Unknown communication ERROR.
//              'UART_COMM_TX_FULL' - Transmitter is not ready to accept
//                                    new data.
//              'UART_COMM_TIMEOUT' - Communication timed out either while
//                                    sending or receiving.
//
// Desc: This function allows the user to receive a single byte of data
//       via the specified UART device.  The received data will be placed
//       in a destination variable whose **address of** is passed as an
//       argument to 'pDest'.
extern UART_COMM_RESULT UART_receive( UART_ID which, unsigned char *pDest );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Specifies which UART device.  Must be one of the
//                       following enumerated constants:
//
//                          'UART_UART0' - UART0 Device.
//                          'UART_UART1' - UART1 Device.
//
// Returns: Nothing.
//
// Desc: Use this function to determine if there is data to be read which has
//       been received.
extern BOOL UART_has_data( UART_ID which );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Specifies which UART device.  Must be one of the
//                       following enumerated constants:
//
//                          'UART_UART0' - UART0 Device.
//                          'UART_UART1' - UART1 Device.
//
//              'str_fmt' - The usual 'printf-style' string formatting.  Ensure
//                          this string is placed inside PSTR() macro. See
//                          description below.
//
//              '...' - Additional arguments related to 'str_fmt' argument.
//
// Returns: Nothing.
//
// Desc: The name says it all... 'printf()' facility via UART.  Useful when
//       working with a 'terminal' such as 'Br@y'.
extern void UART_printf( UART_ID which, const char *str_fmt, ... );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Specifies which UART device.  Must be one of the
//                       following enumerated constants:
//
//                          'UART_UART0' - UART0 Device.
//                          'UART_UART1' - UART1 Device.
//
//              'str_fmt' - The usual 'printf-style' string formatting.  Ensure
//                          this string is placed inside PSTR() macro. See
//                          description below.
//
//              '...' - Additional arguments related to 'str_fmt' argument.
//
// Returns: Nothing.
//
// Desc: This function works the same way as 'UART_printf()' except string
//       data is placed in FLASH instead of SRAM since strings eat up the
//       SRAM VERY QUICKLY... YIKES!  When invoking this function, or the
//       macro versions, you MUST use the 'PSTR()' macro for EVERY string
//       you intend to print via this function -- otherwise, you will not
//       reak in the benefit of storing the string data in program memory.
//
//       For example:
//
//              UART_printf_PGM( UART_UART0, PSTR( "Hello World!" ) );
//
//       Here the 'PSTR()' macro ensures the string gets placed in program mem-
//       mory and not SRAM.
//
//       Here's another example:
//
//              unsigned int val = 22;
//
//              UART_printf_PGM( UART_UART0, PSTR( "Value is = %d.\n" ), val );
//
//       Again, note usage of the 'PSTR()' macro -- it is REQUIRED!
extern void UART_printf_PGM( UART_ID which, const char *str_fmt, ... );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Specifies which UART device.  Must be one of the
//                       following enumerated constants:
//
//                          'UART_UART0' - UART0 Device.
//                          'UART_UART1' - UART1 Device.
// Returns: Nothing.
//
// Desc: Function is used to enable reception interrupts which is
//       usually the case when UART interrupts are enabled/disabled.
extern void UART_enable_RX_interrupt( UART_ID which );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Specifies which UART device.  Must be one of the
//                       following enumerated constants:
//
//                          'UART_UART0' - UART0 Device.
//                          'UART_UART1' - UART1 Device.
// Returns: Nothing.
//
// Desc: Function is used to disable reception interrupts which is usually
//       the case when UART interrupts are enabled/disabled.
extern void UART_disable_RX_interrupt( UART_ID which );
// -------------------------------------------------------------------------- //
// ========================== external declarations ========================= //
extern UART_PARAMS UART_params;
extern TIMEROBJ    UART0_timeout_timer;
extern TIMEROBJ    UART1_timeout_timer;
extern FILE UART0_stdout;
extern FILE UART1_stdout;

#endif /* __UART324V221_H__ */
