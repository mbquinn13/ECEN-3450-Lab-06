//===============================================================================
//= Authors:	Michael Quinn, Collin Peterson.									=
//= Course:		ECEN 3450 - Mobile Robotics.									=
//= Assignment:	Laboratory 06 - Light Homing.									=
//= Due Date:	03/16/18														=
//= File Name:	explore.c														=
//= Desc:		Contains one of the basic behaviors of the system.				=
//= Functions:	explore()														=
//= Other:		none.															=
//===============================================================================

//===============================================================================
//= What:	Include file "ECEN3450Lab06.h" which has prototypes and includes.	=
//===============================================================================
#include "ECEN3450Lab06.h"

//===============================================================================
//= What:	explore()															=
//= Why:	Behavior to have the CEENBoT cruise in a straight line.				=
//= Desc:	Sets state to "EXPLORING" and sets left/right speed/acceleration.	=
//= Return:	void.																=
//= Params:	volatile MOTOR_ACTION *pAction (the pointer for all motor actions)	=
//= Notes:	none.																=
//===============================================================================
void explore( volatile MOTOR_ACTION *pAction )
{
	pAction->state = EXPLORING;
	pAction->speed_L = 150;
	pAction->speed_R = 150;
	pAction->accel_L = 400;
	pAction->accel_R = 400;
} // end explore()