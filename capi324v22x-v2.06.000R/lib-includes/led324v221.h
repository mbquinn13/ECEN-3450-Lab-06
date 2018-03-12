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
// Desc: Header file for the 'led' services module.

#ifndef __LED324V221_H__
#define __LED324V221_H__

#define __LED324MODULE__

#include<avr/io.h>

#include "sys324v221.h"
#include "utils324v221.h"

// ============================== defines =================================== //
// Macro-constants used with 'LED_state()' and 'LED_toggle()' macros (below).
#define LED0         5
#define LED1         6
#define LED_Red      LED0
#define LED_Green    LED1
#define LED_RED      LED0
#define LED_GREEN    LED1

#define LED_ON      SET
#define LED_OFF     CLEAR

// Helper macros for single-bit LED handling.
#define LED_set( which )        LED_set_pattern( ( 1 << ( which ) ) )
#define LED_clr( which )        LED_clr_pattern( ( 1 << ( which ) ) )
#define LED_toggle( which )     LED_tog_pattern( ( 1 << ( which ) ) )
#define LED_state( which, state ) \
    ( state ? ( LED_set( ( which ) ) ) : ( LED_clr( ( which ) ) ) )


// ============================ prototypes ================================== //
// Input Args: None.
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
// Desc: Function initializes the hardware ports for manipulating the LEDs.
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
extern SUBSYS_STATUS LED_open( void );
// -------------------------------------------------------------------------- //
// Input  Args: TODO
// Output Args:
// Globals  Read:
// Globals Write:
// Returns:
//
// Desc: Call this function if LED resources are no longer needed.  This opens
//       up port pins for other use.
extern void LED_close( void );
// -------------------------------------------------------------------------- //
// Input  Args: 'LED_pattern' - A '1' for a corresponding bit turns the LED
//                              on, while a '0' does nothing (i.e., it leaves
//                              the LED in the current state).  The LED
//                              assigned bit patterns are as follows:
//
//                                  bit5: Red   LED on PD5.
//                                  bit6: Green LED on PD6.
//
//                                  All others: Not-assigned (future expansion).
// Output Args: None.                                  
// Globals  Read: None.
// Globals Write: None.
// Returns: Nothing.
//
// Desc: User function to set LED pattern.
extern void LED_set_pattern( unsigned char LED_pattern );
// -------------------------------------------------------------------------- //
// Input  Args: 'LED_pattern' - A '1' for a corresponding bit turns the LED
//                              OFF, while a '0' does nothing (i.e., it leaves
//                              the LED in the current state).  The LED
//                              assigned bit patterns are as follows:
//
//                                  bit5: Red   LED on PD5.
//                                  bit6: Green LED on PD6.
//
//                                  All others: Not-assigned (future expansion).
// Output Args: None.                                  
// Globals  Read: None.
// Globals Write: None.
// Returns: Nothing.
//
// Desc: User function to clear the LED pattern.
extern void LED_clr_pattern( unsigned char LED_pattern );
// -------------------------------------------------------------------------- //
// Input  Args: 'LED_pattern' - A '1' for a corresponding bit will toggle the
//                              current state of the LED from ON to OFF, or
//                              from OFF to ON, etc., while a '0' does nothing 
//                              (i.e., it leaves the LED in the current state).
//                              The LED assigned bit patterns are as follows:
//
//                                  bit5: Red   LED on PD5.
//                                  bit6: Green LED on PD6.
//
//                                  All others: Not-assigned (future expansion).
// Output Args: None.                                  
// Globals  Read: None.
// Globals Write: None.
// Returns: Nothing.
//
// Desc: User function to clear the LED pattern.
extern void LED_tog_pattern( unsigned char LED_pattern );
// -------------------------------------------------------------------------- //

#endif /* __LED324V221_H__ */
