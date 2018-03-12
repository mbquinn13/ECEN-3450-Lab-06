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

// Auth:   Nicholas Wertzberger
// E-mail: wertnick@gmail.com
//
// Desc: Header file for the TI subsystem module which allows the CEENBoT to
//       be controlled with popular TI calculator models.
//
// NOTE: At this time, the library does NOT support multiple TI calculator
//       connections.
//
//
// Revision History:
// =================
//
//  Nov 5, 2010 - by J. Santos (principal maintainer):
//  -------------------------------------------------
//
//      * Several aspects of this source code have been enhanced, modified
//        or completely deleted.  Possibly the most significant, is the 
//        changes made to the source code to take advantage of the MOSFET
//        TI adapter boards for interfacing with the calculator.  These
//        MOSFET-based boards prevent the calculator from being loaded down
//        by the pull-down effect.
//
//        The MOSFET-based adapter boards use four wires instead of two
//        to perform communication and much of the changes in source code
//        were made to accomodate for this change.  (See schematic).
//
//        The large set of 'red-wire'/'white-wire' bit manipulating macros
//        are gone.  These have been replaced with a single '__GET_RED_WIRE()'
//        and '__GET_WHITE_WIRE()' set of macros to accomplish the same task
//        while relying on the standard CEENBoT-API helper macros 'SBV()'
//        and 'CBV()' to set/clear the bit values respectively.  These macro
//        definitions have been moved to '__ticmd324v221.h'.
//
//      * The command structure has been expanded.  Functions that implement
//        this command structure are declared in '__ticmd324v221.h'.  See this
//        header file for details.
//
//      * Previously this code did not support several TI calculator models
//        out of the box, but which were initially assumed to work (they 
//        didn't).  The source code has been modified and enhanced where 
//        needed to support (as in 'officially-tested' WITHOUT errors) the
//        following models, which were NOT supported out of the box.  These
//        include:  TI-83+ (and variants), TI-84+ (and variants), the TI-85
//        (the 'black sheep' of the TI models), and the TI-86.
//
//      * Intermittent behavior regarding usage of the TI-89, and TI-89 Plat-
//        inum series has now been fixed.  These calculators now work flaw-
//        lessly.
//
//      * Timing was a critical issue that required fixing.  At certain key
//        points during 'bit-transfer', the communication was occurring either
//        too slow, or too fast, with bits being changed too quickly without
//        respecting 'hold times' that are necessary before new bit changes
//        take effect.  Introduction of 'DELAY_us()' statements have corrected
//        these issues and is primarily the reason behind the success in 
//        using several of the models that would either not work at all, or 
//        would work stochastically -- these issues have now been corrected.
//
//      * At this point, the TI support is very 'robust' and solid as compared
//        with its initial inception and will be integrated into the 'main'
//        branch of the CEENBoT-API now that it has my 'seal of approval'.
//
//      

#ifndef __TI324V221_H__
#define __TI324V221_H__

#include<inttypes.h>
#include<avr/interrupt.h>
#include<avr/io.h>
#include<avr/pgmspace.h>

#include "isr324v221.h"
#include "sys324v221.h"
#include "utils324v221.h"
#include "uart324v221.h"

// ============================= defines ==================================== //
// NOTE:  Uncomment this for debugging.  This will enable UART messages to
//        be sent to the terminal via the UART device.
//
// #define __DEBUG_TI_UART0_MSG
// #define __DEBUG_TI_UART0_DUMP_DATA
// #define __DEBUG_TI_DUMP_TI_PARAMS_STRUCT

/* Define how the red wire (tip of jack) is connected to the device. */
#define TI_PCICR_MSK            0x01

#define TI_REDWIRE_PORT         PORTA
#define TI_REDWIRE_PIN          PINA
#define TI_REDWIRE_DDR          DDRA
#define TI_REDWIRE_PCMSK_REG    PCMSK0
#define TI_REDWIRE_BIT          6

/* Define how the white wire (middle of jack) is connected to the device. */
#define TI_WHITEWIRE_PORT       PORTA
#define TI_WHITEWIRE_PIN        PINA
#define TI_WHITEWIRE_DDR        DDRA
#define TI_WHITEWIRE_PCMSK_REG  PCMSK0
#define TI_WHITEWIRE_BIT        5

#define TI_PCMSK_REG            PCMSK0

// Desc: Alternate name(s) for the 'TI_complete_get_call()' function.
//       'r' is the 'value' (8-bits wide) to return to the calculator.
#define TI_Get_return( r )   TI_complete_get_call( r ) 
#define TI_return( r )       TI_complete_get_call( r )

// =========================== type declarations ============================ //

// Custom enumerated type declaration for specifying the 'port status'.
typedef enum {

    TI_OPEN_PORT,
    TI_ATTACHED_PORT

} TI_port_status_t;

typedef void ( *GET_CALLBACK  )( void );
typedef void ( *SEND_CALLBACK )( int16_t *list, uint8_t len );

// =========================== prototypes =================================== //
// Input Args: ((TODO: For now set both arguments to 'NULL'.))
//
// Returns: A value of type 'SUBSYS_STATUS', which will be one of the
//          following enumerated constants:
//
//              'SUBSYS_CLOSED' - Indicates the subsystm is closed.
//
//              'SUBSYS_OPEN'   - The subsystm is successfully open and can
//                                now be used.
//
//              'SUBSYS_ALREADY_OPEN' - Indicates the subsystem has already
//                                      been previously opened and initialized
//                                      by another client (subsystem).
//
//              'SUBSYS_IN_USE' - Indicates the subsystem [cannot be closed]
//                                because it is being used, or there is a
//                                dependency that needs to be met.
//
//              'SUBSYS_NOT_AVAILABLE' - Attempting to open a subsystem whose
//                                       resource is not available.
//
//              'SUBSYS_ERROR' - Generic unknown error.
//
//              'SUBSYS_INIT_FAILED' - It means things started out good, but
//                                     something happened along the way (i.e.,
//                                     hardware dependency not responding).
//
//              'SUBSYS_DEPENDENCY_ERROR' - Could not open because a dependency
//                                          (on another subsystem) could not be
//                                          met b/c it is not available or does
//                                          not exist.
//
//              'SUBSYS_DEPENDENCY_CONFLICT' - Could not open because an
//                                             UNSHARABLE resource needed by
//                                             the subsystem is already in use.
//
//          Generally speaking, users are normally interested (and should
//          just check) if 'SUBSYS_OPEN' or 'SUBSYS_ALREADY_OPEN' was returned
//          while all others should be treated as errors.
//
// Desc: Function acquires and initializes resources used by the TI subsystem
//       module.  Presently the arguments to 'get_function' and 'send_function'
//       should be set to 'NULL'.  This will cause 'TI_open()' to register
//       its own internal versions of the send/get functions.
//
//       Users should at least check to see if the function returns 
//       'SUBSYS_OPEN' or 'SUBSYS_ALREADY_OPEN' and proceed accordingly.  The
//       rest of the return status values should be treated as errors and
//       are mainly of interest during debugging, or for debugging purposes.
//
//       The typical 'open' sequence might look like:
//
//          SUBSYS_STATUS opstat;
//
//          // Attempt to open the subsystem in question.
//          opstat = XXXXX_open();
//
//          // Proceed only IF successfully opened.
//          if( ( opstat == SUBSYS_OPEN ) || ( opstat == SUBSYS_ALREADY_OPEN ) )
//          {
//                  /* Your code here */
//          }
//          else
//          {
//                  /* Spit out an error message, or somethin' */
//          }
//
//       It is up to the user to determine if only 'SUBSYS_OPEN' is necessary
//       and whether or not a 'SUBSYS_ALREADY_OPEN' is permissible.  If the
//       user requires exclusive use of a subsystem module, then he/she must
//       want to be the 'first' to open the device.  At this point, then, 
//       the user will want to make sure only 'SUBYS_OPEN' is returned and
//       treat everything else (even 'SUBSYS_ALREADY_OPEN') as an error.
//
extern SUBSYS_STATUS TI_open( GET_CALLBACK  get_function,
                              SEND_CALLBACK send_function );

// -------------------------------------------------------------------------- //
// TODO: (finish)
// Desc: Function deallocates and releases resources used by the TI subsystem
//       module.
extern void TI_close( void );

// -------------------------------------------------------------------------- //
// TODO: Since this is now accessed internally by 'TI_open()' it should be
//       moved to the private header file.
// TODO: (finish)
// Returns:  Returns an enumerated constant of type 'TI_port_status_t', 
//           with one of the following:
//
//              'TI_OPEN_PORT'     -- No calculator attached.
//              'TI_ATTACHED_PORT' -- If Calculator attached.
//
// Desc: Function is used to initialize the necessary interrupts and set the
//       necessary variables for this framework.  This must be called first.
extern TI_port_status_t TI_init( void );

// -------------------------------------------------------------------------- //
// TODO: (finish)
// Desc: This function should be called inside of a while loop so that incoming
//       commands can be processed when the arrive from the calculator.  For
//       example, somewhere in 'CBOT_main()' you should have:
//
//          while( 1 ) {
//          
//              TI_process_commands();
//
//          }
//
//      This function checks the internal buffer if new commands have arrived.
//      If so, the appropriate function will be dispatched for execution.  
//      Otherwise, it does nothing.  This procedure is MANDATORY if the user
//      expects the TI-module to work as expected.
extern void TI_process_commands( void ); 

// -------------------------------------------------------------------------- //
// TODO: Since this is now accessed internally by 'TI_open()' it should be
//       moved to the private header file.
// TODO: (finish)
// Desc: Set the callback function for the calculator's 'Send'.
extern void TI_set_send_callback( 
                        void ( *callback )( int16_t *list, uint8_t len ) );

// -------------------------------------------------------------------------- //
// TODO: Since this is now accessed internally by 'TI_open()' it should be
//       moved to the private header file.
// TODO: (finish)
// Desc: Set the callback function for the calculator's 'Get'.
//
// Note: This will not complete the 'Get' command until 'TI_complete_get_call()'
//       is invoked.  You can set state variables for some other code in here,
//       leave quickly, and call 'TI_complete_get_call()' after, but because
//       this ISR releases its pins for interrupts and then enables interrupts,
//       it's OK if you take your time here and call 'TI_complete_get_call()'
//       at the end of the function.  Whatever is your style.
extern void TI_set_get_callback( 
                        void ( *callback )( void ) );

// -------------------------------------------------------------------------- //
// TODO: (finish)
// Desc: Function is used to complete a 'Get' call, and gives the value in 
//       'var' to the attached calculator.  You MUST make sure that you have
//       previously set up the callback function via 'TI_set_get_callback()'
//       prior to invoking of this function.
extern void TI_complete_get_call( int16_t var );

// -------------------------------------------------------------------------- //
// TODO: Since this is now accessed internally by 'TI_open()' it should be
//       moved to the private header file.
// TODO: (finish)
// Desc: ISR function which gets registered with the PIN-change interrupt.
//       NOT meant to be directly called by the user.  It is internally associ-
//       ated with 'PCINT0_vect' ISR.
extern CBOT_ISR( TI_isr );

// =========================== external declarations ======================== //
#ifdef __DEBUG_TI_UART0_MSG

    extern unsigned char ti_data_buffer[ 128 ];

#endif

#endif /* __TI324V221_H__ */
