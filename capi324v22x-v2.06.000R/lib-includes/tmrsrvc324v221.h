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
// Desc: Header file which exposes the services offered by the Timer Service 
//       module.  It provides 'operating-system'-like timing services.  
//       Presently, only 16-bit timing objects are supported.
#ifndef __TMRSRVC324v221_H__
#define __TMRSRVC324v221_H__

// ============================== includes ================================== //
#include<avr/io.h>
#include<avr/interrupt.h>
#include<stdlib.h>

#include "utils324v221.h"
#include "sys324v221.h"
#include "isr324v221.h"

// =============================== defines ================================== //
// These define the bit positions for the 'flag' field of the timer
// object structure 'TIMEROBJ'.
#define TMRFLG_FLAGNOTIFY   0x01
#define TMRFLG_FUNCNOTIFY   0x02
#define TMRFLG_NOTIFY_FUNC  TMRFLG_FUNCNOTIFY
#define TMRFLG_NOTIFY_FLAG  TMRFLG_FLAGNOTIFY
#define TMRFLG_RESTART      0x04
#define TMRFLG_ENABLED      0x08

// The following macro is used to allow a function to be invoked only when
// the timer object's 'flag notification' has been modified as a result of
// a terminal count.
#define TMRSRVC_on_TC( tobj, function ) \
                    { if ( tobj.tc ){ function; tobj.tc = 0; } }

// The following macro is an alternative name to the above macro-function
// which hides the fact it deals with a 'tc' flag.
#define TMRSRVC_OnAlarm TMRSRVC_on_TC

// The following macro is used to check if a timer object has reached
// terminal count-down only.
#define TIMER_ALARM( tobj ) ( tobj.tc )? 1 : 0

// The following macro is used to clear the timer's terminal count-down flag.
#define TIMER_SNOOZE( tobj ) ( tobj.tc = 0 )

// The following macro is used for declaring 'timer notify routine' functions,
// also referred to as 'TMR_NR' or (T)i(M)e(R) (N)otify (R)outine.
#define TMR_NR( tmr_nr )    void tmr_nr( void )

// Alternate names.
#define TMR_EVENT( tmr_nr )   TMR_NR( tmr_nr )
#define TIMER_EVENT( tmr_nr ) TMR_NR( tmr_nr )

// The following helper macro can be used to specify a delay in units of 
// 'seconds' instead of 'milliseconds'.  You can even specify 't' values that
// include fractional decimal values.
//
// EX:  TMRSRVC_new( &timer0, TMRFLG_FUNCNOTIFY, TMR_TCM_RUNONCE, 
//                                                          TMR_SECS( 2.45 ) )
//
// The example above starts a timer that runs once for 2.45 secods or 2450 ms.
//
#define TMR_SECS( t )   ( ( TIMER16 ) ( ( t ) * 1000 ) )
#define TIMER_SECS  TMR_SECS

// Desc: The following are macro-function for more 'descriptive' cases.  These
//       are just 'wrappers' around the 'TMRSRVC_delay()' function.
#define TMRSRVC_delay_ms( delay_ms )    TMRSRVC_delay( delay_ms )
#define TMRSRVC_delay_sec( delay_sec )  TMRSRVC_delay( TMR_SECS( delay_sec ) )

// Desc: The following helper macro-function is used to regsiter a 
//       'timer event' function to a 'timer object'.  This way the user
//       doesn't have to know about the internal 'field' structure of the
//       timer object.
#define TMRSRVC_REGISTER_EVENT( timer_obj, timer_event ) \
    {   timer_obj.pNotifyFunc = timer_event; }

// Desc: The following macro-function is used to 'wait' for a timer object
//       to be removed from the list (stopped) before moving on.
#define TMRSRVC_wait_on_stop( which_tobj ) \
    { while( ( ( which_tobj ).flags ) & 0b00010000 ); }

// Desc: The following macro is used to declare a CRC (Call Rate-Controlled)
//       function.  It _MUST_ be declared in global scope!
#define DECLARE_CRC_FUNCTION( __func_name, ... )  \
                                             \
        TIMEROBJ ( __func_name ## _timer );  \
        void __func_name( __VA_ARGS__ );     \

// Desc: The following macro is used to defined a CRC function previously
//       declared via 'DECLARE_CRC_FUNCTION()' macro.
#define DEFINE_CRC_FUNCTION( __func_name, ... )  void __func_name( __VA_ARGS__ )

// Desc: The following macro is used to start the CRC timer associated with
//       the specified function, and the delays between invocations.
#define START_CRC_FUNCTION( __func_name, __delay_ms ) {\
                                                    \
    TMRSRVC_new( &( __func_name ## _timer ), TMRFLG_NOTIFY_FLAG, \
                 TMR_TCM_RESTART, __delay_ms );    }

// Desc: The following macro is used to stop the CRC timer associated with
//       the specified function.
#define STOP_CRC_FUNCTION( __func_name ) {\
                                       \
    TMRSRVC_stop_timer( &( __func_name ## _timer ) ); \
    TMRSRVC_wait_on_stop( __func_name ## _timer );  }

// Desc: The following macro allows the named function to be triggered when
//       the associated CRC timer has reached its terminal countdown.  Otherwise
//       it is skipped.  You must have previously declared (and _DEFINED_) the
//       CRC function via 'DECLARE_CRC_FUNCTION()' macro, and of course, started
//       the associated CRC timer in your code. 'START_CRC_FUNCTION()'.
#define ON_CRC_TIME( __func_name, ... ) {  \
                                           \
    if( ( __func_name ## _timer.tc ) != 0 ) \
    {                                      \
                                           \
        __func_name( __VA_ARGS__ );        \
        __func_name ## _timer.tc = 0;      \
                                           \
    } }

// ============================ declarations ================================ //
// *** Timer Declarations ***
typedef signed char TIMER8;         // 8-bit timer.
typedef signed int  TIMER16;        // 16-bit timer.
typedef signed long TIMER32;        // 32-bit timer.
typedef signed char TMR_FLGS;       // Internal timer flags.

typedef volatile signed char TIMERFLG;       // Timer notify flag.

// Alternate names for the 'TMR_TCMODE' (below).
#define TMRTCM_RUNONCE  TMR_TCM_RUNONCE
#define TMRTCM_RESTART  TMR_TCM_RESTART

// Enumeration type for timer termination (Termincal Count or 'TC') mode.
typedef enum TMR_TCMODE_TYPE 
                        { TMR_TCM_RUNONCE = 0, TMR_TCM_RESTART } TMR_TCMODE;

// Enumeration type for the timer type to use.
typedef enum TMRSIZE_TYPE { TMR8BITS, TMR16BITS, TMR32BITS } TMRSIZE;

// Enumeration type for return codes when a new timer is being created.
typedef enum TMRNEW_RESULT_TYPE 
                { TMRNEW_OK, TMRNEW_ERROR, TMRNEW_OUTOFMEMORY } TMRNEW_RESULT;

// Enumeration type to indicate timer creation result codes.
/* NOTE: Presently not in use.
 *
typedef enum TMRCREATE_RESULT_TYPE 
                    { TMR_CREATE_OK, TMR_CREATE_ERROR } TMRCREATE_RESULT;

*/

// Enumerated type to indicate subsystem result on each 'tick'.
typedef enum TMRTICK_RESULT_TYPE
      { TMRTICK_OK, TMRTICK_ERROR, TMRTICK_BUSY, TMRTICK_STOPPED, 
        TMRTICK_NOT_OPEN } TMRTICK_RESULT;

// Function pointer format for timer notifications.
typedef void ( *TMR_NOTIFY_FUNC_PTR )( void );

// Type declaration for the 16-bit 'timer object'.
typedef struct TMROBJ16_TYPE
{

    unsigned char reserved;         // Pack this structure into 8 bytes.

    TMR_FLGS         flags;         // Internal timer flags:
                                    //  bit0: If '1', 'pFlagNotify' is valid.
                                    //  bit1: If '1', 'pNotifyFunc' is valid.
                                    //  bit2: If '1', timer will be restarted
                                    //                on terminal count.
                                    //  bit3: If '1', timer is ENABLED, if '0',
                                    //                the timer is DISABLED.
                                    //  Note:  An 'ENABLED' timer object will
                                    //         will perform 'notification' 
                                    //         action, while a 'DISABLED' one
                                    //         will not.
                                    //
                                    //  bit4: If '1' Pending removal by
                                    //        'TMRSRVC_stop_timer()'.
                                    //
                                    //  bit5-bit7: RESERVED.


    TIMER16          timeReq;       // Holds original time request.  Needed if
                                    // the counter will be restarted on TC.

    TIMER16          ticks;         // Hold ticks (when initialized), and
                                    // 'delta' (when submitted).

    // TIMERFLG    *pNotifyFlag;       // Notify flag.

    TIMERFLG        tc;             // 'Terminal Count' flag.  When '1', the
                                    // timer has reached terminal count.

    TMR_NOTIFY_FUNC_PTR pNotifyFunc; // Notification callback function pointer.

} TIMEROBJ;

// ============================== prototypes ================================ //
// Input  Args: None.
//
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
// Desc: This function is used to initialize timing services.  It MUST be 
//       invoked before any timing services are used or functions invoked.
//       The timer service is -the- most important subsystem module of the
//       API as many components rely on it for timing purposes.  The timer
//       service relies on the 8-bit Timer 0 of the AVR and requires
//       exclusive use of this hardware resource.  It is configured to
//       generate tick interrupts at approximately 1 millisecond intervals.
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
extern SUBSYS_STATUS TMRSRVC_open( void );
// -------------------------------------------------------------------------- //
// Desc: TODO  (see __TMRSRVC_close()).
extern void TMRSRVC_close( void );
// -------------------------------------------------------------------------- //
// Input  Args: 'pTimerObject' - Pass to this argument **THE ADDRESS** of
//                               a variable of type 'TIMEROBJ'.
//
//              'validNotifyFlags' - An 'OR'-ed list of the following:
//
//                               'TMRFLG_FUNCNOTIFY'
//                               'TMRFLG_FLAGNOTIFY'
//
//                               Example:  
//
//                               To validate both arguments you can do this:
//
//                                  FLG_FLAGNOTIFY | TMRFLG_FUNCNOTIFY
//
//               'tcMode' - Must be one of the following enumerated constants:
//
//                              'TMR_TCM_RUNONCE' - Timer runs once and then
//                                                  expires.
//                              'TMR_TCM_RESTART' - When the timer expires, it
//                                                  is automatically restarted
//                                                  again.
//
//               'nTicks' - The number of ticks to associate the current timer
//                          being created.  The 'unit of time' for each 'tick'
//                          is dependent on implementation, it can be 1ms, 
//                          1us, etc.
//
// Output Args: None.
// Globals  Read: None.
// Globals Write: None.
// Returns: Returns one of the following enumerated constants:
//
//              'TMRNEW_OK' - If the timer request has been granted.
//              'TMRNEW_ERROR' - For generic unknown error.
//              'TMRNEW_OUTOFMEMORY' - If the timer service subsystem can no
//                                     longer allocate memory for the new timer
//                                     object service request.
//
// Desc:    Use this function to submit a new timer object.  The arguments req-
//          quire that you pass **the address** of the timer object to be aff-
//          ected and the terminal count mode.  See 'Input Args' above for
//          an explanation.
//
//          Example:
//          -------
//
//              // Suppose the following timer routines are defined somewhere
//              // in your 'functions' definition section, with the following
//              // corresponding prototyes:
//
//              TMR_NR( timer1_callback );
//              TMR_NR( timer2_callback );
//
//              // Then, somewhere else you *define* these functions.
//
//              TMR_NR( timer1_callback )
//              {
//
//                  ... stuff to do when timer 1 expires ...
//
//              }
//
//              TMR_NR( timer2_callback )
//              {
//
//                  ... stuff to do when timer 2 expires ...
//
//              }
//
//              // Then, somewhere else in your 'main()' code perhaps, you
//              // declare the timer objects as follows.
//
//              TIMEROBJ timer1, timer2;    // These are 'timer objects'.
//              TIMERFLG timer1_flag;       // This is a 'timer update flag'.
//
//
//              // Initialize the objects -- note that only timer 1 uses both
//              // the 'flag notify' feature and the 'function notify' feature.
//
//              timer1.pNotifyFunc = foo;
//              timer1.pFlagNotify = &timer1_flag;
//
//              timer2.pNotifyFunc = bar;
//
//              // Create the timers and start them -- timer1 will run once
//              // every 1 'tick', and timer 2 will run perpetually every
//              // 20 'ticks':
//
//              TMRSRVC_new( &timer1,  TMRFLG_FUNCNOTIFY | TMRFLG_FLAGNOTIFY, 
//                           TMR_TCM_RUNONCE, 1  );
//
//              TMRSRVC_new( &timer2, TMRFLG_FUNCNOTIFY, 
//                           TMR_TCM_RESTART, 20 );
//
//              // That's it.  After 1 'tick', the flag 'timer1_flag' will
//              // be changed to a non-zero state, and then 'timer1_callback()'
//              // function will be called.  Then, 20 'ticks' later, 
//              // 'timer2_callback()' function will be called.
//
//
extern
  TMRNEW_RESULT TMRSRVC_new(  TIMEROBJ *pTimerObject, TMR_FLGS validNotifyFlags,
                              TMR_TCMODE tcMode,      TIMER16 nTicks );
// -------------------------------------------------------------------------- //
// TODO: (finish)
// Desc: This function can be used to CHANGE the time associated with a 
//       PREVIOUSLY created timer object via 'TMRSRVC_new()'.  The new time
//       will take effect on the next TC (terminal count).
extern void TMRSRVC_set_timer( TIMEROBJ *pWhich, TIMER16 nTicks );
// -------------------------------------------------------------------------- //
// Desc:  This function allows a timer that was initially created to always
//        restart upon terminal count down to be changed to 'run once' so that
//        it will eventually terminate on the next terminal countdown.  Simply
//        provide the ADDRESS OF the timer object affected.
extern void TMRSRVC_stop_timer( TIMEROBJ *pWhich );
// -------------------------------------------------------------------------- //
// Input  Args: None.
// Output Args: None.
// Globals  Read: None.
// Globals Write: None.
// Returns: Returns one of the following enumerated constants:
//
//              'TMRTICK_OK'    - Timer service is running 'smoothly' :o).
//              'TMRTICK_ERROR' - Unknown generic error.
//              'TMRTICK_OUTOFMEMORY' - The timer service is no longer able
//                                      to allocate memory.
//
//          The user is highly encouraged to examine the result on each tick.
//          Anything other than 'TMRTICK_OK' means the timing service is
//          unstable and no longer reliable.  Corrective action should be
//          taken to determine the cause of the problem, which under most 
//          circumnstances it is likely to be due to 'not enough memory' issues.
//
// Desc: This function is the 'heart' of the timing service subsystem.  Each
//       time it is invoked, it causes the timing subsystem to cycle through
//       its events.  It should be placed inside of an interrupt service routine
//       that gets invoked by the CPU/MCU hardware.  The periodicity of the
//       timing interrupt of the CPU/MCU determines how often the 'ticks' occur.
//       This timing is entirely determined by the software developer and how
//       the MCU/CPU is configured.
extern TMRTICK_RESULT TMRSRVC_tick( void );
// -------------------------------------------------------------------------- //
// Desc: This function performs the typical 'delay' function in milliseconds.
//       You can specify up to 32766 milliseconds (approximately 32 seconds )
//       The function will 'busy-wait' until the specified delay has elapsed.
extern TMRNEW_RESULT TMRSRVC_delay( TIMER16 delay_ms );
// -------------------------------------------------------------------------- //
// FIXME: At this point, so much of the API relies on the timer service that
//        this function should be private.  The Timer Service is integral to
//        many aspects on how the API operates such that casual stopping of
//        the service should not be allowed.
//
// Desc:  This function can be used to start the timer subsystem in software
//        if stopped by 'TMRSRVC_stop()' call.
extern void TMRSRVC_start( void );
// -------------------------------------------------------------------------- //
// FIXME: At this point, so much of the API relies on the timer service that
//        this function should be private.  The Timer Service is integral to
//        many aspects on how the API operates such that casual stopping of
//        the service should not be allowed.
//
// Desc:  This function can be used to stop the timer subsystem in software
//        if currently running.
extern void TMRSRVC_stop( void );

#endif /* __TMRSRVC324v221_H__ */
