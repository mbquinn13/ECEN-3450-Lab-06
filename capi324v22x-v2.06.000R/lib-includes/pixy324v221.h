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

// Auth: J. Santos
// Desc: Header file for the 'Pixy' cam (aka CMUcam5) subsystem module.

#ifndef __PIXY324V221_H__
#define __PIXY324V221_H__

#include<avr/interrupt.h>

#include "spi324v221.h"
#include "utils324v221.h"
#include "sys324v221.h"
#include "tmrsrvc324v221.h"
#include "uart324v221.h"
#include "led324v221.h"

// ============================ defines ==================================== //
// Desc: Macro-function for declaring PIXY callback functions.
#define PIXY_CALLBACK( func_name, param_name ) \
    void func_name( PIXY_DATA * param_name ); /* Prototype */ \
    void func_name( PIXY_DATA * param_name )  /* Signature */
    
// =========================== type declarations =========================== //

// Desc: Enumerated type declaration to specify the state of the parser engine.
typedef enum PIXY_STATE_type {

    PIXY_STATE_IDLE = 0,
    PIXY_STATE_SYNCED1,
    PIXY_STATE_SYNCED2,
    PIXY_STATE_GETTING_DATA

} PIXY_STATE;

// Desc: Structure type declaration for block data.
typedef struct __attribute__ ((__packed__)) PIXY_DATA_type {

    unsigned short int cksum;   // Checksum.
    unsigned short int signum;  // Signature number.

    // Centroid position coordinates.
    struct {

        unsigned short int x;  // Varies from 0 to 319.
        unsigned short int y;  // Varies from 0 to 239.

    } pos;  

    // Bounding box width and height (size).
    struct {
        
        unsigned short int width;
        unsigned short int height;

    } size;

    BOOL has_data;      // This is only meaningful when callbacks are
                        // not registered with the PIXY engine.

} PIXY_DATA;

// Desc: Type definition for declaring callback function pointers for processing
//       pixy tracking events.
typedef void ( *PIXY_CALLBACK_FUNCTION_PTR )( PIXY_DATA * pixy_data );

// Desc: Structure type declaration for holding internal state variables.
//       Some of the parameters are obtained from the data block sent by
//       the Pixy, while others are derived from this data for convenience.
typedef struct PIXY_PARAMS_type {

    volatile BOOL synced;                       // Set to true when synced with
                                                // the Pixy's sync bytes.

    volatile PIXY_STATE state;                  // Pixy's reception state.

    PIXY_CALLBACK_FUNCTION_PTR pixy_callback;   // Callback function pointer.

    PIXY_DATA *p_pixy_data_struct;              // Holds address of user's
                                                //  registered structure.

    volatile BOOL buf_overrun;                  // 'TRUE' if an internal
                                                //  buffer overrun has occured.

    volatile unsigned short int nchars;         // Holds number of character
                                                //  (bytes) received during
                                                //  transmission of block data.

} PIXY_PARAMS;

// =========================== prototypes ================================== //
// Input Args: None.
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
// Desc: Function opens and initializes the PIXY subsystem module.
//       Users should at least check to see if function returns 'SUBSYS_OPEN'
//       or 'SUBSYS_ALREADY_OPEN' and proceed accordingly.  The rest of these
//       errors are mostly for debugging purpses.  Thus, a typical 'open' 
//       sequence might go something like:
//
//          SUBSYS_STATUS opstat;
//
//          // Attempt to open.
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
//        The PIXY must be attached to the CEENBoT via UART1 and this may or
//        may not require a level translator from 5V-to-3.3V since the PIXY is
//        a 3.3V system.  
//
//                  ** AGAIN YOU MUST USE UART1 and NOT UART0! **
//
//        The PIXY _MUST_ be configured via the 'PixyMon' utility (provided by
//        the manufacturer of the PixyCam to output data via the UART serial
//        port _WHICH IS NOT THE DEFAULT_ (the default is SPI!).  Lastly, the
//        PIXY must also be configured for a default UART baud rate of 57600bps,
//        b/c that's the speed that the library will open the PIXY at.  Note
//        that you cannot configure these camera parameters within the API.
//        You must use 'PixyMon' to do that!
//
//        FIXME: Consider adding option for specifying BAUD rate, and leave it
//               to the user to decide this.
//
//        The function will do a quick check to see if the camera is streaming
//        ANY data (even if it's trash) to determine if a PIXY is indeed
//        connected on UART1.
//
//        OBVIOUSLY!, the camera must be powered (and connected/wired appro-
//        prietly) before invokation of this function.
//
//        As soon as you inovke 'PIXY_open()' users should IMMEDIATELY register
//        a 'PIXY_DATA' structure and a 'callback' function which will be 
//        invoked each time an object is detected by the PixyCam.  The user 
//        then makes decisions about what to do next with the CEENBoT inside 
//        of this callback function.  Refer to 'PIXY_register_callback()' 
//        function for details.
extern SUBSYS_STATUS PIXY_open( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Returns: Nothing.
// Desc: Closes the PIXY subsystem.
extern void PIXY_close( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Returns: Nothing.
// Desc: This function begins the tracking task, which means the CEENBoT will
//       listen for tracking data streaming down the UART port.  This data will
//       then be parsed, and stored in a user-supplied structure of type
//       'PIXY_DATA'.  Once this structure is filled, a user-supplied callback
//       function will be triggered.   It is in this callback that the user then
//       READS the PIXY_DATA structure to make decisions about what to do next
//       (i.e., make the CEENBoT turn left/right, slow down or speed-up, etc).
//
//       The PIXY_DATA structure and callback functions MUST be registered with
//       the subsystem via 'PIXY_register_callback()' function, which should be
//       called immediately after you call 'PIXY_open()'.
extern void PIXY_track_start( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Returns: Nothing.
// Desc: This function stops the tracking task, which means the CEENBoT stops
//       listening for tracking data streaming down the UART port.
extern void PIXY_track_stop( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Returns: Returns a value of type BOOL, being 'TRUE' if new data has been
//          written by the PIXY into the data structure registered by the
//          user, or 'FALSE' otherwise.
// Desc: This function should ONLY be used when NO CALLBACK has been
//       registered with the PIXY subsystem module.  When no callback mechanism
//       is in place, then the user can 'poll' the PIXY subsystem to see if
//       it has written new data.  After the user has made use of this data
//       the user should then call 'PIXY_process_finished()' to let the
//       PIXY subsystem module that it is now 'safe' to write in new data.
extern BOOL PIXY_has_data( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Returns: Nothing.
// Desc: This function should ONLY be used when NO CALLBACK has been
//       registered with the PIXY subsystem module.  Usually, the user should
//       poll the PIXY subsystem if new data has been written by it by calling
//       'PIXY_has_data()'.  After the user reads the data and is done
//       processing, s/he then calls 'PIXY_process_finished()' to let the
//       PIXY underlying engine know that it is now safe to write in new
//       data onto the user's previously registered data structure of type
//       PIXY_DATA.
extern void PIXY_process_finished( void );
// -------------------------------------------------------------------------- //
// Input Args: 'pixy_callback' - Pass to this argument the name of a callback
//                               function that must have been PREVIOUSLY declared
//                               with the 'PIXY_CALLBACK()' macro. (See 'Desc'
//                               below for details).
//
//             'p_pixy_data_struct' - Pass to this argument _THE ADDRESS_ of a
//                                    structure of type 'PIXY_DATA' previously
//                                    declared by you.  You should declare this
//                                    data structure GLOBALLY.
//
// Returns: Nothing.
//
// Desc: This function should be invoked immediately after you invoke
//       'PIXY_open()'.  It allows you to register two things with the subsystem:
//       a) a callback function which will be called each time an object is
//       detected matching the color-class trained via PixyCam, and b) a
//       'PIXY_DATA' data structure that is declared in GLOBAL scope.  This
//       structure is filled with data relating the position and size of the
//       detected object.
//      
//       The callback, must be declared via the 'PIXY_CALLBACK()' macro-function,
//       which takes two parameters:  the name you wish to give the callback,
//       and the name you wish to give to the pointer to the 'PIXY_DATA'
//       structure.  Thus, somewhere at the beginning of your program you might
//       do:
//
//          PIXY_CALLBACK( process_object, p_pixyobj )
//          {
//
//              // Access the structure as follows...
//              if( ( p_pixyobj->pos.x > 10 ) && ( p_pixyobj->pos.x < 250 ) )
//              {
//                  // ... Go faster... or -something-.
//              }
//          }
//
//       Thus, the above declares (and defines) the callback and its body.  The
//       declaration of the 'PIXY_DATA' structure should be done in GLOBAL
//       scope, and you can call that whatever you want.
//
//          PIXY_DATA pixy_data;   // Structure holds detected object data.
//
//       Then, after you invoke 'PIXY_open()', you would do the following:
//
//          PIXY_register_callback( process_object, &pixy_data );
//
//       Note that 'process_object', being a _function pointer_ in disguise
//       does NOT require the '&' b/c it already represents an address.
//
//       Note that the name you have the 'PIXY_DATA' object when you declared
//       it (e.g., 'pixy_data') does NOT have to match the name you assign it
//       in the 'PIXY_CALLBACK()' macro (e.g., 'p_pixyobj').
//
//       If you do not register a callback, nor a structure, you'll have no
//       way to make decisions when objects are detected and nothing will
//       happen.
extern void PIXY_register_callback( PIXY_CALLBACK_FUNCTION_PTR pixy_callback,
                                    PIXY_DATA *p_pixy_data_struct );
// =======================  external declarations ========================== //
extern PIXY_PARAMS PIXY_params;

#endif /* __PIXY324V221_H__ */
