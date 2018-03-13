//===============================================================================
//= README FOR THE TEMPLATE PROGRAM "bbc_skeleton_v1_6.c"						=
//=																				=
//= Behavior-Based Control Skeleton code.										=
//=																				=
//= Provides a C program structure that emulates multi-tasking and				=
//= modularity for Behavior-based control with easy scalability.				=
//=																				=
//= Supplied for: Students of Mobile Robotics I, Fall 2013.						=
//= University of Nebraska-Lincoln Dept. of Computer & Electronics Engineering	=
//= Alisa N. Gilmore, P.E., Instructor, Course Developer.  Jose Santos, T.A.	=
//=																				=
//= Version 1.6  Updated 2/24/2016												=
//===============================================================================

//===============================================================================
//= Authors:	Michael Quinn, Collin Peterson.									=
//= Course:		ECEN 3450 - Mobile Robotics.									=
//= Assignment:	Laboratory 06 - Light Homing.									=
//= Due Date:	03/16/18														=
//= File Name:	main.c															=
//= Desc:		Main file that everything else runs off.						=
//= Functions:	CBOT_main()														=
//= Other:		Globals.														=
//= Note:		On start, user must add "libcapi324v22x" and "libprintf_flt"	=
//=				to the project libraries.										=
//===============================================================================

//===============================================================================
//= What:	Include file "ECEN3450Lab06.h" which has prototypes and includes.	=
//===============================================================================
#include "ECEN3450Lab06.h"

//===============================================================================
//= What:	Globals.															=
//===============================================================================
// This variable holds parameters that determine
// the current action that is taking place.
// Here, a structure named "action" of type
// MOTOR_ACTION is declared.
volatile MOTOR_ACTION action; 

//===============================================================================
//= What:	CBOT_main()															=
//= Why:	Main function of the program. 										=
//= Desc:	Preps CEENBoT to run and contains while(1) priority arbitration.	=
//= Return:	void.																=
//= Params:	void.																=
//= Notes:	none.																=
//===============================================================================
void CBOT_main( void )
{
	// Open everything
	open_modules();
	
	volatile SENSOR_DATA sensor_data;
	
	// Calibrate the PR sensors
	calibrate_pr( &sensor_data );
	/*
	// Get the calibration settings (PR_delta_RL)
	int switch_bool = 1;
	LCD_clear();
	LCD_printf("ECEN 3450: Mob Rob\nLab5: Light Homing\nQuinn & Peterson\nSW3: Calibrate");
	while(switch_bool == 1)
	{
		if(ATTINY_get_SW_state(ATTINY_SW3))
		{
			DELAY_ms(400);
			get_PR_diff( &sensor_data );
			switch_bool = 0;
		}
	}
	*/
	
	// Reset the current motor action.
	__RESET_ACTION( action );
	
	// Notify program is about to start.
	LCD_clear();
	LCD_printf( "Starting...\n" );
	
	// Wait 3 seconds or so.
	TMRSRVC_delay( TMR_SECS( 3 ) );
	
	// Clear the screen and enter the arbitration loop.
	LCD_clear();
	
	// Enter the 'arbitration' while() loop -- it is important that NONE
	// of the behavior functions listed in the arbitration loop BLOCK!
	// Behaviors are listed in increasing order of priority, with the last
	// behavior having the greatest priority (because it has the last 'say'
	// regarding motor action (or any action)).
	while( 1 )
	{
		// Sense must always happen first.
		// (IR sense happens every 125ms).
		IR_sense( &sensor_data, 125 );
		PR_sense( &sensor_data, 125 );
		
		// ================= Behaviors.
		// Priority (least to greatest): explore, light_follow, ir_avoid
		// Note that 'avoidance' relies on sensor data to determine
		// whether or not 'avoidance' is necessary.
		explore( &action );
		light_follow( &action, &sensor_data );
//		light_observe( &action, &sensor_data );
		IR_avoid( &action, &sensor_data );
		
		// Perform the action of highest priority.
		act( &action );

		// Real-time display info, should happen last, if possible
		// (except for 'ballistic' behaviors).  Technically this is
		// sort of 'optional' as it does not constitute a 'behavior'.
		info_display( &action );
	} // end while()
} // end CBOT_main()
