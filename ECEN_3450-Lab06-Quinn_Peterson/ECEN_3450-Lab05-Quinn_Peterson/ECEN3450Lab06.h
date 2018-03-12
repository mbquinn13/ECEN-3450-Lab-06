//===============================================================================
//= Authors:	Michael Quinn, Collin Peterson.									=
//= Course:		ECEN 3450 - Mobile Robotics.									=
//= Assignment:	Laboratory 06 - Light Homing.									=
//= Due Date:	03/16/18														=
//= File Name:	ECEN3450Lab06.h													=
//= Desc:		The defines, enums, structs, and prototypes of entire program.	=
//= Functions:	none.															=
//= Other:		none.															=
//===============================================================================

//===============================================================================
//= What:	Include file "capi324v221.h" for the CEENBoT API header.			=
//= Why:	Prevents ECEN3450Lab06.h from being implemented more than once.		=
//===============================================================================
#ifndef __ECEN3450Lab06_H__
#define __ECEN3450Lab06_H__

//===============================================================================
//= What:	Include file "capi324v221.h" for the CEENBoT API header.			=
//= Why:	Put here so that each file doesn't need to implement it.			=
//===============================================================================
#include "capi324v221.h"

//===============================================================================
//= What:	Defines.															=
//= Why:	definitions for main.c (single line first, then multi-line).		=
//===============================================================================
#define DEG_90  150					// Number of steps for a 90-degree (in place) turn.
#define ultrasonic_pin	ADC_CHAN3	// Set the ultrasonic sensor to channel 3	(J3, Pin 1)
#define right_pr_channel ADC_CHAN4	// Set the right photoresistor to channel 4	(J3, Pin 2)
#define left_pr_channel ADC_CHAN5	// Set the left photoresistor to channel 5	(J3, Pin 3)
// channel 6 (J3, Pin 4)
// channel 7 (J3, Pin 5)
#define LCD_Row_PR_L 1				// Left photoresistor value will be on row 1 of LCD
#define LCD_Row_PR_R 0				// Right photoresistor value will be on row 0 of LCD

// Desc: This macro-function can be used to reset a motor-action structure
//       easily.  It is a helper macro-function.
#define __RESET_ACTION( motor_action )	\
do {									\
	( motor_action ).speed_L = 0;		\
	( motor_action ).speed_R = 0;		\
	( motor_action ).accel_L = 0;		\
	( motor_action ).accel_R = 0;		\
	( motor_action ).state = STARTUP;	\
	} while( 0 ) /* end __RESET_ACTION() */

// Desc: This macro-function translates action to motion -- it is a helper
//       macro-function.
#define __MOTOR_ACTION( motor_action )											\
do {																			\
	STEPPER_set_accel2( ( motor_action ).accel_L, ( motor_action ).accel_R );	\
	STEPPER_runn( ( motor_action ).speed_L, ( motor_action ).speed_R );			\
	} while( 0 ) /* end __MOTOR_ACTION() */

//===============================================================================
//= What:	Type Declarations.													=
//= Why:	enums, and structures for main.c.									=
//===============================================================================

// Desc: The following custom enumerated type can be used to specify the
//       current state of the robot.  This parameter can be expanded upon
//       as complexity grows without interfering with the 'act()' function.
//		 It is a new type which can take the values of 0, 1, or 2 using
//		 the SYMBOLIC representations of STARTUP, EXPLORING, etc.
typedef enum ROBOT_STATE_TYPE {
	STARTUP = 0,	// 'Startup' = 0		state -- initial state upon RESET.
	EXPLORING,		// 'Exploring' = 1		state -- the robot is 'roaming around'.
	LIGHT_FOLLOW,	// 'Light Follow" = 2	state -- the robot is following a light.
//	LIGHT_OBSERVE,	// 'Light Observe" = 3	state -- the robot is stopping at a light.
	AVOIDING		// 'Avoiding' = 4		state -- the robot is avoiding a collision.
} ROBOT_STATE;

// Desc: Structure encapsulates a 'motor' action. It contains parameters that
//       controls the motors 'down the line' with information depicting the
//       current state of the robot.  The 'state' variable is useful to
//       'print' information on the LCD based on the current 'state', for
//       example.
typedef struct MOTOR_ACTION_TYPE {
	ROBOT_STATE state;              // Holds the current STATE of the robot.
	signed short int speed_L;       // SPEED for LEFT  motor.
	signed short int speed_R;       // SPEED for RIGHT motor.
	unsigned short int accel_L;     // ACCELERATION for LEFT  motor.
	unsigned short int accel_R;     // ACCELERATION for RIGHT motor.
} MOTOR_ACTION;

// Desc: Structure encapsulates 'sensed' data.  Right now that only consists
//       of the state of the left & right IR sensors when queried.  You can
//       expand this structure and add additional custom fields as needed.
typedef struct SENSOR_DATA_TYPE {
	BOOL left_IR;				// Holds the state of the left IR.
	BOOL right_IR;				// Holds the state of the right IR.
	unsigned int left_PR;		// Holds the voltage of the left photo resistor.
	unsigned int right_PR;		// Holds the voltage of the right photo resistor.
	unsigned int PR_delta_LR;	// Holds the voltage difference of the right pr - left pr.
} SENSOR_DATA;


//===============================================================================
//= What:	Prototypes.															=
//= Why:	When functions are made in C, it is best practice to explicitly		=
//=			define the function, known as a prototype.							=
//= Notes:	Listed in order of appearance in code (CBOT_main not included).		=
//===============================================================================

// Contained in convenience.c
void act( volatile MOTOR_ACTION *pAction );
void open_modules( void );
void info_display( volatile MOTOR_ACTION *pAction );
BOOL compare_actions( volatile MOTOR_ACTION *a, volatile MOTOR_ACTION *b );

// Contained in explore.c
void explore( volatile MOTOR_ACTION *pAction );

// Contained in ir_behaviors.c
void IR_sense( volatile SENSOR_DATA *pSensors, TIMER16 interval_ms );
void IR_avoid( volatile MOTOR_ACTION *pAction, volatile SENSOR_DATA *pSensors );

// Contained in pr_behaviors.c
void calibrate_pr( volatile SENSOR_DATA *pSensors );
void get_PR_diff( volatile SENSOR_DATA *pSensors );
void PR_sense ( volatile SENSOR_DATA *pSensors, TIMER16 interval_ms );
void light_follow ( volatile MOTOR_ACTION *pAction, volatile SENSOR_DATA *pSensors );
void light_observe ( volatile MOTOR_ACTION *pAction, volatile SENSOR_DATA *pSensors );

#endif // __ECEN3450Lab06_H__