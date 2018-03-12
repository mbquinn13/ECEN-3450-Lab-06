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
// Desc: Header file contains some important declarations needed to communicate
//       with the ATtiny support MCU on the '324 board.

#ifndef __TINY324FIRM_H__
#define __TINY324FIRM_H__

#include "utils324v221.h"
#include "spi324v221.h"
#include "sys324v221.h"
#include "tmrsrvc324v221.h"
#include "tiny324v221.h"

// ============================== defines =================================== //
// *** ATTiny Command Messages ***
//
// Desc:  These are the command messages sent to the ATTiny by the ATmega324
//        to have it do various tasks -- currently, these are the supported
//        messages.
//
#define ATTMSG_ATTN     0x0A        /* ATTiny attention byte.               */
#define ATTMSG_BL_ADJ   0xB1        /* ATTiny LCD backlight adjust command. */
#define ATTMSG_SERVO    0xB0        /* ATTiny RC servo control command.     */
#define ATTMSG_SW_IR    0xB2        /* ATTiny Switch/IR command.            */
#define ATTMSG_FIRM_REV 0xB3        /* ATTiny Firmware Rev. request command.*/
#define ATTMSG_LONG_FIRM_REV 0xB4   /* ATTiny Firmware Rev. request (long)  */
#define ATTMSG_PWRCYCLE 0xB5        /* ATTiny En/Dis IR Power-Cycling.      */

// *** Servo related ***
//
// NOTE: These limits are experimentally determined.
//
#define SERVO_POS_MAX_LIMIT 2100    /* Full counter-clockwise. (180-deg) */
#define SERVO_POS_MIN_LIMIT 400     /* Full clockwise. (0-deg).          */

// *** Bit Positions regarding sensor data ***
#define SNSR_IR_RIGHT   0x01
#define SNSR_IR_LEFT    0x02
#define SNSR_SW5        0x04
#define SNSR_SW4        0x08
#define SNSR_SW3        0x10
#define SNSR_SW5_STATE  SNSR_SW5
#define SNSR_SW4_STATE  SNSR_SW4
#define SNSR_SW3_STATE  SNSR_SW3
#define SNSR_SW5_EDGE   0x20
#define SNSR_SW4_EDGE   0x40
#define SNSR_SW3_EDGE   0x80

#define MAX_SWITCHES    3

// Desc: The 50ms delay cases the switch state to be queried at a rate
//       no greater than 20 times-per-second, while the 40ms delay causes
//       the IR states to be queried at a rate no greater than 25 times-
//       per-second no matter how fast the 'ATTINY_get_IR_state()' and 
//       'ATTINY_get_SW_state()' functions run inside a loop.
//
#define __SW_WAIT_DELAY 50          /* Wait 50ms between 'Tiny queries. */
#define __IR_WAIT_DELAY 40          /* wait 40ms between 'Tiny queries. */


// =========================== type declarations ============================ //

// Special type for holding firmware information from the ATTiny.  It is used
// by the 'ATTiny_get_firm_rev()' function.
typedef struct ATTINY_FIRMREV_TYPE
{

    unsigned char major;        // Holds 'major' revision number.
    unsigned char minor;        // Holds 'minor' revision number.
    unsigned char status;       // Holds 'status' revision code.

} ATTINY_FIRMREV;

// Enumerated type declaration for specifying power-cycling of IRs with
// 'ATTINY_power_cycle_IR()' function.
typedef enum PWRCYCLE_STATE_TYPE {

    PWRCYCLE_OFF = 0,
    PWRCYCLE_ON

} PWRCYCLE_ST;

// Enumerated type declaration for specifying which switch status' is being
// requested via 'ATTINY_get_SW_state()'.
typedef enum ATTINY_SW_TYPE {

    ATTINY_SW3 = 0,
    ATTINY_SW4,
    ATTINY_SW5

} ATTINY_SW;

// Enumerated type declaration for specifying which IR status' is being
// requested via 'ATTINY_get_IR_state()' function.
typedef enum ATTINY_IR_TYPE {

    ATTINY_IR_LEFT = 0,
    ATTINY_IR_RIGHT,
    ATTINY_IR_EITHER,
    ATTINY_IR_BOTH

} ATTINY_IR;

// Enumerated type declaration for specifying an RC servo.
typedef enum RCSERVO_ID_TYPE { 

    RC_SERVO0 = 0,
    RC_SERVO1,
    RC_SERVO2,
    RC_SERVO3,
    RC_SERVO4

} RCSERVO_ID;

// Structure type declaration for storing related module parameters.
typedef struct ATTINY_PARAMS_TYPE {

    // Servo parameters.
    struct {

        // Servo limits.
        struct {

            // Minimum limits.
            struct {

                unsigned int RCS0;
                unsigned int RCS1;
                unsigned int RCS2;
                unsigned int RCS3;
                unsigned int RCS4;

            } min;

            // Maximum limits.
            struct {

                unsigned int RCS0;
                unsigned int RCS1;
                unsigned int RCS2;
                unsigned int RCS3;
                unsigned int RCS4;

            } max;

        } limits;

        // Servo positions -- these hold the 'current' positions of the
        //                    servo since the 'Tiny doesn't store this data.
        //
        struct {

            unsigned int RCS0;
            unsigned int RCS1;
            unsigned int RCS2;
            unsigned int RCS3;
            unsigned int RCS4;

        } pos;

    } servos;

} ATTINY_PARAMS;

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
// Desc: This function allocates and initializes necessary resources for
//       utilizing the ATtiny support MCU.
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
extern SUBSYS_STATUS ATTINY_open( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Returns: Nothing.
// Desc: This function de-allocates and releases any resources in use by the
//       ATtiny subsystem.
extern void ATTINY_close( void );
// -------------------------------------------------------------------------- //
// Input  Args: TODO
// Output Args:
// Globals  Read:
// Globals Write:
// Returns: Nothing.
//
// Desc:  Use this function to get revision information from the onboard
//        ATTiny support MCU.  You must declare a variable of type 
//        'ATTINY_FIRMREV' and pass **the address** of this variable to the
//        function.  The function will then populate the 'major' and 'minor'
//        fields with appropriate numerical values.
//
extern void ATTINY_get_firm_rev( ATTINY_FIRMREV *pDest );
// -------------------------------------------------------------------------- //
// TODO
// Returns: A single 'byte' whose bits encode the state of IR sensor and 
//          switch data with the following interpretation when the corresponding
//          bit is set to '1':
//
//              bit0:   Left IR sensor is actively triggered.
//              bit1:   Right IR sensor is actively triggered.
//              bit2:   Switch pushed down (S5).
//              bit3:   Switch pushed down (S4).
//              bit4:   Switch pushed down (S3).
//              bit5:   Switch pushed down (S5) [ single-trigger ].
//              bit6:   Switch pushed down (S4) [ single-trigger ].
//              bit7:   Switch pushed down (S3) [ single-trigger ].
//
//          As can be seen a switch 'press' can be monitored through two
//          bits: the 'single-trigger' version and the 'standard' version,
//          if you will.  The 'standard' version will remain set to '1' so 
//          long as the corresponding switched is pushed down.  In contranst,
//          the 'single-trigger' version will be set to '1' when press and 
//          immediately reset to '0' SO LONG AS THE CORRESPONDING BUTTON
//          REMAINS PUSHED.  The 'crude' time-line below illustrates this
//          idea:
//
//                 P = Switch is pushed down.
//                 R = First instance when switch is released.
//                 - = Nothing happening (switch is left loose).
//
//                 S = 'standard' switch bit.
//                 T = 'single-trigger' switch bit.
//
//             
//              User: -------PPPPPPPPR-------PPPPPPPPR------
//                 S: 00000001111111100000000111111111000000
//                 T: 00000001000000000000000100000000000000
//
//          Note in the above timeline that the 'switch bit' for the normal
//          or standard bit remains active ( a '1' ) UNTIL the user releases
//          the switch.  In contrast, the 'single-trigger' version of the same
//          is active ONCE and then immediately reset to 0 and it remains as
//          zero until pressed again where the process repeats.
//
//
// Desc: This function polls the ATtiny for sensor data.  It returns a byte
//       whose bit values represent two kinds of information.  State of the 
//       IR sensors, and state of onboard push-button switches.
extern unsigned char ATTINY_get_sensors( void );
// -------------------------------------------------------------------------- //
// Input Args:  'RC_servo0' - Controls RC servo on port 0.
//              'RC_servo1' - Controls RC servo on port 1.
//              'RC_servo2' - Controls RC servo on port 2.
//              'RC_servo3' - Controls RC servo on port 3.
//              'RC_servo4' - Controls RC servo on port 4.
//
//              The valid range for the above parameters is: 0-255, with
//              0 representing FULL clockwise, and 255 representing FULL
//              counter-clockwise.
//
// Returns: Nothing.
// Desc: This function is used to control the RC-servo motors under the control
//       of the ATtiny.  It performs RC-servo control by sending the appropriate
//       commands to the ATtiny.  The ATtiny takes care of the rest.
extern void ATTINY_set_RC_servos( unsigned int RCS0_pos,
                                  unsigned int RCS1_pos,
                                  unsigned int RCS2_pos,
                                  unsigned int RCS3_pos,
                                  unsigned int RCS4_pos );
// -------------------------------------------------------------------------- //
// TODO: (finish)
// Desc: This function can be used to set the position of an individual 
//       servo without affecting the current position of the others when
//       you have no need to move all 5 servos at once via the
//       'ATTINY_set_RC_servos()' (plural) function.  Before you use this
//       function, you should call 'ATTINY_set_RC_servos()' (plural) to set
//       the initial positions of all servos to a 'safe' position.
extern void ATTINY_set_RC_servo( RCSERVO_ID which, unsigned int RCS_pos );
// -------------------------------------------------------------------------- //
// Desc: Allows you to set 'safe-guard' limits for each of the supported servos.
extern void ATTINY_set_RC_limits( unsigned int min, 
                                  unsigned int max, 
                                  RCSERVO_ID which );
// -------------------------------------------------------------------------- //
// Input Args: 'pwr_cycle_state' - Must be one of the following enumerated
//                                 constants:
//  
//                                  'PWRCYCLE_ON' - Enable power-cycling of IRs.
//                                  'PWRCYCLE_OFF'- Disable power-cycling.
// Output Args: NONE.
// 
// Returns: Nothing.

// Desc: Function enables or disables power-cycling of the IR sensors.  Power
//       cycling shaves off a few dozen milli-amps of so of power usage.  How-
//       ever this power cycling creates noise on the power bus, so under
//       certain circumnstances, power-cycling has to be disabled.  This
//       function allows you to do just that.
extern void ATTINY_power_cycle_IR( PWRCYCLE_ST pwr_cycle_state );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Must be one of the following enumerated constants:
//
//                          'ATTINY_SW3' - Get state data from Switch 3.
//                          'ATTINY_SW4' - Get state data from Switch 4.
//                          'ATTINY_SW5' - Get state data from Switch 5.
//
// Returns: Function returns 'TRUE' if the corresponding switch specified
//          via 'which' is depressed.  This will only be true during the first
//          read.  Any consecutive read after will return 'FALSE' until the
//          user releases the switch.
//
// Desc: Function is a 'LOOP SAFE' implementation of the 'ATTINY_get_sensors()'.
//       This particular function is designed so that it can be 'safely' 
//       implemented inside a loop since it will only trigger once until the
//       user lets go of the button.
extern BOOL ATTINY_get_SW_state( ATTINY_SW which );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Must be one of the following enumerated constants:
//
//                          'ATTINY_IR_LEFT'  - For LEFT IR status.
//                          'ATTINY_IR_RIGHT' - For RIGHT IR status.
//                          'ATTINY_IR_EITHER' - For Either Left OR Right stat.
//                          'ATTINY_IR_BOTH'  - For simultaenous only status.
//
// Returns: Function returns 'TRUE' if the requested IR is being 'triggered' 
//          that is, there is an object blocking the IR sensor.  Otherwise
//          it will return 'FALSE'.  For the 'EITHER' case, the function will
//          return 'TRUE' if either the LEFT or RIGHT (or BOTH) IRs are being
//          triggered.  For the 'BOTH' case, the function will only return
//          'TRUE' if BOTH IRs are being simultaneously triggered.  Note that
//          'EITHER' or 'BOTH' case only tell you if these IRs are being
//          triggered, but they don't tell you WHICH! (Not Yet...).
//
// Desc: Function is a 'LOOP SAFE' implementation of the 'ATTINY_get_sensors()'.
//       This particular function is designed so that it can be 'safely' 
//       implemented inside of a loop without overwhelming the 'Tiny MCU.
extern BOOL ATTINY_get_IR_state( ATTINY_IR which );

// ====================== external declarations ============================= //
extern ATTINY_PARAMS ATTINY_params;

#endif /* __TINY324FIRM_H__ */
