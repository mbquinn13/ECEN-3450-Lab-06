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
// Desc: Header file for the 'CMU Cam' subsystem module.

#ifndef __CMUCAM324V221_H__
#define __CMUCAM324V221_H__

#include <string.h>
#include "uart324v221.h"
#include "utils324v221.h"
#include "sys324v221.h"
#include "tmrsrvc324v221.h"

// =============================== defines ================================== //

#define CMUCAM_FDATA_LEN    600 /* F-Data packets send 600 bytes to hold a
                                    bitmap frame 80x60 pixels. */

#define CMUCAM_MAX_BINS     32  /* Maximum number of histogram bins that
                                    can be obtained by histogram packets. */

#define CMUCAM_TOTAL_MSG_IDs    9



// Desc: This macro-function is used to declare as a prototype, and also define
//       (in a separate statement) the body of the callback function for 
//       T-data packets.  The function is invoked by the callback engine once
//       T-data packets arrive after the user has registered them with the
//       CMUCam subsystem module via 'CMUCAM_register_tdata_callback()'
//       function.  Note that the callback receives a single parameter consis-
//       ting of a pointer to a 'CMUCAM_TDATA' structure.  The very first
//       task within your callback is to ensure the data is valid, which
//       you do by checking the 'is_valid' element of this structure, which
//       is a variable of type 'BOOL'.  If this value is 'FALSE', then you
//       should simply DO NOTHING in your callback as this does not necessarily
//       constitute an error.  In the process of receiving multiple data 
//       streams at different rates, some data will be ultimately lost.
//
//       Example:
//
//       // We -define- the body of the callback as follows:
//       CMUCAM_TDATA_CALLBACK( process_tdata, p_tdata )
//       {
//
//          // Check that the structure is valid.
//          if ( CMU_STRUCT_IS_GOOD( p_tdata )
//          {
//
//              // The data in 'p_tdata_struct' is valid.  For example,
//              // we can do the following, ONLY if ANY pixels are being
//              // tracked.
//              if ( p_tdata_struct->conf > 0 )
//              {
//
//                  // ... do something here ...
//
//              } // end if()
//
//          } // end if()
//      
//       } // end process_tdata() callback.
//
#define CMUCAM_TDATA_CALLBACK( tname, param_name ) \
    void tname( CMUCAM_TDATA * param_name); /* Prototype */ \
    void tname( CMUCAM_TDATA * param_name)  /* Signature */



// Desc: This macro-function is used to declare as a prototype, and also define
//       (in a separate statement) the body of the callback function for
//       S-data packets.  The function is invoked by the callback engine once
//       S-data packets arrive after the user has registered them with the
//       CMUCam subsystem module via 'CMUCAM_register_sdata_callback()'
//       function.  Note that the callback receives a single parameter consis-
//       ting of a pointer to a 'CMUCAM_SDATA' structure.  The very first
//       task within your callback is to ensure the data is valid, which
//       you do by checking the 'is_valid' element of this structure, which
//       is a variable of type 'BOOL'.  If this value is 'FALSE', then you
//       should simply DO NOTHING in your callback as this does not necessarily
//       constitute an error.  In the process of receiving multiple data 
//       streams at different rates, some data will be ultimately lost.
//
//       Example:
//
//       // We -define- the body of the callback as follows:
//       CMUCAM_SDATA_CALLBACK( process_sdata, p_sdata )
//       {
//
//          // Check that the structure is valid.
//          if ( CMU_STRUCT_IS_GOOD( p_data ) )
//          {
//
//              // The data in 'p_sdata_struct' is valid.  For example,
//              // we can do the following, ONLY if ANY pixels are being
//              // tracked.
//              if ( ( p_sdata_struct->mean.red > 150  ) &&
//                   ( p_sdata_struct->mean.red < 180  ) )
//              {
//
//                  // ... do something here ...
//
//              } // end if()
//
//          } // end if()
//      
//       } // end process_sdata() callback.
//
#define CMUCAM_SDATA_CALLBACK( sname, param_name ) \
    void sname( CMUCAM_SDATA * param_name ); /* Prototype */ \
    void sname( CMUCAM_SDATA * param_name )  /* Signature */


// Desc: The following defines the set of internal camera registers.
//       Only those registers of interest are listed.
#define CMUCAM_REG_AGC  0x00    /* Automatic Gain Control Register */
#define CMUCAM_REG_BLUE_GAIN    0x01
#define CMUCAM_REG_RED_GAIN     0x02
#define CMUCAM_REG_GREEN_GAIN   0x16

#define CMUCAM_YUV( y, u, v )   (v),(y),(u)

// Desc: The following helper macros are meant to simplify the syntax
//       when processing TDATA or SDATA structures in interrupt or
//       process context. Process context is in effect when the user does
//       NOT register a 'callback' function via 
//       'CMUCAM_register_sdata_callback()' or 
//       'CMUCAM_register_tdata_callback()' by setting the callback argument
//       to NULL in these functions.
//
//       When doing process-context processing, usually, for example, you 
//       might have something like:
//
//          void process_tdata( CMUCAM_TDATA *p_tdata )
//          {
//
//              // Don't do anything unless the data is good.
//              if( CMUCAM_STRUCT_IS_GOOD( p_tdata ) )
//              {
//
//                  /*   Do something with TDATA structure.  */
//
//                  // Note we're done.  This is _REQUIRED_ in process-context.
//                  CMUCAM_STRUCT_FINISHED( p_tdata );
//
//              }
//              
//          }
//
//      In interrupt context, however, which occurs when you DO register a
//      callback, the use of 'CMU_STRUCT_FINISHED()' is _NOT NEEDED_.  However,
//      you should still check if the structure is good:
//
//          CMUCAM_TDATA_CALLBACK( process_tdata, p_tdata )
//          {
//              if ( CMUCAM_STRUCT_IS_GOOD( p_tdata ) )
//              {
//                  /* Do something with TDATA structure. */
//              }
//          }
//
#define CMUCAM_STRUCT_IS_GOOD( p_cmucam_struct ) \
    ((p_cmucam_struct->has_data == TRUE)&&(p_cmucam_struct->is_valid==TRUE))

#define CMUCAM_STRUCT_FINISHED( p_cmucam_struct ) \
    p_cmucam_strict->has_data = FALSE

// =========================== type declarations ============================ //

// Desc: Structure type declaration for specifying a color.  Note that this
//       can also be used to hold YUV colors (or VYU order, specifically)
//       when the YUV color space is selected.  The union allows for both
//       color spaces to be treated equally by the user.
typedef union CMUCAM_COLOR_type {

    struct {
        unsigned char red;
        unsigned char green;
        unsigned char blue;

    } rgb;

    struct {

        // The order for YUV color space is in this manner
        // as provided by CMUcam4 documentation.
        unsigned char v;
        unsigned char y;
        unsigned char u;

    } yuv;

} CMUCAM_COLOR;



// Desc: Structure type declaration for specifying a color range
//       that spans from an initial 3-point color to another 3-point color.
//       Note that this can also be used to hold YUV colors (or VYU order,
//       specifically) when the YUV color space is selected.
typedef struct CUMCAM_COLOR_RANGE_type {

    CMUCAM_COLOR from;
    CMUCAM_COLOR to;

} CMUCAM_COLOR_RANGE;


// Desc: Type declaration for specifying the message IDs.
//
// WARNING: MAKE SURE 'CMUCAM_TOTAL_MSG_IDs' DECLARED ABOVE HAS THE
//          SAME NUMBER AS DEFINED IN THE ENUM BELOW.  ERROR CODES
//          IN STARTING FROM 255 (and backing down) DO NOT COUNT.
//
typedef enum CMUCAM_MSG_ID_type {

    CMUCAM_MSG_ACK    = 0,
    CMUCAM_MSG_NACK   = 1,
    CMUCAM_MSG_ERR    = 2,
    CMUCAM_MSG_MSG    = 3,
    CMUCAM_MSG_DAT    = 4,
    CMUCAM_MSG_F_TYPE = 5,
    CMUCAM_MSG_H_TYPE = 6,
    CMUCAM_MSG_S_TYPE = 7,
    CMUCAM_MSG_T_TYPE = 8,
    CMUCAM_MSG_SINGLETON = 9,

    CMUCAM_MSG_TIMEOUT = 254,
    CMUCAM_MSG_UNKNOWN = 255

} CMUCAM_MSG_ID;

// Desc: This enumerated type declaration is for specifying the different
//       status commands when issued to the CMUCam.  The assigned values
//       are the same as 'CMUCAM_MSG_ID' type, so that these can be typecasted
//       from one form to another.
typedef enum CMUCAM_CMD_STATUS_type {

    CMUCAM_CMD_ACK  = 0,
    CMUCAM_CMD_NACK = 1,
    CMUCAM_CMD_ERR  = 2,

    CMUCAM_CMD_NOT_READY = 253,
    CMUCAM_CMD_TIMEOUT   = 254,
    CMUCAM_CMD_UNKNOWN   = 255

} CMUCAM_CMD_STATUS;

// Desc: The following enumerated type declaration is for specifying the
//       switching mode for the 'CMUCAM_set_switching_mode()' function.
typedef enum CMUCAM_SW_MODE_type {

    CMUCAM_SW_OFF = 0,  // To disable 'switching mode'.
    CMUCAM_SW_ON  = 1   // To enable 'switching mode'.

} CMUCAM_SW_MODE;

// Desc: The following enumerated type declaration is for specifying the
//       gain mode for the 'CMUCAM_set_autogain_mode()' function (i.e.,
//       turn it ON or turn it OFF.
typedef enum CMUCAM_AUTOGAIN_MODE_type {

    CMUCAM_AUTOGAIN_OFF = 0,    // Turn autogain OFF.
    CMUCAM_AUTOGAIN_ON  = 1     // Turn autogain ON.

} CMUCAM_AUTOGAIN_MODE;

// Desc: The following enumerated type declaration is for specifying the
//       frame resolution when saving to the microSD card via
//       the 'CMUCAM_save_frame_to_microSD()' function.
typedef enum CMUCAM_FRAMERES_type {

    CMUCAM_FRAMERES_640X480 = 0,    // Resolution: 640x480.
    CMUCAM_FRAMERES_320X240 = 1,    // Resolution: 320x240.
    CMUCAM_FRAMERES_160X120 = 2,    // Resolution: 160x120.
    CMUCAM_FRAMERES_80X60   = 3     // Resolution: 80x60.

} CMUCAM_FRAMERES;

// Desc: The following enumerated type declaration is for specifying
//       the focused tracking mode with 'CMUCAM_focused_tracking_mode()'
//       function.
typedef enum CMUCAM_FOCUS_MODE_type {

    CMUCAM_FOCUS_MODE_OFF = 0,
    CMUCAM_FOCUS_MODE_ON  = 1

} CMUCAM_FOCUS_MODE;

// Desc: The following enumerated type declaration is for specifying the
//       CMUCAM color space mode via the 'Color Tracking (CT)' command
//       by way of 'CMUCAM_set_color_space()' function.  Note that when
//       YUV color space is set, 
typedef enum CMUCAM_CSPACE_type {

    CMUCAM_CSPACE_RGB = 0,  // RGB color space.
    CMUCAM_CSPACE_YUV       // YUV color space.

} CMUCAM_CSPACE;

// Desc: Structure declaration for storage of 'bitmap tracking' data from
//       F-type packets.
typedef struct CMUCAM_FDATA_type {

    unsigned char data[ CMUCAM_FDATA_LEN ];

} CMUCAM_FDATA;

// Desc: Structure declaration for the storage of 'statistical' data.
//       According to documentation the 8-bit value is mapped such that
//       0 = 0% and 255 = 100% of the corresponding 'metric'.
typedef struct __attribute__ ((__packed__)) CMUCAM_SDATA_type {

    // Contains the 'mean' for each color component.
    CMUCAM_COLOR mean;

    // Contains the 'median' for each color component.
    CMUCAM_COLOR median;

    // Contains the 'mode' for each color component.
    CMUCAM_COLOR mode;

    // Contains the 'standard deviation' for each color component.
    CMUCAM_COLOR stdev;

    BOOL is_valid;  // This -must- be checked by the user first before
                    // reading other entries from this structure.

    BOOL has_data;  // This is set when the structure has been written with
                    // new packet data, provided a callback function has
                    // NOT been registered.  If a callback function HAS been
                    // registered, this parameter should be IGNORED.  Once the
                    // user has extracted and made use of this data, the user
                    // _MUST_ MANUALLY clear this to 'FALSE'.  Otherwise
                    // new data will never be written to the structure by
                    // the CMUcam engine.  Again, this is ONLY valid if a
                    // callback function has NOT been registered via
                    // 'CMUCAM_register_sdata_callback()'.

} CMUCAM_SDATA;

// Desc: Structure declaration for the storage of 'tracking' data.
//       According to documentation the 8-bit value is mapped such that
//       0 = 0% and 255 = 100%.
typedef struct __attribute__ ((__packed__)) CMUCAM_TDATA_type {

    struct {

        unsigned char x;
        unsigned char y;
        
    } centroid;

    struct {

        // Represents the upper left-corner coordinate bounding
        //  tracked image data.
        struct {

            unsigned char x;
            unsigned char y;

        } upper_left;

        // Represents the lower right-corner coordinate bounding
        //  tracked image data.
        struct {

            unsigned char x;
            unsigned char y;

        } lower_right;

    } bound;

    unsigned char pixels; // Represents %-pixels tracked in color tracking wind.
    unsigned char conf;   // Represents %-pixels in bounding box.

    BOOL is_valid;  // This -must- be checked by the user first before
                    // reading other entries from this structure.

    BOOL has_data;  // This is set when the structure has been written with
                    // new packet data, provided a callback function has
                    // NOT been registered.  If a callback function HAS been
                    // registered, this parameter should be IGNORED.  Once the
                    // user has extracted and made use of this data, the user
                    // _MUST_ MANUALLY clear this to 'FALSE'.  Otherwise
                    // new data will NEVER be written to the structure
                    // by the CMUcam engine.  Again, this is ONLY valid if
                    // a callback function has NOT been registrered via
                    // 'CMUCAM_register_tdata_callback()'.


} CMUCAM_TDATA;

// Desc: Structure declaration for storing 'histogram' data.
//       According to documentation the 8-bit value is mapped such that
//       0 = 0% and 255 = 100%.
typedef struct __attribute__ ((__packed__)) CMUCAM_HDATA_type {

    unsigned char bin[ CMUCAM_MAX_BINS ];
    unsigned char len;                      // Stores tha actual length (# of
                                            //  bins in the 'bin[]' array where
                                            //
                                            //   'len <= CMUCAM_MAX_BINS'.
                                            //
                                            //  that is, it might be equal to,
                                            //  or less than, this value.

} CMUCAM_HDATA;

// Desc: The following type declaration is used for declaring function
//       pointer callbacks for processing special type of data packets.
typedef void (*CMUCAM_TDATA_CALLBACK_PTR)( CMUCAM_TDATA *p_tdata_struct );

// Desc: The following type declaration is used for declaring function
//       pointer callbacks for processing special type of data packets.
typedef void (*CMUCAM_SDATA_CALLBACK_PTR)( CMUCAM_SDATA *p_sdata_struct );

// Desc: Holds internal state variables related to the CMUCam by the API.
typedef struct CMUCAM_PARAMS_type {

    volatile BOOL ready;  // Set to 'TRUE' if the -prompt- has been received.
                          // After each command is issued, the CMUCam replies
                          // with a '\r', the prompt character to let you know
                          // it is ready to receive more.

    volatile BOOL in_poll_mode; // Set to 'TRUE' when the CMUCam's poll mode
                                //  has been enabled.

    volatile BOOL is_idling;    // Set to 'TRUE' when an 'idle' command has
                                //  been issued and we're waiting for the
                                //  first character received as a result of
                                //  that command.

    volatile BOOL is_streaming; // Set to 'TRUE' if the CMUCam is presently
                                // busy streaming tracking data.

    volatile BOOL buf_overrun;  // Set to 'TRUE' if the packet string did not
                                //  fit in the message buffer.  When this 
                                //  occurs, the data should be discarded
                                //  as it is invalid.

    volatile BOOL waiting_singleton; // Set to 'TRUE' if we're waiting for
                                     // a _single_ piece of data.

    volatile unsigned short int nchars;  // Holds number of character (bytes) 
                                         //  received during the last trans-
                                         //  action.

    volatile unsigned char tdata_freq;  // The frequency of T-data packet
                                        // notifications.  If 'n' value, only
                                        // every 'n' packet will be sent. So
                                        // If '1', every packet will trigger
                                        // a callback.  If '2', then every other
                                        // packet will trigger a callback, etc.

    volatile unsigned char sdata_freq;  // The frequency of S-data packet
                                        // notifications (see explanation for
                                        // 't_data_freq', above).

    volatile unsigned char tdata_interval; // The current T-packet interval.
    volatile unsigned char sdata_interval; // The current S-packet interval.
                                         
    volatile CMUCAM_CMD_STATUS last_cmd_status; // Holds the status of the
                                                //  last command issued to the
                                                //  CMUCam.

    CMUCAM_TDATA_CALLBACK_PTR tdata_callback; // Stores pointer to callback
                                              //  function registered by user.

    CMUCAM_TDATA *p_tdata_struct;  // Pointer to TDATA structure supplied by
                                   //  the user upon registration of tdata
                                   //  callback.

    CMUCAM_SDATA_CALLBACK_PTR sdata_callback; // Stores pointer to callback
                                              //  function registered by user.

    CMUCAM_SDATA *p_sdata_struct;  // Pointer to SDATA structure supplied by
                                   //  the user upon registration of sdata
                                   //  callback.

    CMUCAM_SDATA *p_sdata_struct_copy;

    volatile unsigned char reg_data; // For reading and writing register values.

    volatile CMUCAM_CSPACE current_cspace; // Keeps track of the
                                           //  current color space.

    volatile BOOL waiting_for_spacket; // 'TRUE' if we're waiting for an
                                       // S-type packet.  (Specifically when
                                       // using learning/sampling mode).

} CMUCAM_PARAMS;


// =============================== prototypes =============================== //
// Input Args: None.
// Output Args: None.
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
// TODO: Consider modifying this function to take a parameter which specifies
//       the serial baud connection speed with the CMUCam.
//
// Desc: This function opens and initializes the CMUCam subsystem module.
//       It has a dependency on the UART subsystem module (specifically UART1)
//       and thus, UART1 cannot be used while the CMUCam subsystem is active.
//       This dependency is automatically taken care of.  That is, UART1 is
//       automatically initialized by this subsystem module and the user
//       does NOT need to invoke 'UART_open()' function.
//
//       The function will wait until the camera "is ready" before returning
//       and thus, there's no need to follow 'CMUCAM_open()' call with a 
//       'CMUCAM_is_ready()' call, as 'CMUCAM_open()' will fail if this is
//       not the case.  
//       
//       Unlike other 'open()' functions, where checking the
//       return value is not very important, it is imperative that the user
//       verify a successful open with the CMUCAM subsystem, because a
//       successful open means that communication with the camera is successful
//       and that it has been verified that it is 'READY' for use.  Users
//       should NOT ASSUME this to be the case without checking the return
//       value.  Therefore, the proper procedure should be:
//
//          SUBSYS_STATUS open_stat;
//
//          open_stat = CMUCAM_open();
//          
//          if( open_state == SUBSYS_OPEN )
//          {
//
//              // Start playing w/ the CMUCAM.
//
//          }
//
//          else
//          {
//
//              // Something went wrong, and you should NOT do anything
//              //  with the CMUCAM.
//          }
//
//       Only 'SUBSYS_OPEN' should be expected as the CMUcam is not a sharable
//       resource, so the user will want exclusive use of this device.  Any
//       other value returned by this function should be treated as an error!
//
//       The camera's default rate will be boosted from the default's 
//       19200bps to 115200bps.  Note that this speed is precompiled into the
//       library and cannot be changed unless the library is itself re-compiled
//       or changed to allow for arbitrary changes of speed (perhaps a future
//       feature).
extern SUBSYS_STATUS CMUCAM_open( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Output Args: None.
// Returns: Nothing.
// Desc: Closes the camera subsystem module.  The camera 'timeout timer' is
//       stopped, the serial devices in used are closed and released, and
//       interrupt servies are stopped.
extern void CMUCAM_close( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
//
//
// Desc: This function performs a 'hardware' reset by using a reset
//       signal the to RESET pin of the CMUCam.  The function will not
//       return until the CMUCam is 'READY' (i.e., it has sent the prompt
//       character).  Hardware reset is preferred over software reset b/c
//       it also results in the CMUCam being reset to the default baud
//       rate.  However, it will automatically boost the baud rate back
//       to its high-speed default.
extern CMUCAM_CMD_STATUS CMUCAM_reset( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
//
// Desc: This function performs a 'software' reset by issuing a reset command
//       to the CMUCam.  The function will not return until the CMUCam is
//       'READY' (i.e., it has sent the prompt character).  Issuing a soft
//       reset to the CMUcam will stop all streaming (if any) and it will
//       bring the CMUcam back to its default baud rate.  However, the function
//       automatically takes care of boosting the bad rate back to its
//       high-speed default.
//
extern CMUCAM_CMD_STATUS CMUCAM_soft_reset( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Output Args: None.
// Returns: Returns 'TRUE' if the camera -is- ready (i.e., it -has- received
//          the 'prompt' character.
//
// Desc: The function returns 'TRUE' (a 'BOOL' type) if the CMUCAM subsystem
//       module has received the prompt character via the serial interface
//       indicating the CMUCAM is ready to receive a command.
extern BOOL CMUCAM_is_ready( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Output Args: None.
// Returns: Returns 'TRUE' if the camera is presently streamind data, other-
//          wise it returns 'FALSE'.
//
// Desc: The function returns 'TRUE' (a 'BOOL' type) if the CMUCAM subsystem
//       is streaming data (i.e., T-, or S-type data packets).  The camera
//       can be configured to continuously stream T-type and S-type data
//       packets.  This function can be used to determine if that is happening
//       or not.  It's primary function is to determine if streaming is taking
//       place when one wishes to send the camera a new command.  In the case
//       that streaming is taking place, discontinue this streaming by 
//       issuing 'CMUCAM_idle()'.
extern BOOL CMUCAM_is_streaming( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
//
// Desc: Function sends an 'idle' command to the CMUCAM.  This essentially
//       consists of sending the newline character and waits for the command
//       prompt to show.  It is useful for cancellling previously streaming
//       packet data.
extern CMUCAM_CMD_STATUS CMUCAM_idle( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
//
// Desc: This function returns the status of the last command issued.
extern CMUCAM_CMD_STATUS CMUCAM_get_last_cmd_status( void );
// -------------------------------------------------------------------------- //
// Input Args: 'tdata_callback'  - You must pass to this parameter the name
//                                 of a callback function previously declared
//                                 (and defined) with 'CMUCAM_TDATA_CALLBACK()' 
//                                 macro.  When this parameter is other than
//                                 'NULL', the callback function will be invoked
//                                 upon reception of 'TDATA' packets.  If this
//                                 parameter is set to 'NULL', then no callbacks
//                                 will be issued and the user should simply
//                                 poll the structures to determine if new
//                                 data has been supplied.
//
//             'p_tdata_struct'  - Pass to this the address of a structure of
//                                 type 'CMUCAM_TDATA'.  This structure will
//                                 be populated with revelant tracking data
//                                 upon completion of a frame scan.
//
//             'tdata_frequency' - Supply to this parameter an integer greater
//                                 than, or equal to 1.  A value of 1, means
//                                 you want to receive new packet data on 
//                                 -every- frame.  A value of 2 means you want
//                                 to receive data on every other frame, etc.
//                                 This parameter allows you to slow down the
//                                 rate at which new data is received.
//
// Output Args: None.
// Returns: Nothing.
// Desc: This function allows the user to register a callback that is invoked
//       by the subsystem module when it receives 'T-type' data packets from
//       the CMUCam.  The callback must have been previously declared and
//       defined using the 'CMUCAM_TDATA_CALLBACK()' macro.  Along with this
//       information, the user must also provide a pointer to a preallocated
//       TDATA structure, which will be populated with corresponding data 
//       extracted from the T-type data packet.
//
//       Alternatively, the user can pass 'NULL' to the callback argument.
//       In this case the behavior will be that the user has to check the
//       'has_data' field until it is 'TRUE', at which point the user can
//       retreive the packet data from the registered structure but NO callback
//       will be issued.  The user MUST manually clear 'has_data' back to
//       'FALSE' to receive new data.
void CMUCAM_register_tdata_callback( CMUCAM_TDATA_CALLBACK_PTR tdata_callback,
    CMUCAM_TDATA *p_tdata_struct, unsigned int tdata_frequency );
// -------------------------------------------------------------------------- //
// Input Args: 'sdata_callback' - You must pass to this parameter the name
//                                of a callback function previously declared
//                                (and defined) with 'CMUCAM_SDATA_CALLBACK()'
//                                macro.  When this parameter is other than
//                                'NULL', the callback function will be invoked
//                                upon reception of 'SDATA' packets. If this
//                                parameter is set to 'NULL', then no callbacks
//                                will be issued and the user should simply
//                                poll the structures to determine if new
//                                data has been supplied.
//
//             'p_sdata_struct' - Pass to this the address of a structure of
//                                type 'CMUCAM_SDATA'.  This structure will be
//                                populated with revelant statistical data upon
//                                completion of a frame scan.
//
//             'sdata_frequency' - Supply to this parameter an integer greater
//                                 than, or queal to 1.  A value of 1, means
//                                 you want to reveive new packet data on 
//                                 -every- frame.  A value of 2 means you want
//                                 to receive data on every other frame, etc.
//                                 This parameter allows you to slow down the
//                                 rate at which new data is received.
// Output Args: None.
// Returns: Nothing.
// Desc: This function allows the user to register a callback that is invoked
//       by the subsystem module when it receives 'S-type' data packets from
//       the CMUCam.  The callback must have been previously declared and
//       defined using the 'CMUCAM_SDATA_CALLBACK()' macro.  Along with this
//       information, the user must also provide a pointer to a preallocated
//       SDATA structure, which will be populated with corresponding data
//       extracted from the S-type data packet.
//
//       Alternatively, the user can pass 'NULL' to the callback argument.
//       In this case the behavior will be that the user has to check the
//       'has_data' field until it is 'TRUE', at which point the user can
//       retreive the packet data from the registered structure but NO callback
//       will be issued.  The user MUST manually clear 'has_data' back to
//       'FALSE' to receive new data.
void CMUCAM_register_sdata_callback( CMUCAM_SDATA_CALLBACK_PTR sdata_callback,
    CMUCAM_SDATA *p_sdata_struct, unsigned int sdata_frequency );
// -------------------------------------------------------------------------- //
// Input Args: 'sdata_frequency' - Controls the frequency at which S-type
//                                 packets are populated and callbacks are
//                                 triggered.  Must be an integer 1 or greater.
// Output Args: None.
// Returns: Nothing.
// Desc: This function allows you to set/change the frequency at which S-type 
//       packets trigger callbacks.
void CMUCAM_set_sdata_frequency( unsigned int sdata_frequency );
// -------------------------------------------------------------------------- //
// Input Args: 'tdata_frequency' - Controls the frequency at which T-type
//                                 packets are populated and callbacks are
//                                 triggered.  Must be an integer 1 or greater.
// Output Args: None.
// Returns: Nothing.
// Desc: This function allows you to set/change the frequency at which T-type
//       packets trigger callbacks.
void CMUCAM_set_tdata_frequency( unsigned int tdata_frequency );
// -------------------------------------------------------------------------- //
// Input Args: 'R_min' - Minimum color for red: can be 0 to 255.
//             'R_max' - Maximum color for red: can be 0 to 255.
//             'G_min' - Minimum color for green: can be 0 to 255.
//             'G_max' - Maximum color for green: can be 0 to 255.
//             'B_min' - Minimum color for blue: can be 0 to 255.
//             'B_max' - Maximum color for blue: can be 0 to 255.
//       
//       Note the values must be chosen such that:
//
//             R_min < R_max,  G_min < G_max, and B_min < B_max
//
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
//
//
// Desc: Function sets the color tracking parameters.  Specifically, it
//       sets the color ranges used for color tracking.  Colors outside these
//       ranges are ignored, while those that fall within the specified color
//       ranges will constitute 'tracked pixels'.  This function only sets the
//       range, but does not initiate tracking.  If the camera is currently
//       streaming, it will be stopped.
extern CMUCAM_CMD_STATUS CMUCAM_set_color_tracking_params( 
        unsigned char R_min,
        unsigned char R_max,
        unsigned char G_min,
        unsigned char G_max,
        unsigned char B_min,
        unsigned char B_max );
// -------------------------------------------------------------------------- //
// Desc: This function works exactly like 'CMUCAM_set_color_tracking_params()'
//       with the exception that it takes a single parameter 'range' structure
//       as opposed to six separate ones.
extern CMUCAM_CMD_STATUS CMUCAM_set_color_tracking_range( 
                                                CMUCAM_COLOR_RANGE range );
// -------------------------------------------------------------------------- //
// Input Args: 'top_left_x' - Top left X-coordinate: can be 0 to 160.
//             'top_left_y' - Top left Y-coordinate: can be 0 to 120.
//             'bottom_right_x' - Bottom right X-coordinate: can be 0 to 160.
//             'bottom_right_y' - Bottom right Y-coordinate: can be 0 to 120.
//
//      Note that the above parameters must be specified such that:
//
//          top_left_x < bottom_right_x   and   top_left_y < bottom_right_y
//
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
//
// Desc: Function sets the tracking region (i.e., region of interest) within
//       the FOV of the camera where image processing will take place.  By
//       default, the CMUCam is setup to track the entire window, which
//       is a frame of a 160x120 pixels.  If the camera is currently streaming
//       it will be idled.
extern CMUCAM_CMD_STATUS CMUCAM_set_tracking_region( 
    unsigned short int top_left_x,
    unsigned short int top_left_y,
    unsigned short int bottom_right_x,
    unsigned short int bottom_right_y );
// -------------------------------------------------------------------------- //
// Input Args: 'focus_mode' - Specify for this argument one of the following
//                            enumerated constants:
// 
//                              'CMUCAM_FOCUS_MODE_ON' - To enable histogram
//                                                       tracking (focused).
//
//                              'CMUCAM_FOCUS_MODE_OFF' - To disable histogram
//                                                        tracking (un-focused).
//                                                        (This is the default
//                                                        behavior).
//
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
//
//                                                      
// Desc: Allows the user to focus the tracking analysis to either all pixels
//       (both 'tracked' and 'untracked') or to focus specifically on 
//       'tracked' pixels only.  When only 'tracked' pixels are being followed,
//       statistical information returned (such as that obtained by
//       'CMUCAM_get_mean()') corresponds only with those 'tracked' pixels.
//       Otherwise (and this is the default mode) statistical information
//       returned pertains to both tracked and untracked pixels.
//       This function implements what the CMUCam calls "Histogram Tracking"
//       command ("HT").  This function only affects 'S-type' data packets.
//       If the camera is currently streaming, it will be idled.
extern CMUCAM_CMD_STATUS CMUCAM_focused_tracking_mode( 
                                            CMUCAM_FOCUS_MODE focus_mode );
// -------------------------------------------------------------------------- //
// Input Args: 'color_space' - Supply a value of type 'CMUCAM_CSPACE', which
//                             can be one of the following:
//
//                                  'CMUCAM_CSPACE_RGB' - RGB color space.
//                                  'CMUCAM_CSPACE_YUV' - YUV color space.
//
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
// 
// Desc: Function sets the color space that the CMUCam will work in.
//       By default the CMUCam's color space is RGB.  However this color
//       space can be changed to YUV.  Many basic image processing tasks
//       are somewhat easier when performed in YUV as opposed to RGB and thus
//       the reason for its obvious inclusion.    When YUV is active, the
//       RGB channels are mapped to YUV as follows:  R -> V, G -> Y, B -> U.
//       Please note the ORDER! (i.e., YUV color space is spelled in that order
//       but the mapping is VYU instead!).  If the CMUCAM is currently
//       streaming, this streaming will be stopped.
extern CMUCAM_CMD_STATUS CMUCAM_set_color_space( CMUCAM_CSPACE color_space );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
// 
// Desc: Function enables color tracking (if previously 'idled') with the
//       previously set color tracking parameters.  If these parameters
//       have yet to be set, the default parameters upon reset take effect
//       which corresponds to ALL the colors, and the tracking region consisting
//       of the entire window (camera FOV).
extern CMUCAM_CMD_STATUS CMUCAM_track( void );
// -------------------------------------------------------------------------- //
// Input Args: 'R_min' - Minimum color for red: can be 0 to 255.
//             'R_max' - Maximum color for red: can be 0 to 255.
//             'G_min' - Minimum color for green: can be 0 to 255.
//             'G_max' - Maximum color for green: can be 0 to 255.
//             'B_min' - Minimum color for blue: can be 0 to 255.
//             'B_max' - Maximum color for blue: can be 0 to 255.
//       
//       Note the values must be chosen such that:
//
//             R_min < R_max,  G_min < G_max, and B_min < B_max
//
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
// 
// Desc: This function enables color tracking.  It takes the range of colors
//       to take under consideration when color tracking is active.  Upon
//       successful reception of the command the CMUCam will begin issuing
//       T-data (tracking) packets on a continous basis.  Provided the user
//       has registered a t-data callback function via 'CMUCAM_register_tdata_
//       callback()', it will be invoked each time a new T-data packet 
//       arrives.  This stream will be continuous until the user issues 
//       a command to the CMUCam that interrupts that process (i.e., such as
//       when issuing an 'idle' command via 'CMUCAM_idle()' function).
extern CMUCAM_CMD_STATUS CMUCAM_track_color( unsigned char R_min,
                                             unsigned char R_max,
                                             unsigned char G_min,
                                             unsigned char G_max,
                                             unsigned char B_min,
                                             unsigned char B_max );

// -------------------------------------------------------------------------- //
// Desc: This function does the same thing as 'CMUCAM_track_color()' with the
//       exceptiont that it takes a structure as its argument as opposed to
//       six separate arguments.
extern CMUCAM_CMD_STATUS CMUCAM_track_color_range( CMUCAM_COLOR_RANGE range );
// -------------------------------------------------------------------------- //
// Input Args: 'R_range' - +/- Range added to Average RED: 0 to 255.
//             'G_range' - +/- Range added to Average GREEN: 0 to 255.
//             'B_range' - +/- Range added to Average BLUE: 0 to 255.
//
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
// 
// Desc: This function enables color tracking.  It will track pixels that
//       have a red/green/blue, respectively, that is between the computed
//       red/green/blue averages, respectively, plus/minus the specified
//       range.  If the camera is currently streaming, it will be idled.
//
// NOTE: The exact operation of this command isn't very clearly
//       explained in the CMUcam manual.  It is till provided here for
//       whatever its worth.
extern CMUCAM_CMD_STATUS CMUCAM_track_window( unsigned char R_range,
                                              unsigned char G_range,
                                              unsigned char B_range );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
// 
// Desc: This function enables the streaming of statistical data (S-type pack-
//       ets) from the CMUCam.  Upon succesful reception of the command the
//       CMUCam will begin issuing S-data (statistical) packets on a cont-
//       inuous basis.  Provided the user has registered an s-data callback
//       function via 'CMUCAM_register_sdata_callback()', it will be invoked
//       each time a new S-data packet arrives.  This stream will be cont-
//       inous until the user issues a command to the CMUCam that interrupts
//       that process (i.e., such as when issuing an 'idle' command via
//       'CMUCAM_idle()' function).
extern CMUCAM_CMD_STATUS CMUCAM_get_mean( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
// 
// Desc: Function enables 'poll mode'.  In this mode, commands that normally
//       -stream- simply do not, and istead send a single sample.  For example,
//      'CMUCAM_get_mean()' and 'CMUCAM_track()' (as some examples) will stream
//       data on a continuous basis.  However, when poll made is enabled, only
//       a single packet is received, and the camera immediately falls back
//       into idle mode.
extern CMUCAM_CMD_STATUS CMUCAM_enable_poll_mode( void );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
//
// Desc: Function disables 'poll mode'.  By default the camera has poll mode
//       disabled.  See 'CMUCAM_enable_poll_mode()' for information about
//       poll mode.
extern CMUCAM_CMD_STATUS CMUCAM_disable_poll_mode( void );
// -------------------------------------------------------------------------- //
// Input Args: 'mode' - Pass to this argument one of the following predefined
//                      enumerated constants:
//
//                      'CMUCAM_AUTOGAIN_OFF' - To turn autogain OFF.
//                      'CMUCAM_AUTOGAIN_ON'  - Guess.... ;o)
//
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
//
// Desc: Function can be used to enable or disable autogain.
//
extern CMUCAM_CMD_STATUS CMUCAM_set_autogain_mode( CMUCAM_AUTOGAIN_MODE mode );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Output Args: None.
// Returns: A value of type 'BOOL', which is 'TRUE' if the camera is currently
//          in poll mode, or 'FALSE' otherwise.
//
// Desc: Function can be used to check if the camera is in poll mode.
extern BOOL CMUCAM_in_poll_mode( void );
// -------------------------------------------------------------------------- //
// Input Args: You must specify one of the following enumerated constants:
//
//              'CMUCAM_SW_ON'  - To activate 'switching mode'.
//              'CMUCAM_SW_OFF' - To disable 'switching mode'.
//
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
//
// Desc: Function enables 'switching mode'.  When this mode is active,
//       requesting tracking data or statistical data will result in BOTH
//       data packets being sent (when streaming).  As streaming occurs, these
//       packets will alterante between T-type packets and S-type packets,
//       whether you just request statistical data (via 'CMUCAM_get_mean()'),
//       or tracking data (via 'CMUCAM_track()', 'CMUCAM_track_color()', or
//       'CMUCAM_track_window()').
extern CMUCAM_CMD_STATUS CMUCAM_set_switching_mode( CMUCAM_SW_MODE sw_mode );
// -------------------------------------------------------------------------- //
// Input Args: 'frame_resolution' - You must specify one of the following
//                                  enumerated constants:
//
//                                    'CMUCAM_FRAMERES_640X480'
//                                    'CMUCAM_FRAMERES_320X240'
//                                    'CMUCAM_FRAMERES_160X120'
//                                    'CMUCAM_FRAMERES_80X60'
//
// Output Args: None.
// Returns: Returns an enumerated integer of type 'CMUCAM_CMD_STATUS', which
//          can take on one of the following values:
//
//              'CMUCAM_CMD_ACK'  - Camera acknowledged command.
//
//              'CMUCAM_CMD_NACK' - Camera sent a 'NOT Acknowledged' command.
//
//              'CMUCAM_CMD_ERR'  - An error has occurred.
//
//              'CMUCAM_CMD_NOT_READY' - You have sent a command to the camera
//                                       while it is not ready to receive any.
//
//              'CMUCAM_CMD_TIMEOUT' - CMUCAM Subsystem timed out while waiting
//                                     for reply from the camera.
//
//              'CMUCAM_CMD_UNKNOWN' - Unknown error.
//
//
// Desc: Function will save the current frame in view to the microSD card
//       provided one is in the SD-card slot and it is formatted as either FAT16
//       or FAT32.  This operation takes a long time.  Files are saved as
//       progressive numbers of the form XXXXXBM.BMP in RGB565 (16-bit color)
//       format.
//
//       The frame is not frozen, instead as pixels come in are saved in the 
//       file.  This is equivalent as saving a picture over a long exposure.
//       Consequently image quality may be poor and/or images may appear to
//       'tear' if the camera is moving while the image is being captured.
//
//       The function will returm 'CMUCAM_CMD_ACK' to indicate that the
//       CMUCam has received the command.  However, if problems arised in
//       the process of saving the image (e.g., you actually don't have a
//       card inserted, or the card is not supported, or full, etc), you
//       should invoke 'CMUCAM_get_last_cmd_status()' _AFTER_ an attempt
//       has been made to save to determine if an ERROR has occurred.
//
//       Lastly, this command takes a long time to execute (while data is
//       written to the microSD).  Therefore, after issuing of this command,
//       you should WAIT until the camera is 'ready' as follows:
//
//          // Dump frame to microSD.
//          if ( CMUCAM_save_frame_to_microSD( CMUCAM_FRAMRES_160X120 ) ==
//              CMUCAM_CMD_ACK )
//
//              // Wait until the camera is done.
//              while( CMUCAM_is_ready() == FALSE );
//
//
//          // ... Do something else...
//
extern CMUCAM_CMD_STATUS CMUCAM_save_frame_to_microSD( CMUCAM_FRAMERES 
    frame_resolution );
// -------------------------------------------------------------------------- //
// Input Args: 'p_color' - Pass to this argument the address of a structure
//                         of type 'CMUCAM_COLOR'.  It will contain the color
//                         suggested for tracking (baseline).
//
//             'p_crange' - Pass to this argument the address of a structure
//                          of type 'CMUCAM_COLOR_RANGE'.  It will contain the
//                          RGB color range suggested for tracking based 
//                          around the baseline color.
//
//              'p_sdata_struct' - Pass to this argument the address of a 
//                                 structure of type 'CMUCAM_SDATA'.  It will
//                                 contain statistical data pertaining to the
//                                 suggested color (for whatever its worth).
//
//             'window' - Pass a value between 1 and 255, denoting the window
//                        around the baseline color.  For example a window
//                        of 10 for a base color of (120, 130, 142) will
//                        create a range of: (110, 120, 132) to (130, 140, 152)
//                        etc.
//
// Output Args: 'p_color' - This structure will contain the chosen color
//                          for tracking.
//
//              'p_crange' - This structure will contain the chosen range
//                           color to track around the baseline color.  This
//                           information is internally passed to the
//                           'CMUCAM_track_color()' function.
//
//              'p_sdata_struct' - The structure will be populated with
//                                 statistical information regarding the
//                                 observed color for FYI purposes.
//
// Desc: This function, when invoked will read statistical data about the frame
//       and then set a color range based on the median information collected.
//       It is nothing more than a cheap method to get some preliminary
//       color tracking data for tracking purposes.  YOU MUST provide the
//       addresses of all arguments requested or the function will result
//       in unpredictable behavior.
//
//       Note also that the function requires the latest statistical data
//       to decide on what colors to pick.  So if you're not receiving
//       S-type packets, then you won't be able to use the function.  Therefore,
//       you must either have SWITCHING mode via the 
//       'CMUCAM_set_switching_mode()' function, or you must have streaming
//       S-type packets via 'CMUCAM_get_mean()' function.
//
//       The general approach for when using this function is to _enable
//       switching mode_ and just start tracking with default values, say via
//       'CMUCAM_track()', to collect at least ONE S-type data packet into a
//       'CMUCAM_SDATA' structure.  Then, use a button on the CEENBoT, such that
//       when it is pressed, it will invoke -this- function.
//
//       Upon success, new tracking parameters will be automatically issued
//       and tracking will commence (stream) by internally invoking the
//       'CMUCAM_track_color()' function.
//
//       Users are HIGHLY advised to avoid using this complicated function
//       and simply try out 'CMUCAM_sample_and_track_color()', which really
//       hides the complexity needed to successfully use this function.
extern CMUCAM_CMD_STATUS CMUCAM_learn_and_track_color( 
                        CMUCAM_COLOR *p_color, CMUCAM_COLOR_RANGE *p_crange, 
                        CMUCAM_SDATA *p_sdata_struct, unsigned char window );
// -------------------------------------------------------------------------- //
//
// NOTE: See info on 'CMUCAM_learn_and_track_color()'.
//
//       Users are HIGHLY advised to avoid using this complicated function
//       and simply try out 'CMUCAM_sample_and_track_color()', which really
//       hides the complexity needed to successfully use this function.
extern CMUCAM_CMD_STATUS CMUCAM_learn_and_track_color_YUV(
                        CMUCAM_COLOR *p_color, CMUCAM_COLOR_RANGE *p_crange,
                        CMUCAM_SDATA *p_sdata_struct );
// -------------------------------------------------------------------------- //
// Input Args: None.
// Output Args: None.
// Returns: A value of type 'BOOL' (either 'TRUE' or 'FALSE').  It will be
//          'TRUE' if the function successfully obtained a color from the
//          CMUCAM.
// Desc: This function can be inserted in your program, for example, perhaps
//       triggered when the user presses a button.  It will then internally
//       sample statistical data (obtain an S-type packet) and use the
//       median color sampled, set a small window around this specific color
//       and start tracking that color 'range'.  Users should prefer this
//       method over using 'CMUCAM_learn_and_track_color()' and
//       'CMUCAM_learn_and_track_color_YUV()' as those functions are rather
//       involved, complicated and messy.  This function on the other hand
//       hides all that complexity.
//
//       The sampling is always done in YUV color space, so if the camera
//       is not in this color space (RGB is the default), the camera will
//       switch to it, but it will then revert back to the original color
//       space.
//
//       If the camera was previously streaming frames, the streaming will
//       be suspended.
//
//       Upon completion of color sampled, the camera will immediately begin
//       color tracking (streaming) with the newly selected color.
//
//       The function is built on top of 'CMUCAM_learn_and_track_color_YUV()'.
extern BOOL CMUCAM_sample_and_track_color( void );
// -------------------------------------------------------------------------- //
// FIXME: TEMPORARY. THIS BELONGS IN THE PRIVATE HEADER FILE.  IT IS HERE
//        WHILE SOME THINGS ARE BEING TESTED.
//
// Desc: This function is used to read an internal camera register at the
//       specified address.  The value is placed in the variabled pointed to
//       by 'p_data'.  See the camera's onboard datasheet for further 
//       internal details.  Use this register with caution.
//
//       THIS FUNCTION CAN POTENTIALLY HANG!
//
extern CMUCAM_CMD_STATUS __CMUCAM_read_camera_register( 
    unsigned char reg_addr, unsigned char *p_data );

/* TODO: This doesn't exist yet.
extern CMUCAM_CMD_STATUS __CMUCAM_write_camera_register( 
    unsigned char reg_addr, unsigned char data );
*/
// -------------------------------------------------------------------------- //

// ========================== external declarations ========================= //

extern CMUCAM_PARAMS CMUCAM_params;


#endif /* __CMUCAM324V221_H__ */
