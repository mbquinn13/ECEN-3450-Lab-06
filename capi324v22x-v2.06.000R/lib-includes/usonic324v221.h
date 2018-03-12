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
// Desc: Header file for using the 'Ultrasonic Module'.

#ifndef __USONIC324V221_H__
#define __USONIC324V221_H__

#include "sys324v221.h"
#include "utils324v221.h"
#include "swatch324v221.h"
#include "tmrsrvc324v221.h"
#include "isr324v221.h"
#include "tiny324v221.h"

// ============================== defines =================================== //
// Desc: Helper macro to convert ticks to centimeters.  The result is
//       a value of type `float'.
#define USONIC_DIST_CM( ticks ) \
    0.01724f * (10UL * ((unsigned long int)(ticks)))

#define USONIC_TICKS_TO_CM( ticks ) USONIC_DIST_CM( ticks )

// Desc: Helper macro to convert ticks to inches.  The result is
//       a value of type `float'.
#define USONIC_DIST_INCHES( ticks ) \
   0.00669862f * (10UL * ((unsigned long int)(ticks)))

#define USONIC_TICKS_TO_INCHES( ticks ) USONIC_DIST_INCHES( ticks )

    
// ========================== type declarations ============================= //
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
// Desc: Fuction initializes and acquires resources to use the onboard ultra-
//       sonic module (when attached).  Because the USONIC subsystem module
//       has a dependency on the STOPWATCH subsystem module it will attempt
//       to acquire exclusive use.  If this subsystem module (the STOPWATCH)
//       is already in use an error (most likely a 'SUBSYS_DEPENDENCY_ERROR')
//       will be returned to the user.  The user does NOT have to manually
//       invoke 'STOPWATCH_open()' as that is automatically done by this
//       subsystem module.
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
extern SUBSYS_STATUS USONIC_open( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Returns: Nothing.
// Desc: Function releases resources acquired for use by the Ultrasonic module.
extern void USONIC_close( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Returns: Returns a value of type 'SWTIME', which is a type definition
//          on a 'unsigned short int' (16-bits).  Each integer value represents
//          an elapsed duration of 10us (microseconds).  So, say, a value of
//          257 corresponds to 257*10us = 2.57ms (milliseconds).
//
// Desc: Function triggers the attached ultrasonic sensor to emit a 'ping'.  The
//       function returns the number 'ticks' it took the 'ping' sound wave to
//       echo back after it is emitted by the ultrasonic sensor.  That is the
//       number of ticks measures the ROUND TRIP of this sound wave.
//       The value of each 'tick' depends on the granularity of the stopwatch 
//       service.  At the time this function is being written, each stopwatch
//       'tick' corresponds to 10us/tick.  So a value of, say 257 corresponds
//       a sound round-trip travel time of 257*10us = 2.57ms.  This value can
//       then be used to determine distance to object/feature.
//
//       The function will 'busy-wait' in a series of loops until the pulse
//       returns from the echo.  As a result, this function will _block_ until
//       the echo returns.  If no device is attached when a 'ping' is issued,
//       the program will freeze because it is waiting for an 'echo' that will
//       never return, given one never left.
extern SWTIME USONIC_ping( void );

// ========================= external declarations ========================== //

// Nothing yet.

#endif /* __USONIC324V221_H__ */
