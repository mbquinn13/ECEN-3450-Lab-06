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
// Desc: Header file for the GPI module.  The GPI exposes an interface consist-
//       ing of specific CEENBoT API functions that specifically authored to 
//       be used by the GPI software.  The GPI module for the CEENBoT API 
//       represents the 'thin layer' that stands between the CEENBoT API and 
//       the GPI GUI software. The GPI GUI software generates GPI module 
//       functions.  These functions in turn take advantage of the rich set of
//       functions exposed by the rest of the CEENBoT API.  GPI module is meant
//       to be used strictly by the GPI GUI software.

#ifndef __GPI324V221_H__
#define __GPI324V221_H__

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

#include "capi324v221.h"

// ============================== defines =================================== //
#define GPI_LEDS_SUPPORTED_MASK 0b01100000

// The following macros are used to define certain parameters via existing
// enumerated types in the STEPPER module.
#define GPI_STEPPER_LEFT        LEFT_STEPPER
#define GPI_STEPPER_RIGHT       RIGHT_STEPPER
#define GPI_STEPPER_BOTH        BOTH_STEPPERS

#define GPI_STEP_DIR_FWD        STEPPER_FWD
#define GPI_STEP_DIR_REV        STEPPER_REV

#define GPI_STEP_BRK_ON         STEPPER_BRK_ON
#define GPI_STEP_BRK_OFF        STEPPER_BRK_OFF

// FIXME: Change 'FREERUNNING' to 'FREERUN'(?).
#define GPI_STEPPER_FREERUNNING STEPPER_FREERUNNING
#define GPI_STEPPER_BLOCK       STEPPER_STEP_BLOCK
#define GPI_STEPPER_NO_BLOCK    STEPPER_STEP_NO_BLOCK

#define GPI_STEPPER_EVENT       STEPPER_EVENT

// Desc: Wrappers around 'types' in the TINY subsystem module -- these are
//       used in conjunction with 'GPI_get_IR_state()' and 'GPI_get_SW_state()'
//       set of functions.
#define GPI_SW3 ATTINY_SW3
#define GPI_SW4 ATTINY_SW4
#define GPI_SW5 ATTINY_SW5

#define GPI_IR_LEFT     ATTINY_IR_LEFT
#define GPI_IR_RIGHT    ATTINY_IR_RIGHT
#define GPI_IR_EITHER   ATTINY_IR_EITHER
#define GPI_IR_BOTH     ATTINY_IR_BOTH

// Desc: Wrappers around the RC-SERVO subsystem that is part of the TINY
//       subsystem module.  These are used in conjunction with the
//       'GPI_servo()' function.
#define GPI_SERVO0     RC_SERVO0
#define GPI_SERVO1     RC_SERVO1
#define GPI_SERVO2     RC_SERVO2
#define GPI_SERVO3     RC_SERVO3
#define GPI_SERVO4     RC_SERVO4

// Desc: This helper macro can be used to automatically reset all entries
// of a structure of type 'GPI_MODULES' to 'FALSE'.  This way, the user need
// only worry about those entries that should be set to 'TRUE'.
#define GPI_CLEAR_MODS_STRUCT( gpi_mods_struct ) \
    { \
    gpi_mods_struct.GPI_psxc   = FALSE; \
    gpi_mods_struct.GPI_step   = FALSE; \
    gpi_mods_struct.GPI_lcd    = FALSE; \
    gpi_mods_struct.GPI_led    = FALSE; \
    gpi_mods_struct.GPI_swatch = FALSE; \
    gpi_mods_struct.GPI_usonic = FALSE; \
    gpi_mods_struct.GPI_spkr   = FALSE; \
    gpi_mods_struct.GPI_beep   = FALSE; \
    } /* end GPI_CLEAR_MODS_STRUCT() */

// Desc: This macro marks the END of a GPI program.  It must be the LAST
//       statement in 'CBOT_main()' for ALL GPI programs.
#define GPI_END( ret_code )     while( 1 )

// Desc: This macro-function can be used with 'GPI_play_note()' function.
#define GPI_map_diatone( ... ) SPKR_map_diatone( __VA_ARGS__ )

// Desc: Macro-function which calls on 'GPI_splash()' function with the 
//       assumption that the following 'macro-constants' have been defined:
//
//          __GPI_REV_MAJOR
//          __GPI_REV_MINOR
//          __GPI_REV_BUILD
//
//       See the information for 'GPI_splash()' (below) for additional info.
//
#define __GPI_SPLASH()  GPI_splash( __GPI_REV_MAJOR, \
                                    __GPI_REV_MINOR, \
                                    __GPI_REV_BUILD )

// Desc: Function GPI-to-API mapping macros.  Usage of these is fairly 
//       involved (in terms of explanation).  See the 'GPI developers manual'
//       or the 'API Programmer's Reference' for detailed info.
//
// PXSC mappings:
//
//  GPI_PSXC_read()        -> PSXC_read()
//  GPI_PSXC_plinear_map() -> PSXC_plinear_map()
//  GPI_PSXC_get_center()  -> PSXC_get_center()
//
#define GPI_PSXC_read           PSXC_read
#define GPI_PSXC_plinear_map    PSXC_plinear_map
#define GPI_PSXC_get_center     PSXC_get_center

// =========================== type declarations ============================ //
// Structure type declaration used to specify modules to use (and open) via
// the 'GPI_open()' function.
typedef struct GPI_MODULES_TYPE {

    BOOL GPI_psxc;
    BOOL GPI_step;
    BOOL GPI_lcd;
    BOOL GPI_led;
    BOOL GPI_swatch;
    BOOL GPI_usonic;
    BOOL GPI_spkr;
    BOOL GPI_beep;

    // BOOL GPI_cpu0;       // Open by default.
    // BOOL GPI_cpu1;       // Open by default.
    // BOOL GPI_tmrsrvc;    // Open by default.
    // BOOL GPI_spi;        // Open by default.

} GPI_MODULES;

// Enumerated type declaration for specifying whether LED 'bit-patterns' used
// with 'GPI_led()' function specifies LEDs being turned ON, or LEDs being
// turned OFF.  That is the LED STATE or "GPI_LEDST".
typedef enum GPI_LEDST_TYPE {

    GPI_LED_OFF = 0,    // Bits specify which LEDs should be turned ON.
    GPI_LED_ON,         // Bits specify which LEDs should be turned OFF.
    GPI_LED_TOGGLE      // Bits specify which LEDs should be toggled.

} GPI_LEDST;    

// Enumerated type declaration for specifying the display number.
typedef enum GPI_DISPLINE_TYPE {

    GPI_DISPLINE0 = 3,
    GPI_DISPLINE1 = 2,
    GPI_DISPLINE2 = 1,
    GPI_DISPLINE3 = 0

} GPI_DISPLINE;

// Enumerated type declaration for specifying the units for distance as given
// by 'GPI_ping()' function.
typedef enum GPI_PINGUNITS_TYPE {

    GPI_PINGUNITS_CM = 0,   // Return distance in units of 'cm'.
    GPI_PINGUNITS_FT,       // Return distance in units of 'ft'.
    GPI_PINGUNITS_IN        // Return distance in units of 'in'.

} GPI_PINGUNITS;

// Type declaration for specifying the 'ping distance'.
typedef unsigned short int GPI_PINGDIST;

// The following type declarations tie the following types to types defined
// by the STEPPER module.
typedef STEPPER_RUNMODE     GPI_STEPPER_RUNMODE;
typedef STEPPER_ID          GPI_STEPPER_ID;
typedef STEPPER_DIR         GPI_STEP_DIR;
typedef unsigned short int  GPI_STEP_DIST; 
typedef unsigned short int  GPI_STEP_SPEED;
typedef unsigned short int  GPI_STEP_ACCEL;
typedef STEPPER_BRKMODE     GPI_STEP_BRKMODE;
typedef STEPPER_EVENT_PTR   GPI_STEPPER_EVT;
typedef CAPI_REV            GPI_REV;
typedef signed short int    GPI_RANDOM;
typedef SPKR_FREQ           GPI_SPKR_FREQ;
typedef SPKR_NOTE           GPI_SPKR_NOTE;
typedef SPKR_OCTV           GPI_SPKR_OCTV;
typedef SPKR_TIME           GPI_SPKR_TIME;
typedef BOOL                GPI_BOOL;
typedef ATTINY_SW           GPI_SW;
typedef ATTINY_IR           GPI_IR;
typedef RCSERVO_ID          GPI_SERVO_ID;
typedef SPKR_PLAYNOTE       GPI_PLAYNOTE;
typedef SPKR_MEASURE        GPI_PLAYMEASURE;
typedef SPKR_SONG           GPI_PLAYSONG;
typedef PSXC_TYPE           GPI_PSXC_TYPE;
typedef PSXC_STDATA         GPI_PSXC_DATA;
typedef PSXC_CENTER         GPI_PSXC_CENTER;

// ============================ prototypes ================================== //
// TODO
// Desc: Function initializes GPI subsystem.  It starts up any needed modules
//       required by the GPI GUI.  You must pass to this the **ADDRESS** of a
//       structure of type 'GPI_MODULES' which the particular module's element
//       set to 'TRUE' if you wish to make use of that respective module.  
//       'GPI_open()' will in turn open those modules.
extern void GPI_open( GPI_MODULES *pModules );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Function releases and closes any open GPI modules.
extern void GPI_close( GPI_MODULES *pModules );
// -------------------------------------------------------------------------- //
// Desc: Function display API revision and GPI revision data _IF_, the LCD
//       subsystem module has been opened.  This function must be inserted by
//       the GPI-GUI auto-generator AFTER the modules have been opened, but
//       before user program code is inserted.
//
//       The function requires the following macro-constants to be defined
//       by the GPI-GUI!
//
//          #define __GPI_REV_MAJOR <major-number>
//          #define __GPI_REV_MINOR <minor-number>
//          #define __GPI_REV_BUILD <build-number>
//
//      Where you must specify the values for <major-number>, <minor-number>,
//      and <build-number>.  The versioning number will be formatted as:
//
//          vN.NN.NNN   (where each group of N's separated by the dots, 
//                       correspond to the major, minor and build numbers ).
//
// NOTE: *** DO NOT CALL THIS FUNCTION DIRECTLY **
//      
//       Instead, call the macro-function '__GPI_SPLASH()' with no arguments.
//
extern void GPI_splash( unsigned short int gpi_rev_major,
                        unsigned short int gpi_rev_minor,
                        unsigned short int gpi_rev_build );
// -------------------------------------------------------------------------- //
// Input Arguments: 'pRev' - You must pass to this argument the ADDRESS OF
//                           a structure of type 'GPI_REV'.  The members of
//                           the structure contain revision info.
// 
//
// Returns: The structure pointed to by 'pRev' will be populated with
//          revision info.  The GPI_REV structure contains the following
//          fields:
//
//              .major  (contains MAJOR revision #, 1 or more digits).
//              .minor  (contains MINOR revision #, 2 digits exactly).
//              .build  (contains BUILD revision #, 3 digits exactly)
//              .status (contains a single ASCII character representing the 
//                       build status: a = alpha, b = beta, T = Testing 
//                      (or Release Candidate), R = Release, E = Experimental.
//
// Example:
//
//          GPI_REV rev_info;
//
//          // Get revision info.
//          GPI_get_API_rev( &rev_info );
//
//          // Print the revision info.
//          GPI_display( GPI_DISPLINE0, "Rev: v%d.%02d.%03d%c\n",
//                                                  rev_info.major,
//                                                  rev_info.minor,
//                                                  rev_info.build,
//                                                  rev_info.status );
//
//          Note that the format MUST be: vN.NN.NNNc  (N = Number, c = char).
//
//          Please always ensure that printed version numbers always follow
//          this convention!
//
// Desc: Function can be used to obtain the API revision number.
extern void GPI_get_API_rev( GPI_REV *pRev );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Function implements a the 'Delay' tool in the GPI-GUI.  The 'delay_ms' 
//       parameter, as the name suggests, is given in units of milliseconds.
//       The maximum delay value allowed is 32767 ms (32.767 seconds).

extern void GPI_delay( signed int delay_ms );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Function that implements the 'Led' tool in the GPI-GUI.
extern void GPI_led( GPI_LEDST led_state, unsigned char led_pattern );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Function that implements the 'Display' tool in the GPI-GUI.
extern void GPI_display( GPI_DISPLINE line_number, const char *str, ... );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Function that can be used with the 'Display' tool to clear the display
//       completely.
extern void GPI_clr_disp( void );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Function that implements the 'Ultrasonic' tool in the GPI-GUI.
extern GPI_PINGDIST GPI_ping( GPI_PINGUNITS ping_units );
// -------------------------------------------------------------------------- //
// TODO
// Input  Args: 'run_mode' - Must be one of the following enumerated constatns:
//
//                              'GPI_FREERUNNING', 'GPI_STEP_NO_BLOCK', or
//                              'GPI_STEP_BLOCK'.
//
//              'which' - Must be one of the following enumerated constants:
//
//                              'GPI_STEPPER_LEFT', 'GPI_STEPPER_RIGHT', or
//                              'GPI_STEPPER_BOTH'.
//
//              'dir_L'   - Direction of the left stepper is to take.  It must
//                          be one of the following enumerated constants:
//
//                              'GPI_STEP_DIR_FWD' or 'GPI_STEP_DIR_REV'.
//
//              'steps_L' - Number of steps the left stepper should traverse.
//                          This parameter has NO effect in 'GPI_FREERUNNING'
//                          mode.
//
//              'speed_L' - Speed in steps/sec for the left motor.
//
//              'accel_L' - Acceleration in steps/sec^2 for the left motor.
//
//              'brkmode_L' - Specifies wheather 'brakes' should be engaged
//                            upon motion completion of the left motor.  This
//                            parameter has NO effect in 'GPI_FREERUNNING' mode.
//
//              'step_event_L' - If 'run_mode' is 'GPI_STEP_BLOCK', and if this
//                               parameter is NOT 'NULL', then the function
//                               will invoke a 'stepper event' function defined
//                               via 'GPI_STEPPER_EVENT()' macro.  If no 
//                               'stepper event' notification is required, then
//                               you must pass 'NULL' to this argument.
//
//              'dir_R'   - ( same as 'dir_L',   but for the right stepper ).
//              'steps_R' - ( same as 'steps_L', but for the right stepper ).
//              'speed_R' - ( same as 'speed_L', but for the right stepper ).
//              'accel_R' - ( same as 'accel_L', but for the right stepper ).
//              'brkmode_R' - ( same as 'brkmode_L' but for the right stepper ).
//              'step_event_R' - ( same as 'step_event_L' but for the right ).
//
//
// Desc: Function that implements the 'Move' tool in the GPI-GUI.
//
// Details: This function allows you to issue a 'move' command to the stepper
//          motors onboard the '324 board.  The 'move' commands can be set 
//          independently for the left and right motors.  It can affect both
//          motors simultaneously (with independent settings for each), or it
//          can be issued to only affect one of the motors.  Which motors are
//          affected depends on the argument given for 'which'.
//
//          There are three 'motion' modes of operation, which is indicated by
//          the argument given to 'run_mode'. 'GPI_FREERUNNING' means the once
//          the 'move' command is issued, the motors will move INDEFINITELY.
//          That is the [specified] motors will NEVER stop until a 'GPI_stop()'
//          command is issued some time later.  Issuing a 'GPI_move()' in this
//          run mode will happen immediately -- that is, 'GPI_move()' will NOT
//          BLOCK.  Execution will proceed to the next instruction as soon as
//          the 'GPI_move()' command is issued.  For example:
// 
//              GPI_move( GPI_FREERUNNING, ... );   // Move forward.
//                  ...
//               (do stuff -- and then...)
//                  ...
//              GPI_stop( GPI_LEFT_STEPPER );       // Force a stop.
//
//          The other mode is 'GPI_STEP_BLOCK'.  This is a 'finite-distance'
//          move in which the distance is given by the number of 'steps' that
//          the [specified] motor(s) is to move.  It is called 'GPI_STEP_BLOCK' 
//          because the function will run in a [finite] STEP mode and also
//          because the function will 'block' execution until the 'finite'
//          motion completes -- that is, until the number of specified steps
//          is exhausted and reaches zero.  This is useful when one wishes
//          to define a motion through a given sequence of 'moves' as shown
//          in the snipped below:
//
//              GPI_move( GPI_STEP_BLOCK, ... );    // Turn one way.
//              GPI_move( GPI_STEP_BLOCK, ... );    // Turn another way.
//              GPI_move( GPI_STEP_BLOCK, ... );    // Turn this way.
//              GPI_move( GPI_STEP_BLOCK, ... ):    // Turn that way.
//
//          This allows motions to proceed in an ordered fashion.  That is
//          one motion won't start until the previous one completes.
//          
//          The third mode is 'GPI_STEP_NO_BLOCK'.  This too is a 'finite-
//          distance' move in which the distance is given by the number of 
//          'steps' that the [specified] motor(s) is to move.  However, as soon
//          as the command is issued, execution to the next function will 
//          continue, which is sometimes a desired behavior.  You'll most likely
//          want to use 'GPI_STEP_NO_BLOCK' run mode in conjunction with the
//          'GPI_wait_on()' function.  You have to be careful how you use this
//          mode.  For example, if we defined a complicated motion through
//          a series of moves using this mode as follows:
//
//              GPI_move( GPI_STEP_NO_BLOCK, ... );    // Turn one way.
//              GPI_move( GPI_STEP_NO_BLOCK, ... );    // Turn another way.
//              GPI_move( GPI_STEP_NO_BLOCK, ... );    // Turn this way.
//              GPI_move( GPI_STEP_NO_BLOCK, ... ):    // Turn that way.
//
//          Then the only motion you would actually 'see' take place is the
//          LAST ONE.  REMEMBER -- WE'RE USING NON-BLOCKING VERSIONS NOW.  So
//          in the above example all functions would execute one after the other
//          in a pretty much simultaneous fashion and only the last one would
//          be perceptible as the last function would get the 'last laugh'.
//          The 'GPI_STEP_NO_BLOCK' run mode is given in cases when one wishes
//          to perform other tasks immediately following the command to move
//          the steppers.  This can be done as follows:
//
//              GPI_move( GPI_STEP_NO_BLOCK, ... ); // Turn one way.
//                  ... ( do something else ) ...
//                  ... ( do another  thing ) ...
//              GPI_wait_on( GPI_BOTH_STEPPERS );  // BLOCK HERE!
//
//              GPI_move( GPI_STEP_NO_BLOCK, ... ); // Turn another way.
//                  ... ( do something else ) ...
//                  ... ( do another  thing ) ...
//              GPI_wait_on( GPI_BOTH_STEPPERS );  // BLOCK HERE!
//
//              GPI_move( GPI_STEP_NO_BLOCK, ... ); // Turn this way.
//                  ... ( do something else ) ...
//                  ... ( do another  thing ) ...
//              GPI_wait_on( GPI_BOTH_STEPPERS );  // BLOCK HERE!
//
//              GPI_move( GPI_STEP_NO_BLOCK, ... ); // Turn that way.
//                  ... ( do something else ) ...
//                  ... ( do another  thing ) ...
//              GPI_wait_on( GPI_BOTH_STEPPERS );  // BLOCK HERE!
//
//          TODO: ** FINISH DETAILS ***
//
extern
  void GPI_move( GPI_STEPPER_RUNMODE    run_mode,
                 GPI_STEPPER_ID         which,

                 GPI_STEP_DIR       dir_L,
                 GPI_STEP_DIST      steps_L,
                 GPI_STEP_SPEED     speed_L,
                 GPI_STEP_ACCEL     accel_L,
                 GPI_STEP_BRKMODE   brkmode_L,
                 GPI_STEPPER_EVT    step_event_L,

                 GPI_STEP_DIR       dir_R,
                 GPI_STEP_DIST      steps_R,
                 GPI_STEP_SPEED     speed_R,
                 GPI_STEP_ACCEL     accel_R,
                 GPI_STEP_BRKMODE   brkmode_R,
                 GPI_STEPPER_EVT    step_event_R );
                
// -------------------------------------------------------------------------- //
// TODO
extern
    void GPI_stop( GPI_STEPPER_ID which, GPI_STEP_BRKMODE brkmode );
// -------------------------------------------------------------------------- //
// TODO
// Desc: This function can be issued after a GPI_stop() with the brakes
//       ON -- without 'canceling' any previously issued motion, provided
//       that a motion remains to be completed.
extern
    void GPI_go( GPI_STEPPER_ID which );
// -------------------------------------------------------------------------- //
// TODO
extern
    void GPI_move_wait_on( GPI_STEPPER_ID which );
// -------------------------------------------------------------------------- //
// TODO
extern
    void GPI_move_wait_and_then( GPI_STEPPER_ID  which, 
                                 GPI_STEPPER_EVT stepper_evt_L,
                                 GPI_STEPPER_EVT stepper_evt_R );
// -------------------------------------------------------------------------- //
// TODO
// Returns: Returns an 'unsigned char' whose bit positions have the following
//          interpretation when the corresponding bit is set to '1':
//  
//              bit0:   Left  IR sensor is actively triggered.
//              bit1:   Right IR sensor is actively triggered.
//              
//              bit2-bit7: Always zero.
//
extern unsigned char GPI_get_IR( void );
// -------------------------------------------------------------------------- //
// TODO
// Returns: Returns an 'unsigned char' whose bit positions have the following
//          interpretation when the corresponding bit is set to '1':
//
//              bit0:   Always zero.
//              bit1:   Always zero.
//              bit2:   Switch Engaged (S5 on '324-v221 board).
//              bit3:   Switch Engaged (S4 on '324-v221 board).
//              bit4:   Switch Engaged (S3 on '324-v221 board).
//              bit5:   Same as 'bit 2' but 'single-trigger' mode.
//              bit6:   Same as 'bit 3' but 'single-trigger' mode.
//              bit7:   Same as 'bit 5' but 'single-trigger' mode.
//
//          Note that bits 5,6,7 ALSO refer to S5, S4, and S3 just as bits
//          2,3,4 do, except these are 'single-trigger' versions.
//
extern unsigned char GPI_get_SW( void );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Must be one of the following enumerated constants:
//
//                          'GPI_SW3' - Get state data from Switch 3.
//                          'GPI_SW4' - Get state data from Switch 4.
//                          'GPI_SW5' - Get state data from Switch 5.
//
// Returns: Function returns 'TRUE' if the corresponding switch specified
//          via 'which' is depressed.  This will only be true during the first
//          read.  Any consecutive read after will return 'FALSE' until the
//          user releases the switch.
//
// Desc: Function is a 'LOOP SAFE' implementation of the 'GPI_get_sensors()'.
//       This particular function is designed so that it can be 'safely' 
//       implemented inside a loop since it will only trigger once until the
//       user lets go of the button.

extern GPI_BOOL GPI_get_SW_state( GPI_SW which );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Must be one of the following enumerated constants:
//
//                          'GPI_IR_LEFT'  - For LEFT IR status.
//                          'GPI_IR_RIGHT' - For RIGHT IR status.
//                          'GPI_IR_EITHER' - For Either Left OR Right stat.
//                          'GPI_IR_BOTH'  - For simultaenous only status.
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
extern GPI_BOOL GPI_get_IR_state( GPI_IR which );
// -------------------------------------------------------------------------- //
// Input Args: 'seed' - This is a value anywhere between 0 and 32767.
//
extern void GPI_set_seed( unsigned short int seed );
// -------------------------------------------------------------------------- //
// Desc: Function returns a random seed value (16-bit) that can be used with
//       GPI_set_seed() to set the seed value of the pseudo-random number
//       generator to generate new random sequences when 'GPI_get_random()'
//       is called.  The seed is generated by sampling the ADC values from
//       various sources, such as battery and current channels, and even non-
//       existent channels to increase the probability of a new seed value
//       being generated.  The seed value is persistent for the current session,
//       meaning, if you call the function back-to-back, you'll get the same
//       seed value.  Thus, a new seed is only generated upon power-up and/or
//       a system RESET.
extern unsigned short int GPI_get_seed( void );
// -------------------------------------------------------------------------- //
// Input Args: 'start_val' - This is the value starting from 0 to 32767.
//             'range' - This is the 'range' of integers for which random
//                       values will be generated MINUS one.  See below for
//                       details.  You must take care that 'range' doesn't
//                       push you past 'RAND_MAX' which is 32767.
//
// Desc: Pseudo-random number generator build on top of 'rand()' function.  
//       The start value is the lowest non-negative value that will be gen-
//       erated.  For example, if 'start_val' = 2, then random numbers will
//       begin from '2'.
//
//       The 'range' represents the number of integers (minus one) that will
//       be generated starting from 'start_val'.  For example, if you call:
//
//              GPI_get_random( 7, 50 );
//
//       You will get a random value anywhere between: 7 and 56.
//
//       Or if you specify:
//
//              GPI_get_random( 10, 100 );
//
//       You will get a random value anywhere between: 10 and 109.
extern GPI_RANDOM GPI_get_random( unsigned short int start_val,
                                        unsigned short int range );
// -------------------------------------------------------------------------- //
// Desc: Function plays a tone at the specified frequency.  The tone will 
//       occupy a duration specified by 'duration_ms'.  However, it will only
//       be active (or audible) for a percentage of this 'duration' as specified
//       by 'len'.
//
//       The 'tone_freq' value must be value desired multiplied times 10.
//       Alternatively, you can use the 'SPKR_FREQ()' macro to do the conversion
//       for you, so you can either do:
//
//          GPI_play_tone( 440 * 10, 250, 90 );
//
//       OR...
//
//          GPI_play_tone( SPKR_FREQ( 440 ), 250, 90 );
//
extern void GPI_play_tone( GPI_SPKR_FREQ tone_freq,
                           GPI_SPKR_TIME duration_ms,
                           unsigned short int len );
// -------------------------------------------------------------------------- //
// Input Args: note - This must be a value between 0 and 11 representing 
//                    all the 12 tones in the chromatic scale for any given
//                    octave:
//
//                      0 = C       (SPKR_NOTE_C)
//                      1 = C#      (SPKR_NOTE_C_S)
//                      2 = D       (SPKR_NOTE_D)
//                      3 = D#      (SPKR_NOTE_D_S)
//                      4 = E       (SPKR_NOTE_E)
//                      5 = F       (SPKR_NOTE_F)
//                      6 = F#      (SPKR_NOTE_F_S)
//                      7 = G       (SPKR_NOTE_G)
//                      8 = G#      (SPKR_NOTE_G_S)
//                      9 = A       (SPKR_NOTE_A)
//                     10 = A#      (SPKR_NOTE_A_S)
//                     11 = B       (SPKR_NOTE_B)
//
//                  Instead of numerical values, you can also specify the
//                  predefined macro-constants (given in parenthesis above).
//                  Note that these numerical values map linearly to the entire
//                  chromatic scale.  Sometimes, however, you'll just want to
//                  specify a linear mapping that corresponds to the diatonic
//                  scale (i.e., white keys on the piano only).  For this, you
//                  can use the 'SPKR_map_diatone()' conversion function, which
//                  re-maps the values 0 - 6 to the notes C, D, E, F, G, A, B 
//                  respectively.  (See example below).
//
//              octave - This must be a value between 0 and 5 (6 octaves total):
//
//                    0 = Octave 0  (SPKR_OCTV0)
//                    1 = Octave 1  (SPKR_OCTV1)
//                    2 = Octave 2  (SPKR_OCTV2)
//                    3 = Octave 3  (SPKR_OCTV3)  [ Middle Octave ]
//                    4 = Octave 4  (SPKR_OCTV4)
//                    5 = Octave 5  (SPKR_OCTV5)
//
//                  Instead of the numerical values, you can also specify the
//                  predefined macro-constants (given in parenthesis above).
//
//              transp - This can be a value between -12 to 12, representing
//                       the number of 'semitones' to shift the scale by.  A
//                       positive value will shift the notes by that many
//                       semitones, and a negative will shift it down.
//
//              duration_ms - The number of milliseconds that the given note
//                            will occupy regardless of whether the note is
//                            active (audible) or not.  The maximum value is
//                            32767 milliseconds (32.767 seconds).
//
//              len - An integer between 0 to 100 representing the percentage
//                    of the 'duration_ms' for which the note will be active
//                    (that is, audible).
//
// Desc: This function represents the highest-level musical tone generating
//       function.  It is used to play a 'note' a the specified 'octave' and
//       given 'transposition' for a length of time within a given duration.
//
//       The 'duration_ms' parameter is used to control the length of time that
//       the musical note will 'occupy'.  However, this alone does not dictate
//       for how long the given note is actually 'active' or 'audible'.  
//       That part is controlled by the 'len' parameter, which represents 
//       a percentage (0% to 100%) of the 'duration' given for which the note 
//       will be active.
//
//       For example, if you specify a note for a duration of 500ms, and an
//       'active' length of 75%, then the note will remain on for 375ms (75%
//       of 500), and inactive (silent) for the remaining 125ms, etc.
//
// Example:
//
//       The follwing 'for()' loop will play melodic interval C-E-G three times
//       , with each note staying ON for 80% of the 250ms interval:
//
//          for( i = 0; i < 3; ++i )
//          {
//
//              GPI_play_note( SPKR_NOTE_C, SPKR_OCTV3, 0, 250, 80 );
//              GPI_play_note( SPKR_NOTE_E, SPKR_OCTV3, 0, 250, 80 );
//              GPI_play_note( SPKR_NOTE_G, SPKR_OCTV3, 0, 250, 80 );
//
//          }
//
//       In contranst if we want each note to still occupy 250ms, but be active
//       for a very short time to simulate 'staccato' notes, you would have
//       the notes active, for say 20% of the time:
//
//          for( i = 0; i < 3; ++i )
//          {
//
//              GPI_play_note( SPKR_NOTE_C, SPKR_OCTV3, 0, 250, 20 );
//              GPI_play_note( SPKR_NOTE_E, SPKR_OCTV3, 0, 250, 20 );
//              GPI_play_note( SPKR_NOTE_G, SPKR_OCTV3, 0, 250, 20 );
//
//          }
//
//       Etc.
//
extern void GPI_play_note( GPI_SPKR_NOTE note,
                           GPI_SPKR_OCTV octave,
                           signed short int transp,
                           GPI_SPKR_TIME duration_ms,
                           unsigned short int len );
// -------------------------------------------------------------------------- //
// Input Args:  'pSong' - You must pass to this argument **THE ADDRESS** of
//                        a structure of type 'GPI_PLAYSONG', which must be
//                        'carefully' constructed and populated with song
//                        data: 'GPI_PLAYNOTE' and 'GPI_PLAYMEASURE' structures.
//
// Desc: Function plays a 'song'.  A 'song' is a properly constructed structure
//       of various elements.  First you take a collection of notes called
//       'play notes' which constitutes the notes of your song.  Then you 
//       collect these into 'measures'.  You can construct as many measures as
//       you want (or more precisely -- as memory will allow).  Then you collect
//       this measures into one cohesive  unit called a 'song'.  This 'song' is
//       what you pass to this function.  Then function then goes measure-by-
//       measure and plays the notes in each measure.  An example is given below
//       which makes the procedure clear.
//
// Example: The following example gives a simple example on constructing the
//          song.  Say we want to play the notes, C-E-G (Melodic C-Major) twice
//          followed by the notes F-A-C (Melodic F-Major) 3 times:
//
//              // First construct the collection of notes.
//              GPI_PLAYNOTE measure_1[] = {
//
//                  //  <note>,       <octv>   <transp> <duration> <len>
//                  { SPKR_NOTE_C, SPKR_OCTV3,    0,       250,      80 },
//                  { SPKR_NOTE_E, SPKR_OCTV3,    0,       250,      80 },
//                  { SPKR_NOTE_G, SPKR_OCTV3,    0,       250,      20 }
//
//              };
//
//              GPI_PLAYNOTE measure_2[] = {
//
//                  //  <note>,       <octv>   <transp> <duration> <len>
//                  { SPKR_NOTE_F, SPKR_OCTV3,    0,       250,      80 },
//                  { SPKR_NOTE_A, SPKR_OCTV3,    0,       250,      80 },
//                  { SPKR_NOTE_C, SPKR_OCTV3,    0,       250,      20 }
//
//              };
//
//              // Next we collect our measures together.
//              GPI_PLAYMEASURE measures[] = {
//
//                  // <playnotes> <num_notes> <repeat>
//                  { measure_1,       3,          2 },
//                  { measure_2,       3,          3 }
//
//              };
//
//              // Finally construct the 'Song' itself.
//              GPI_PLAYSONG   song[] = {
//
//                  // <measures>   <num_measures>  <repeat>
//                  { measures,          2,            1 }
//
//              };
//
//              // With the 'song' construcuted, we can now play it.
//              GPI_play_song( song );
//
//          Note in the example there are three principal ingredients:
//
//              1) One or more arrays of 'GPI_PLAYNOTE' structures.  In the 
//                 above example, we declared two such arrays, each one repre-
//                 senting a 'measure'.  Note the parameters for each entry
//                 in the 'GPI_PLAYNOTE' structure follows the exact same 
//                 convention as the arguments passed to the 'GPI_play_note()'
//                 function.  Refer to that function for detailed info on 
//                 the meaning of each entry.
//
//              2) One or more arrays of 'GPI_PLAYMEASURE' structures.  In the
//                 above example we collected the individual 'measures' into
//                 a single collection of 'measures'.  The first column is
//                 the name of the 'GPI_PLAYNOTE' structure, the second is the
//                 number of notes (3 in each case), and the third is the number
//                 of times that each measure should be repeated.  In our case
//                 we want the first measure repeated twice, and the second 
//                 repeated 3 times.  So the notes will play:
//
//                      C-E-G-C-E-G-F-A-C-F-A-C-F-A-C.
//
//              3) An array of 'GPI_PLAYSONG' structures containing your 
//                 collection of measures, however number you may have.  
//                 In our example,
//                 we have a single such container, hence only one entry.
//                 The first column lists the name of the 'meausures' array,
//                 while the second the number of such measures contained in
//                 it, and the last one, the number of times to repeat the
//                 collection of measures.
//
//                 Obviously this means there are two opportunities for repet-
//                 ition.  One is on a 'per-measure' context, and another one
//                 on a 'per-song' context.
//              

extern void GPI_play_song( GPI_PLAYSONG *pSong );
// -------------------------------------------------------------------------- //
// Desc: This function is similar to the 'GPI_play_note()' function except it
//       it is not very accurate and is meant for generating beeps with 
//       'ball-park' frequency values.  Like 'SPKR_play_note()', 'duration_ms'
//       specifies the duration of time the given beep will occupy in time
//       (whether audible or not).  The 'len' parameter, in turn, specifies the
//       percentage of the 'duration_ms' value for which the beep IS audible.
//
//       For example, if you specify 500ms beep, and 75% length, then the beep
//       will be audible for 375ms (active), and remain quiet for the remaining
//       125ms.
extern void GPI_play_beep( GPI_SPKR_FREQ beep_frequency,
                           GPI_SPKR_TIME duration_ms,
                           unsigned short int len );
// -------------------------------------------------------------------------- //
// Input Args: 'which' - Must be one of the following enumerated constants:
//
//                          'GPI_SERVO0'
//                          'GPI_SERVO1'
//                          'GPI_SERVO2'
//                          'GPI_SERVO3'
//                          'GPI_SERVO4'
//
//              'servo_pos' - Must be a value between 400 and 2100.  These 
//                            values correspond to the approximate 'extremes'
//                            of the positional servos.
//
// Desc: This function can be used to set the position of an individual servo.
extern void GPI_servo( GPI_SERVO_ID which, unsigned int servo_pos );
// -------------------------------------------------------------------------- //

#endif /* __GPI324V221_H__ */
