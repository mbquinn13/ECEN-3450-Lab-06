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
// Desc: Header file for the ADC subsystem module.

#ifndef __ADC324V221_H__
#define __ADC324V221_H__

#include <avr/io.h>

#include "utils324v221.h"
#include "sys324v221.h"

// =============================== defines ================================== //
// ============================ type declarations =========================== //
// Custom enumerated type for specifying the ADC channel to capture.
typedef enum ADC_CHAN_TYPE {

    ADC_CHAN0 = 0,
    ADC_CHAN1,
    ADC_CHAN2,
    ADC_CHAN3,
    ADC_CHAN4,
    ADC_CHAN5,
    ADC_CHAN6,
    ADC_CHAN7,
    ADC_CHAN_VBG = 30,   // Band-gap voltage channel (1.1V).
    ADC_CHAN_GND = 31    // GND reference channel (0V).

} ADC_CHAN;

// Custom enumerated type for specifying the reference voltage for the ADC.
typedef enum ADC_VREF_TYPE {

    ADC_VREF_AREF = 0,   // 'AREF', Internal VREF turned OFF.
    ADC_VREF_AVCC,       // 'AVCC' with external cap at AREF pin.
    ADC_VREF_1P1V,       // Internal 1.10V Voltage Reference.
    ADC_VREF_2P56V       // Internal 2.56V Voltage Reference.

} ADC_VREF;

// Custom type for storing a single 10-bit sample acquired via 'ADC_sample()'
// function.
typedef unsigned short int ADC_SAMPLE;

// =============================== prototypes =============================== //
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
// Desc: Function acquires and initializes resources for using the ADC subsystem
//       module.
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
extern SUBSYS_STATUS ADC_open( void );
// -------------------------------------------------------------------------- //
// TODO: (finish)
// Desc: Function de-allocates and releases resources in use by the ADC subsys-
//       tem module.
extern void ADC_close( void );
// -------------------------------------------------------------------------- //
// TODO: (finish)
// Desc: Function allows the user to set the input channel for ADC conversion.
extern void ADC_set_channel( ADC_CHAN which );
// -------------------------------------------------------------------------- //
// TODO: (finish)
// Desc: Function allows the user to set the Voltage Reference for the ADC.
extern void ADC_set_VREF( ADC_VREF which );
// -------------------------------------------------------------------------- //
// TODO: (finish)
// Desc: Function allows you to 'sample' (convert) the currently selected
//       channel to its digital representation.
extern ADC_SAMPLE ADC_sample( void );
// ======================== external declarations =========================== //

#endif /* __ADC324V221_H__ */
