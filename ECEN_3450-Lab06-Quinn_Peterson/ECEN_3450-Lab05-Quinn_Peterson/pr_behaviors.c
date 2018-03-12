//===============================================================================
//= Authors:	Michael Quinn, Collin Peterson.									=
//= Course:		ECEN 3450 - Mobile Robotics.									=
//= Assignment:	Laboratory 06 - Light Homing.									=
//= Due Date:	03/16/18														=
//= File Name:	pr_behaviors.c													=
//= Desc:		Contains the behaviors relating to the photoresistors.			=
//= Functions:	get_PR_diff(), PR_sense(), light_follow(), light_observe()		=
//= Other:		none.															=
//===============================================================================

//===============================================================================
//= What:	Include file "ECEN3450Lab06.h" which has prototypes and includes.	=
//===============================================================================
#include "ECEN3450Lab06.h"

//===============================================================================
//= What:	calibrate_pr()														=
//= Why:	Calibrate the photoresistors in case the values aren't even.		=
//= Desc:	Wait for Switch 3 to be pressed, then calibrate the sensors.		=
//= Return:	void.																=
//= Params:	volatile SENSOR_DATA *pSensors (the pointer for all sensor data)	=
//= Notes:	none.																=
//===============================================================================
void calibrate_pr( volatile SENSOR_DATA *pSensors )
{
	// Get the calibration settings (PR_delta_RL)
	int switch_bool = 1;
	LCD_clear();
	LCD_printf("ECEN 3450: Mob Rob\nLab5: Light Homing\nQuinn & Peterson\nSW3: Calibrate");
	while(switch_bool == 1)
	{
		if(ATTINY_get_SW_state(ATTINY_SW3))
		{
			DELAY_ms(400);
			get_PR_diff( pSensors );
			switch_bool = 0;
		}
	}
}

//===============================================================================
//= What:	get_PR_diff()														=
//= Why:	In case the photoresistors aren't balanced, this finds the			=
//=			difference so that it can be added to smaller value.				=
//= Desc:	Reads from both photoresistors, and stores delta (left - right).	=
//= Return:	void.																=
//= Params:	volatile SENSOR_DATA *pSensors (the pointer for all sensor data)	=
//= Notes:	none.																=
//===============================================================================
void get_PR_diff( volatile SENSOR_DATA *pSensors )
{
	// Get
	ADC_set_channel(left_pr_channel);
	pSensors->left_PR  = ADC_sample();
	
	ADC_set_channel(right_pr_channel);
	pSensors->right_PR = ADC_sample();
	
	pSensors->PR_delta_LR = (pSensors->left_PR - pSensors->right_PR);
}

//===============================================================================
//= What:	PR_sense()															=
//= Why:	Behavior to read photoresistor values.								=
//= Desc:	Starts the timer service if it hasn't been yet, otherwise it will	=
//=			read from the photoresistors every "interval_ms".					=
//= Return:	void.																=
//= Params:	volatile SENSOR_DATA *pSensors (the pointer for all sensor data)	=
//=			TIMER16 interval_ms (number of ms between checks of the sensor)		=
//= Notes:	none.																=
//===============================================================================
void PR_sense( volatile SENSOR_DATA *pSensors, TIMER16 interval_ms )
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
			LED_toggle( LED_Red );
			
			ADC_set_channel(left_pr_channel);
			pSensors->left_PR  = ADC_sample();
			
			ADC_set_channel(right_pr_channel);
			pSensors->right_PR = ADC_sample();

			// Snooze the alarm so it can trigger again.
			TIMER_SNOOZE( sense_timer );
		} // end if()
	} // end else.
} // end PR_sense()

//===============================================================================
//= What:	light_follow()														=
//= Why:	Behavior to read ballistically stop CEENBoT, reverse, and turn it.	=
//= Desc:	
//= Return:	void.																=
//= Params:	volatile MOTOR_ACTION *pAction (the pointer for all motor actions)	=
//=			volatile SENSOR_DATA *pSensors (the pointer for all sensor data)	=
//= Notes:	none.																=
//===============================================================================
void light_follow(volatile MOTOR_ACTION *pAction, volatile SENSOR_DATA *pSensors)
{
	// Get voltage from sensors.
	float Lv = ( ( ( pSensors->left_PR  ) * 5.0f ) / 1024 );	// L voltage and L sensor level
	float Rv = ( ( ( pSensors->right_PR ) * 5.0f ) / 1024 );	// R voltage and R sensor level
	
	// Average
	float average = (Rv + Lv) / 2.0f;
	float diff_LR = ( Lv - Rv );
		
	// If threshold (average) is hit, then light follow, else default.
	if(	( average > 0.9f && average < 4.3f ) &&
		( diff_LR > 0.5f || diff_LR < -0.5f ) )
	{
		// Set motor action and display values
		pAction->state = LIGHT_FOLLOW;
		LCD_printf_RC(LCD_Row_PR_L, 0, "Left  PR: %f", Lv);
		LCD_printf_RC(LCD_Row_PR_R, 0, "Right PR: %f", Rv);
		
		// More light on left, Left > Right
		// Right is speed up, and delta added to right
		if( diff_LR >= 0.5f )
		{
			pAction->speed_L = Lv*50;
			pAction->speed_R = Rv*200 + ( pSensors->PR_delta_LR );
		}
		// Left < Right
		// Left is speed up, and delta (which is negative) is subtracted from left
		else 
		{
			pAction->speed_L = Lv*200 - ( pSensors->PR_delta_LR );
			pAction->speed_R = Rv*50;
		}
	}
} // end light_follow()

/*
//===============================================================================
//= What:	light_observe()														=
//= Why:	Behavior to read ballistically stop CEENBoT, reverse, and turn it.	=
//= Desc:	
//= Return:	void.																=
//= Params:	volatile MOTOR_ACTION *pAction (the pointer for all motor actions)	=
//=			volatile SENSOR_DATA *pSensors (the pointer for all sensor data)	=
//= Notes:	none.																=
//===============================================================================
void light_observe(volatile MOTOR_ACTION *pAction, volatile SENSOR_DATA *pSensors)
{
	// Get voltage from sensors.
	float Lv = ( ( ( pSensors->left_PR  ) * 5.0f ) / 1024 );	// L voltage and L sensor level
	float Rv = ( ( ( pSensors->right_PR ) * 5.0f ) / 1024 );	// R voltage and R sensor level
	
	// Average
	float average = (Rv + Lv) / 2.0f;
	float diff_LR = ( Lv - Rv );
	
	// If threshold (average) is hit, then light follow, else default.
	if(	( average > 0.9f && average < 4.3f ) &&
	( diff_LR > 0.5f || diff_LR < -0.5f ) )
	{
		// Set motor action and display values
		pAction ->state = LIGHT_OBSERVE;
		LCD_printf_RC(LCD_Row_PR_L, 0, "Left  PR: %f", Lv);
		LCD_printf_RC(LCD_Row_PR_R, 0, "Right PR: %f", Rv);
		
		// Left > Right
		// Right is speed up, and delta added to right
		if( diff_LR >= 0.5f )
		{
			pAction->speed_L = Lv*100;
			pAction->speed_R = Rv*50 + ( pSensors->PR_delta_LR );
		}
		// Left < Right
		// Left is speed up, and delta (which is negative) is subtracted from left
		else
		{
			pAction->speed_L = Lv*50 - ( pSensors->PR_delta_LR );
			pAction->speed_R = Rv*100;
		}
	}
} // end light_observe()
*/