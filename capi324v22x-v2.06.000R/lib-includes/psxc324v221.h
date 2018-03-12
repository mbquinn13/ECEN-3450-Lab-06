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
// Desc: Header file for the Playstation controller module.  Contains sets of 
//       function/services that allow interacting with PSX-compliant cont-
//       rollers.

#ifndef __PSX324V221_H__
#define __PSX324V221_H__

#include<avr/io.h>
#include<stdlib.h>

#include "utils324v221.h"
#include "sys324v221.h"
#include "spi324v221.h"
#include "tmrsrvc324v221.h"
#include "lcd324v221.h"

// ============================== defines =================================== //

// Controller bit definitions ( 5th byte response 'psxc_response[ 4 ]' ).
#define L2_BIT          0x01
#define R2_BIT          0x02
#define L1_BIT          0x04
#define R1_BIT          0x08
#define TRI_BIT         0x10
#define CIR_BIT         0x20
#define X_BIT           0x40
#define SQR_BIT         0x80

// Other button bit definitions (4th byte response 'psxc_response[ 3 ]' ).
// Except for 'ANALOG ONLY' mode, these bits are present in both digital
// and analog mode (red mode) data streams.
#define SLCT_BIT        0x01
#define JOYR_BIT        0x02        /* Analog mode only */
#define JOYL_BIT        0x04        /* Analog mode only */

#define L3_BIT          JOYR_BIT    /* Alternate names (Analog mode only) */
#define R3_BIT          JOYL_BIT    /* Alternate names (Analog mode only) */

#define STRT_BIT        0x08
#define DPAD_UP_BIT     0x10    /* Digital pad UP    */
#define DPAD_RT_BIT     0x20    /* Digital pad RIGHT */
#define DPAD_DN_BIT     0x40    /* Digital pad DOWN  */
#define DPAD_LT_BIT     0x80    /* Digital pad LEFT  */

// ============================ declarations ================================ //
// Enumerated type declaration for the type of data returned by the PSXC
// controller, whether analog-mode data or digital-mode data.
typedef enum PSXC_TYPE_TYPE {

    PSXC_INVALID = 0,
    PSXC_DIGITAL = 1,
    PSXC_ANALOG = 2

} PSXC_TYPE;

// Structure type declaration for controller pushbutton/joystick/dpad status
// data or simply 'status data'.
typedef struct PSXC_STDATA_TYPE {

    PSXC_TYPE data_type;      // Variable holds the type of data (analog or 
                              // digital) or if it contains invalid data.

    unsigned char buttons0;   // Select/Start/DPAD and L3/R3 status.
    unsigned char buttons1;   // Shoulder buttons and right-hand side button 
                              // status.
    
    // Holds analog data for the left analog joystick.  The values contained
    // range from -128 to 127, with ~0/1 being the IDEAL 'center'.
    struct {

        signed char up_down;        
        signed char left_right;

    } left_joy;

    // Holds analog data for the right analog joystick. The values contained
    // range from -128 to 127, with ~0/1 being the IDEAL 'center'.
    struct {

        signed char up_down;
        signed char left_right;

    } right_joy;

} PSXC_STDATA;

// Structure type declaration used when requesting 'default center' values
// for the analog sticks via 'PSXC_get_center()' function.
typedef struct PSXC_CENTER_TYPE {

    struct {

        signed char up_down;
        signed char left_right;

    } left_joy;

    struct {

        signed char up_down;
        signed char left_right;

    } right_joy;

} PSXC_CENTER;

// Type declaration for holding internal parameters for the PSXC module.
typedef struct PSXC_PARAMS_TYPE {

    // 'center' property holds the default center values (idle stick values)
    // obtained from the PSX controller.  These values represent the 'dead
    // center' values when the analog sticks are left idle on their own.
    struct {

        BOOL valid;

        struct {

            unsigned char up_down;
            unsigned char left_right;

        } left_joy;

        struct {

            unsigned char up_down;
            unsigned char left_right;

        } right_joy;

    } center;

} PSXC_PARAMS;

// ============================= prototypes ================================= //
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
// Desc:  This function allocates resources and initializes the playstation
//        controller interface for use.  If PSXC function relies on other
//        subsystems, it will attempt to restart those subsystems if closed.
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
extern SUBSYS_STATUS PSXC_open( void );
// -------------------------------------------------------------------------- //
// Desc: This function de-allocates resources and closes the PSXC subsystem.
//       This 'may' free resources if those resources are not in use by other
//       subsystems as well.
extern void PSXC_close( void );
// -------------------------------------------------------------------------- //
// Desc:  This function allows you to read the current state of the PSX
//        controller.  The function returns 'FALSE' if the obtained data is
//        no longer valid (because of controller disconnection, for example).
//        The user is encouraged to analyze the return value to determine
//        that it is indeed valid before taking any 'action' as a result of
//        this acquired controller data.  For example, if the function returns
//        'FALSE', the user might wish to recalibrate the manner in which
//        controller values for the analog sticks are 'mapped' to the user's
//        application ( see 'PSXC_get_center()' function ).
extern BOOL PSXC_read( PSXC_STDATA *pStatus_data );
// -------------------------------------------------------------------------- //
// Desc:  This function allows the user to get default 'center' values for
//        the analog sticks when in ANALOG mode.  These center values corresp-
//        ond to the values read from the controller when the sticks are left
//        to center on their own (idle).  A user can then use these
//        values to calibrate the PSXC controller for custom needs, since
//        various PSX controller vendors may assume different 'center' values
//        when in the analog sticks are left to idle.  The function returns
//        TRUE, when the PSX subsystem has successfully read and determined
//        'center values'.  When the function returns FALSE, the PSX subsystem
//        has NOT yet (for whatever reason) read any analog values and has 
//        not been able to determine the corresponding 'center' values.  Reasons
//        for this could be a disconnected controller, or the controller has
//        not been yet started (by the user, such as wireless controller), or
//        not yet initialized (failing to call 'PSXC_open()').
extern BOOL PSXC_get_center( PSXC_CENTER *pCenter_data );
// -------------------------------------------------------------------------- //
// Input  args: 'analog_val' - This is the 'raw' controller value from the
//                             analog stick of the controller.  This value
//                             can range between 0 to 255 (0x00 - 0xFF).  It
//                             comes from controller data, which can be obt-
//                             ained via 'PSXC_read()' function.
//              'min_in_val' - Must be a value between 0-127.  An analog input
//                             value below 'min_in_val' will be mapped to zero.
//              'max_in_val' - Must be a value between 0-127.  An analog input
//                             value above 'max_in_val' will be mapped to the
//                             maximum output value of 'max_out_val'.
//
//                             NOTE:  The following relation must hold between
//                                    the values of 'min_in_val' and 
//                                    'max_in_val':
//
//                                       0 <= min_in_val < max_in_val <= 127
// Desc: This function maps a controller value 'analog_val' from the linear
//       range of 'min_in_val' to 'max_in_val' to the output range from zero
//       to 'max_out_val'.  The mapping is done in a 'pseudo-linear' manner
//       (hence the name of the function 'plinear').  Therefore, the mapping
//       is not necessarily accurate or one-to-one, but yields generally 
//       acceptable results.
extern signed short int PSXC_plinear_map( signed char analog_val,
                                          signed char min_in_val,
                                          signed char max_in_val,
                                          signed short int max_out_val );
                                                 
// -------------------------------------------------------------------------- //
// Desc: This function is for debugging and testing purposes.  It displays
//       PSXC data 4 times/sec pertaining to controller status (e.g., buttons
//       digital pad, analog sticks, etc).  Make sure the PSXC subsystem is
//       'open' before invocation of this function.
extern void PSXC_run_test( void );
// -------------------------------------------------------------------------- //
// Desc: This function is to be used after 'PSXC_run_test()' has been issued
//       It stops the test that is in progress.
extern void PSXC_stop_test( void );

// =========================== external globals ============================= //
extern volatile PSXC_PARAMS PSXC_params;
extern TIMEROBJ psxc_test_timer;
extern const unsigned char psxc_poll_seq[];
extern unsigned char psxc_response[];

#endif /* __PSXC324V221_H__ */
