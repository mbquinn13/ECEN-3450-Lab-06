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
// Desc: Header file for the Behavior-Based Toolkit.

#ifndef __BBTK324V221_H__
#define __BBTK324V221_H__

#include "utils324v221.h"
#include "tmrsrvc324v221.h"
#include "lcd324v221.h"

// =============================== defines ================================== //
// Desc: The following macro is used for declaring behavior states.
#define DECLARE_BEHAVIOR_STATE( state_name ) \
    volatile const BEHAVIOR_STATE state_name = (unsigned char)( __LINE__ )

// Desc: The following macro starts the behavior main event loop.
#define RUN_BEHAVIORS( behavior_list, total_behaviors ) \
    BBTK_process_behaviors( behavior_list, total_behaviors )

// Desc: The following macro functions are used for declaring and defining
//       task functions for various behavior types.
#define BEHAVIOR_START( func_name ) BEHAVIOR_START_RESULT func_name( void )
#define BEHAVIOR_SENSE( func_name ) \
    BEHAVIOR_SENSE_RESULT func_name( BEHAVIOR_SENSE_STRUCT *pSense )

#define BEHAVIOR_PROCESS( func_name ) \
    BEHAVIOR_PROCESS_RESULT func_name( BEHAVIOR_SENSE_STRUCT *pSense, \
                                       BEHAVIOR_ACTION_STRUCT *pAction )

#define BEHAVIOR_BALLISTIC( func_name ) \
    void func_name( BEHAVIOR_ACTION_STRUCT *pAction )

#define BEHAVIOR_ACTION( func_name ) \
    void func_name( BEHAVIOR_ACTION_STRUCT *pAction )

// Desc: The following macros are for setting various parameters of
//       the behavior.
#define SET_BEHAVIOR_STATE( b, s ) \
    ( b ).state = s

#define SET_BEHAVIOR_START( b, start_func ) \
    ( b ).start = start_func

#define SET_BEHAVIOR_SENSE( b, sense_func ) \
    ( b ).sense = sense_func

#define SET_BEHAVIOR_PROCESS( b, proc_func ) \
    ( b ).process = proc_func

#define SET_BEHAVIOR_ACTION( b, action_func ) \
    ( b ).action = action_func

#define SET_BEHAVIOR_BALLISTIC( b, ballistic_func ) \
    ( b ).ballistic = ballistic_func

#define SET_BEHAVIOR_INTERVAL( b, interval_ms ) \
    ( b ).sense_interval_ms = interval_ms

#define SET_BEHAVIOR_SYNC( b, behavior_state ) \
    ( b ).sync = behavior_state

#define SET_BEHAVIOR_TYPE( b, behavior_type ) \
    ( b ).type = behavior_type

#define SET_BEHAVIOR_DISPLAY( b, const_string ) \
    ( b ).display =  const_string

#define SET_BEHAVIOR_ACTION_STRUCT( b, action_struct_name ) \
    ( b ).pAction = & action_struct_name

#define SET_BEHAVIOR_SENSE_STRUCT( b, sense_struct_name ) \
    ( b ).pSense = & sense_struct_name

// Desc: The following macro is used for declaring a behavior.  These must
//       be declared using global scope.
#define DECLARE_BEHAVIOR( behavior_name ) \
    BEHAVIOR behavior_name;           /* Declare the behavior */ \
    TIMEROBJ  behavior_name ## _timer /* Declare a timer object */

#define INITIALIZE_BEHAVIOR( behavior_name ) \
    BBTK_initialize_default_behavior( &behavior_name, &behavior_name ## _timer )

// Desc: The following macro is used to declare a behavior arbiter.
#define DECLARE_BEHAVIOR_ARBITER( arbiter_name, behavior_count ) \
    BEHAVIOR * arbiter_name[ behavior_count ]

// Desc: The following macro attaches a behavior to the arbiter.
#define ATTACH_BEHAVIOR( arbiter_name, behavior_slot, behavior ) \
    arbiter_name[ behavior_slot ] = &( behavior )

// Desc: The following macro is for declaring sense data structures
//       along with their member data.
#define SENSE_DATA_BEGIN( sense_struct_name ) \
    struct BEHAVIOR_SENSE_STRUCT_type {

#define SENSE_DATA_END( sense_struct_name ) } sense_struct_name;

// Desc: The following macro is for declaring action data structures
//       along with their member data.
#define ACTION_DATA_BEGIN( action_struct_name ) \
    struct BEHAVIOR_ACTION_STRUCT_type {

#define ACTION_DATA_END( action_struct_name ) } action_struct_name;
// ============================ type declarations =========================== //
// Desc: The following type definition is for behavior states.
typedef unsigned char BEHAVIOR_STATE;

// Desc: The following enumerated type definition is for specifying the
//       return value for a behavior process function.
typedef enum BEHAVIOR_RESULT_type {

    BEHAVIOR_PROCESS_OK = 0,
    BEHAVIOR_PROCESS_BALLISTIC,
    BEHAVIOR_PROCESS_NOTHING_TO_DO,
    BEHAVIOR_PROCESS_ERROR

} BEHAVIOR_PROCESS_RESULT;

// Desc: The following enumerated type definition is for specifying the
//       return value for 'start()' functions that are part of a behavior.
typedef enum BEHAVIOR_START_RESULT_type {

    BEHAVIOR_START_OK = 0,
    BEHAVIOR_START_ERROR

} BEHAVIOR_START_RESULT;

// Desc: The following enumerated type definition is for specifying
//       the return value for the 'sense()' functions that are part of
//       a behavior.
typedef enum BEHAVIOR_SENSE_RESULT_type {

    BEHAVIOR_SENSE_OK = 0,
    BEHAVIOR_SENSE_ERROR

} BEHAVIOR_SENSE_RESULT;

// Desc: The following are structure forward declarations whose full
//       definition must be provided by the user!
typedef struct BEHAVIOR_SENSE_STRUCT_type  BEHAVIOR_SENSE_STRUCT;
typedef struct BEHAVIOR_ACTION_STRUCT_type BEHAVIOR_ACTION_STRUCT;

// Desc: The following type definitions is for specifying function pointers.
typedef BEHAVIOR_START_RESULT (*BEHAVIOR_START_FUNCTION_PTR)( void );

typedef BEHAVIOR_SENSE_RESULT (*BEHAVIOR_SENSE_FUNCTION_PTR)( 
                                        BEHAVIOR_SENSE_STRUCT *pSense );

typedef BEHAVIOR_PROCESS_RESULT (*BEHAVIOR_PROCESS_FUNCTION_PTR)(
    BEHAVIOR_SENSE_STRUCT *pSense, BEHAVIOR_ACTION_STRUCT *pAction );

typedef void (*BEHAVIOR_BALLISTIC_FUNCTION_PTR)( 
                                            BEHAVIOR_ACTION_STRUCT *pAction );

typedef void (*BEHAVIOR_ACTION_FUNCTION_PTR)( BEHAVIOR_ACTION_STRUCT *pAction );


// Desc: The following enumerated type definition is for specifying the
//       behavior types for when a behavior is first created.
typedef enum BEHAVIOR_TYPE_type {

    BEHAVIOR_STANDARD = 0,
    BEHAVIOR_BALLISTIC

} BEHAVIOR_TYPE;


// Desc: The following structure declaration encapsulates the concept of
//       a `behavior' within the toolkit.
typedef struct __attribute__((packed)) BEHAVIOR_type{

    BOOL                                started;
    BEHAVIOR_STATE                      state;
    BEHAVIOR_START_FUNCTION_PTR         start;
    BEHAVIOR_SENSE_FUNCTION_PTR         sense;
    BEHAVIOR_PROCESS_FUNCTION_PTR       process;
    BEHAVIOR_ACTION_FUNCTION_PTR        action;
    BEHAVIOR_BALLISTIC_FUNCTION_PTR     ballistic;
    BEHAVIOR_SENSE_STRUCT               *pSense;
    BEHAVIOR_ACTION_STRUCT              *pAction;
    unsigned short int                  sense_interval_ms;
    TIMEROBJ                            *pTimerObj;
    BOOL                                sync_process_to_sense;
    BEHAVIOR_TYPE                       type;
    char                                *display;

} BEHAVIOR;
// ============================== prototypes ================================ //
extern void BBTK_process_behaviors( BEHAVIOR *behaviors[],
                                    unsigned short int total_behaviors );
// -------------------------------------------------------------------------- //
extern void BBTK_display( const char *str );
// -------------------------------------------------------------------------- //
extern void BBTK_initialize_default_behavior( BEHAVIOR *behavior,
                                              TIMEROBJ *pTimerObj );
// =============================== globals ================================== //
// Desc: These are predefined behavior states.
extern volatile const BEHAVIOR_STATE BHVR_ST_STARTUP;
extern volatile const BEHAVIOR_STATE BHVR_ST_IDLE;

#endif /* __BBTK324V221_H__ */
