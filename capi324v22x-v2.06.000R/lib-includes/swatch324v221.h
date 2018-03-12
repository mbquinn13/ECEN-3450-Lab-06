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
// Desc: Header file for 'stopwatch' services.  The stopwatch module provides
//       10us accurate timing services for measuring events to a time granu-
//       larity of 10us per tick.
//       As the name implies it is a start/stop watch.  This
//       module has been implemented primarily for determining pulse travel 
//       times of 'ping echoes' to measure distances using the Parallax Ping
//       module, but obviously it has other uses.  The module uses the 16-bit
//       timer (timer1 for the '324) while stop-watch services are in effect.
//       Timer 1 resources are relinquished upon completion of the stop-watch
//       task.

#ifndef __SWATCH324v221_H__
#define __SWATCH324v221_H__

// ================================ includes ================================ //
#include<avr/io.h>

#include "sys324v221.h"
#include "utils324v221.h"
#include "isr324v221.h"
#include "spkr324v221.h"

// ================================= defines ================================ //
// ============================== public declarations ======================= //
typedef unsigned short int SWTIME;  // Parameter for storing the 16-bit 
                                    // stopwatch value obtained from the 16-bit
                                    // timer.

// Structure type declaration for storing internal parameters for the 
// stopwatch service module.
typedef struct STOPWATCH_PARAMS_TYPE {

    volatile SWTIME sw_time;

} STOPWATCH_PARAMS;
// ================================ prototypes ============================== //
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
// Desc: Function allocates and initializes resources needed by the stop-watch
//       service.
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
extern SUBSYS_STATUS STOPWATCH_open( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Returns: Nothing.
// Desc: Function deallocates and releases resources used by the stop-watch
//       service.
extern void STOPWATCH_close( void );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Stop watch clock function drives the 'ticking' of the stop-watch
//       subsystem.
extern void STOPWATCH_clk( void );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Starts the micro-second stopwatch.  The current stop value is NOT
//       reset.  That is, if the stop-watch has already some non-zero value
//       (from a previous stop-watch event) it will simply continue counting
//       from this previous value.  use 'STOPWATCH_rest()' to reset the counter.
extern void STOPWATCH_start( void );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Stops the micro-second stopwatch.  Function returns number of 'ticks'
//       occurred since the stopwatch was started.  Each tick is worth 10us.
//       Thus, since: 
//                      0 <= SWTIME <= 65535, 
//
//      accounts for a time interval of:
//
//                      0 <= SWTIME <= 655.35ms (655350us).
//
extern SWTIME STOPWATCH_stop( void );
// -------------------------------------------------------------------------- //
// TODO
// Desc:  Resets the stop-watch to zero.
extern SWTIME STOPWATCH_reset( void );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Allows the user to set the initial count to a desired value.  The 
//       stop-watch (when started) will begin counting from this specified
//       value.
extern void STOPWATCH_set( SWTIME value );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Allows the user to get the current value of the stop watch.  This is
//       only useful when the stop watch is NOT counting.
extern SWTIME STOPWATCH_get_ticks( void );

// ============================ external declarations ======================= //
extern STOPWATCH_PARAMS STOPWATCH_params;


#endif /* __SWATCH324v221_H__ */
