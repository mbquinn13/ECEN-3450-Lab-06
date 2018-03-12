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
// Desc: Header file for the 'system' module.  This module provides resources
//       and access to system-wide variables.  Specifically, 'accessor 
//       functions' to system variables to other subsystem modules.

#ifndef __SYS324V221_H__
#define __SYS324V221_H__

#include "utils324v221.h"

// =========================== global declarations ========================== //
// Desc: Enumerated type declaration for specifying the return value of a 
//       subsystem status when it is opened/closed, etc.
typedef enum SUBSYS_STATUS_TYPE 
{

    SUBSYS_CLOSED = 0,      // Indicates the subsystem is closed.
    SUBSYS_OPEN,            // Indicates the subsystem is open.
    SUBSYS_ALREADY_OPEN,    // Indicates the subsystem is already open.
    SUBSYS_IN_USE,          // Indicates the subsystem [cannot be closed]
                            //  because it is being used, or there is a
                            //  dependency.

    SUBSYS_NOT_AVAILABLE,   // Attempting to open a subsystem whose
                            //  resource is not available.

    SUBSYS_ERROR,           // Generic subsystem error.
    SUBSYS_INIT_FAILED,     // Initialization failed.
    SUBSYS_DEPENDENCY_ERROR,    // Could not open because a dependency could
                                //  by the subsystem could not be met.
                                
    SUBSYS_DEPENDENCY_CONFLICT  // Could not open because an UNSHARABLE
                                //  resources needed by the subsystem is
                                //  already in use.

} SUBSYS_STATUS;

// Desc: Enumerated type declaration for specifying various subsystems.
#define SUBSYS_COUNT   21   /* Number of subsystems in the below enumeration
                               -not- including 'SUBSYS_NA'. */
typedef enum SUBSYS_TYPE
{

    SUBSYS_CPU0 = 0,    // ATmega324.
    SUBSYS_CPU1,        // ATtiny.
    SUBSYS_SPI,         // SPI subsystem.
    SUBSYS_SPIFLASH,    // SPIFLASH subsystem.
    SUBSYS_PSXC,        // PSX controller subsystem.
    SUBSYS_LCD,         // LCD subsystem.
    SUBSYS_LED,         // LED subsystem.
    SUBSYS_STEPPER,     // STEPPER subsystem.
    SUBSYS_TMRSRVC,     // Timer Service subsystem.
    SUBSYS_SPKR,        // Speaker Service subsystem.
    SUBSYS_BEEP,        // Beeper Service subsystem.
    SUBSYS_SWATCH,      // Stopwatch subsystem.
    SUBSYS_USONIC,      // Ultrasonic device subsystem.
    SUBSYS_UART0,       // UART0 subsystem.
    SUBSYS_UART1,       // UART1 subsystem.
    SUBSYS_ADC,         // ADC subsystem.
    SUBSYS_ISR,         // ISR subsystem.
    SUBSYS_TI,          // TI subsystem.
    SUBSYS_I2C,         // I2C subsystem.
    SUBSYS_CMUCAM,      // CMUCAM (CMUcam4) subsystem.
    SUBSYS_PIXY,        // PIXY (CMUcam5) subsystem.
    SUBSYS_NA

} SUBSYS;

// Desc: This type declaration is for specifying the type of a reference
//       count for each subsystem.
typedef unsigned char SUBSYS_REFCOUNT;

// =========================== prototypes =================================== //
// Input  Args: None.
// Output Args: None.
// Returns: Nothing.
// Desc: Initializes the internal state of the system-wide state variables.
extern void SYS_init( void );
// -------------------------------------------------------------------------- //
// Input  Args: None.
// Output Args: None.
// Returns: Returns a value of type 'SUBSYS_REFCOUNT'.  If 0, the subsystem
//          is closed, a non-zero value means the subsystem is open by
//          a number of devices given by the integer value.
//              
// Desc: Function used to get the subsystem state of a given subsystem.
extern SUBSYS_REFCOUNT SYS_get_refcount( SUBSYS which );
// -------------------------------------------------------------------------- //
// Input  Args: 'which' - Specify which subsystem.  See the 'SUBSYS' structure
//                        declaration to find what values are valid.
//
//              'count_val' - The reference count value.
//
// Output Args: None.
// Returns: Nothing.
// Desc: Function used to set the subsystem state.
extern void SYS_set_refcount( SUBSYS which, SUBSYS_REFCOUNT count_val );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Specify which susbsystem.  See the 'SUBSYS' structure
//                       declaration to find what values are valud.
// Output Args: None.
// Returns: Returns a value of type 'SUBSYS_REFCOUNT' corresponding to the
//          current reference count -BEFORE- it has been incremented.  If '0',
//          the subsystem in question needs to be open.
//
// Desc: Function is used to increment the reference count for a subsystem.
extern SUBSYS_REFCOUNT SYS_increment_refcount( SUBSYS which );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Specify which susbsystem.  See the 'SUBSYS' structure
//                       declaration to find what values are valud.
// Output Args: None.
// Returns: Returns a value of type 'SUBSYS_REFCOUNT' corresponding to the
//          reference count -AFTER- it has been decremented.  If '0' the subsy-
//          stem in question should be closed.
//
// Desc: Function is used to decrement the reference count for a subsystem.
extern SUBSYS_REFCOUNT SYS_decrement_refcount( SUBSYS which );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Specify which susbsystem.  See the 'SUBSYS' structure
//                       declaration to find what values are valud.
// Output Args: None.
// Returns: Returns a value of type 'BOOL' which is 'TRUE' if the subsystem
//          in question is open.  'Open' means the refcount of the subsystem
//          is not zero.
extern BOOL SYS_isOpen( SUBSYS which );

// ======================== external globals ================================ //
// Array which holds the subsystem state of each subsystem.  For each subsystem
// there is a resource count.  Each 'open' increments the corresponding value,
// and each 'close' decrements it.  Only when the reference count is '0', will
// the corresponding subsystem close for good (until re-opened again).
extern volatile SUBSYS_REFCOUNT CBOT_SYSTEM_refcount[ SUBSYS_COUNT ];


#endif /* __SYS324V221_H__ */
