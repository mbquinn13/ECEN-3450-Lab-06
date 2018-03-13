//===============================================================================
//= Authors:	Michael Quinn, Collin Peterson.									=
//= Course:		ECEN 3450 - Mobile Robotics.									=
//= Assignment:	Laboratory 06 - Light Homing.									=
//= Due Date:	03/16/18														=
//= File Name:	ir_behaviors.c													=
//= Desc:		Contains the behaviors relating to the IR sensors.				=
//= Functions:	IR_sense(), IR_avoid()											=
//= Other:		none.															=
//===============================================================================

//===============================================================================
//= What:	Include file "ECEN3450Lab06.h" which has prototypes and includes.	=
//===============================================================================
#include "ECEN3450Lab06.h"

//===============================================================================
//= What:	IR_sense()															=
//= Why:	Behavior to read IR sensor values.									=
//= Desc:	Starts the timer service if it hasn't been yet, otherwise it will	=
//=			read from the IR sensors every "interval_ms".						=
//= Return:	void.																=
//= Params:	volatile SENSOR_DATA *pSensors (the pointer for all sensor data)	=
//=			TIMER16 interval_ms (number of ms between checks of the sensor)		=
//= Notes:	none.																=
//===============================================================================
void IR_sense( volatile SENSOR_DATA *pSensors, TIMER16 interval_ms )
{
	// Sense must know if it's already sensing.
	//
	// NOTE: 'BOOL' is a custom data type offered by the CEENBoT API.
	//
	static BOOL timer_started = FALSE;
	
	// The 'sense' timer is used to control how often gathering sensor
	// data takes place.  The pace at which this happens needs to be
	// controlled.  So we're forced to use TIMER OBJECTS along with the
	// TIMER SERVICE.  It must be 'static' because the timer object must remain
	// 'alive' even when it is out of scope -- otherwise the program will crash.
	static TIMEROBJ sense_timer;
	
	// If this is the FIRST time that sense() is running, we need to start the
	// sense timer.  We do this ONLY ONCE!
	if ( timer_started == FALSE )
	{
		// Start the 'sense timer' to tick on every 'interval_ms'.
		//
		// NOTE:  You can adjust the delay value to suit your needs.
		//
		TMRSRVC_new( &sense_timer, TMRFLG_NOTIFY_FLAG, TMRTCM_RESTART,
		interval_ms );
		
		// Mark that the timer has already been started.
		timer_started = TRUE;
	} // end if()
	
	// Otherwise, just do the usual thing and just 'sense'.
	else
	{
		// Only read the sensors when it is time to do so (e.g., every
		// 125ms).  Otherwise, do nothing.
		if ( TIMER_ALARM( sense_timer ) )
		{
			// NOTE: Just as a 'debugging' feature, let's also toggle the green LED
			//       to know that this is working for sure.  The LED will only
			//       toggle when 'it's time'.
			LED_toggle( LED_Green );

			// Read the left and right sensors, and store this
			// data in the 'SENSOR_DATA' structure.
			pSensors->left_IR  = ATTINY_get_IR_state( ATTINY_IR_LEFT  );
			pSensors->right_IR = ATTINY_get_IR_state( ATTINY_IR_RIGHT );
			
			// NOTE: You can add more stuff to 'sense' here.
			
			// Snooze the alarm so it can trigger again.
			TIMER_SNOOZE( sense_timer );
		} // end if()
	} // end else.
} // end IR_sense()


//===============================================================================
//= What:	IR_avoid()															=
//= Why:	Behavior to ballistically stop CEENBoT, reverse, and turn it.		=
//= Desc:	If only left sensor is tripped, rotate right ~90-deg.				=
//=			If only right sensor is tripped, rotate left ~90-deg.				=
//=			If the left and right sensors are tripped, rotate right ~180-deg.	=
//= Return:	void.																=
//= Params:	volatile MOTOR_ACTION *pAction (the pointer for all motor actions)	=
//=			volatile SENSOR_DATA *pSensors (the pointer for all sensor data)	=
//= Notes:	none.																=
//===============================================================================
void IR_avoid( volatile MOTOR_ACTION *pAction, volatile SENSOR_DATA *pSensors )
{
	// NOTE: Here we have NO CHOICE, but to do this 'ballistically'.
	//       **NOTHING** else can happen while we're 'avoiding'.
	
	// If only the LEFT sensor tripped...
	if( pSensors->left_IR == TRUE && pSensors->right_IR == FALSE)
	{
		// Note that we're avoiding...
		pAction->state = AVOIDING;
		
		// STOP!
		STEPPER_stop( STEPPER_BOTH, STEPPER_BRK_OFF );
		
		// Back up...
		STEPPER_move_stwt( STEPPER_BOTH,
		STEPPER_REV, 150, 200, 400, STEPPER_BRK_OFF,
		STEPPER_REV, 150, 200, 400, STEPPER_BRK_OFF );
		
		// ... and turn RIGHT ~90-deg.
		STEPPER_move_stwt( STEPPER_BOTH,
		STEPPER_FWD, DEG_90, 200, 400, STEPPER_BRK_OFF,
		STEPPER_REV, DEG_90, 200, 400, STEPPER_BRK_OFF );

		// ... and set the motor action structure with variables to move forward.
		pAction->speed_L = 200;
		pAction->speed_R = 200;
		pAction->accel_L = 400;
		pAction->accel_R = 400;
	} // end if()
	else if( pSensors->left_IR == FALSE && pSensors->right_IR == TRUE )
	{
		// Note that we're avoiding...
		pAction->state = AVOIDING;

		// STOP!
		STEPPER_stop( STEPPER_BOTH, STEPPER_BRK_OFF );
		
		// Back up...
		STEPPER_move_stwt( STEPPER_BOTH,
		STEPPER_REV, 150, 200, 400, STEPPER_BRK_OFF,
		STEPPER_REV, 150, 200, 400, STEPPER_BRK_OFF );
		
		// ... and turn LEFT ~90-deg.
		STEPPER_move_stwt( STEPPER_BOTH,
		STEPPER_REV, DEG_90, 200, 400, STEPPER_BRK_OFF,
		STEPPER_FWD, DEG_90, 200, 400, STEPPER_BRK_OFF );

		// ... and set the motor action structure with variables to move forward.
		pAction->speed_L = 200;
		pAction->speed_R = 200;
		pAction->accel_L = 400;
		pAction->accel_R = 400;
	}
	else if( pSensors->left_IR == TRUE && pSensors->right_IR == TRUE )
	{
		// Note that we're avoiding...
		pAction->state = AVOIDING;

		// STOP!
		STEPPER_stop( STEPPER_BOTH, STEPPER_BRK_OFF );
		
		// Back up...
		STEPPER_move_stwt( STEPPER_BOTH,
		STEPPER_REV, 150, 200, 400, STEPPER_BRK_OFF,
		STEPPER_REV, 150, 200, 400, STEPPER_BRK_OFF );
		
		// ... and turn RIGHT ~180-deg.
		STEPPER_move_stwt( STEPPER_BOTH,
		STEPPER_REV, DEG_90*2, 200, 400, STEPPER_BRK_OFF,
		STEPPER_FWD, DEG_90*2, 200, 400, STEPPER_BRK_OFF );

		// ... and set the motor action structure with variables to move forward.
		pAction->speed_L = 200;
		pAction->speed_R = 200;
		pAction->accel_L = 400;
		pAction->accel_R = 400;
	}
} // end avoid()
