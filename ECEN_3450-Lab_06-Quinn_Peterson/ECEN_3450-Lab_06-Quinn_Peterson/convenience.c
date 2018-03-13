//===============================================================================
//= Authors:	Michael Quinn, Collin Peterson.									=
//= Course:		ECEN 3450 - Mobile Robotics.									=
//= Assignment:	Laboratory 06 - Light Homing.									=
//= Due Date:	03/16/18														=
//= File Name:	convenience.c													=
//= Desc:		Miscellaneous functions that don't really fit elsewhere.		=
//= Functions:	act(), open_modules(), info_display(), compare_actions()		=
//= Other:		none.															=
//===============================================================================

//===============================================================================
//= What:	Include file "ECEN3450Lab06.h" which has prototypes and includes.	=
//===============================================================================
#include "ECEN3450Lab06.h"

//===============================================================================
//= What:	act()																=
//= Why:	Uses the *pAction values to set the speed of the motors.			=
//= Desc:	If the current state of CEENBoT is same as the previous state, then	=
//=			it won't do anything, otherwise it will set the motors to *pAction.	=
//= Return:	void.																=
//= Params:	volatile MOTOR_ACTION *pAction (the pointer for all motor actions)	=
//= Notes:	none.																=
//===============================================================================
void act( volatile MOTOR_ACTION *pAction )
{
	// 'act()' always keeps track of the PREVIOUS action to determine
	// if a new action must be executed, and to execute such action ONLY
	// if any parameters in the 'MOTOR_ACTION' structure have changed.
	// This is necessary to prevent motor 'jitter'.
	static MOTOR_ACTION previous_action = {
		STARTUP, 0, 0, 0, 0
	};

	if( compare_actions( pAction, &previous_action ) == FALSE )
	{
		// Perform the action.  Just call the 'free-running' version
		// of stepper move function and feed these same parameters.
		__MOTOR_ACTION( *pAction );

		// Save the previous action.
		previous_action = *pAction;
	} // end if()
} // end act()

//===============================================================================
//= What:	open_modules()														=
//= Why:	Opens all modules in once simple function.							=
//= Desc:	LEDs (opens), LCD (opens, then clears), Steppers (opens),			=
//=			ADC (opens, waits 400 ms to initialize, sets reference to 5V).		=
//= Return:	void.																=
//= Params:	void.																=
//= Notes:	none.																=
//===============================================================================
void open_modules( void )
{
	// Opening LEDs
	LED_open();
	
	// Opening & clearing LCD
	LCD_open();
	LCD_clear();
	
	// Opening Steppers
	STEPPER_open();
	
	// Opening & Initializing ADCs
	ADC_open();
	DELAY_ms(400);
	// set ADC reference to 5V
	ADC_set_VREF(ADC_VREF_AVCC);
} // end open_modules

//===============================================================================
//= What:	info_display()														=
//= Why:	Display current state to LCD.										=
//= Desc:	If not in the startup state, and the previous state is different	=
//=			than the current state, then print (prevents screen flicker).		=
//= Return:	void.																=
//= Params:	volatile MOTOR_ACTION *pAction (the pointer for all motor actions)	=
//= Notes:	none.																=
//===============================================================================
void info_display( volatile MOTOR_ACTION *pAction )
{
	// NOTE:  We keep track of the 'previous' state to prevent the LCD
	//        display from being needlessly written, if there's  nothing
	//        new to display.  Otherwise, the screen will 'flicker' from
	//        too many writes.
	static ROBOT_STATE previous_state = STARTUP;

	if ( ( pAction->state != previous_state ) || ( pAction->state == STARTUP ) )
	{
		LCD_clear();

		//  Display information based on the current 'ROBOT STATE'.
		switch( pAction->state )
		{
			case STARTUP:
			LCD_printf("Let me wake up\nplease...\n");
			break;

			case EXPLORING:
			LCD_printf( "Exploring...\n" );
			break;

			case AVOIDING:
			LCD_printf("GET OUT CHALLENGE!!!\n");
			break;
			
			case LIGHT_FOLLOW:
			LCD_printf("Go to the light,\nJerry...");
			break;
			
//			case LIGHT_OBSERVE:
//			LCD_printf("Stay away from the\nlight, Icarus...");
//			break;

			default:
			LCD_printf( "Unknown state!\n" );
			break;
		} // end switch()

		// Note the new state in effect.
		previous_state = pAction->state;
	} // end if()
} // end info_display()

//===============================================================================
//= What:	compare_actions()													=
//= Why:	Checks if previous state is the same as the current state.			=
//= Desc:	If any actions (*a and *b) are the same return TRUE, else FALSE.	=
//= Return:	BOOL.																=
//= Params:	volatile MOTOR_ACTION *a (the pointer for all motor actions)		=
//=			volatile MOTOR_ACTION *b (the pointer for all motor actions)		=
//= Notes:	none.																=
//===============================================================================
BOOL compare_actions( volatile MOTOR_ACTION *a, volatile MOTOR_ACTION *b )
{
	// NOTE:  The 'sole' purpose of this function is to
	//        compare the 'elements' of MOTOR_ACTION structures
	//        'a' and 'b' and see if 'any' differ.

	// Assume these actions are equal.
	BOOL rval = TRUE;

	if (
	( a->state   != b->state )   ||
	( a->speed_L != b->speed_L ) ||
	( a->speed_R != b->speed_R ) ||
	( a->accel_L != b->accel_L ) ||
	( a->accel_R != b->accel_R ) )

	rval = FALSE;

	// Return comparison result.
	return rval;
} // end compare_actions()