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
// Desc: This header file constitutes the 'primary' header file for the CEENBoT
//       API.  It should always be included by the user who writes code using
//       this CEENBoT API.
//

#ifndef __CAPI324V221_H__
#define __CAPI324V221_H__

// *** IMPORTANT:  THESE MUST MATCH THE CURRENT REVISION ***

// Revision format is given as: 'vX.XX.XXXc'
// Where the positions indicate 'v<major>.<minor>.<build><status-code>'

// Version: v2.06.000R
#define CAPI_REV_MAJOR  2   /* One or more digits.    */
#define CAPI_REV_MINOR  6   /* Two Digits exactly.    */
#define CAPI_REV_BUILD  0 /* Three Digits exactly.  */
#define CAPI_REV_STAT   'R' /* One character exactly. */

// ============================= includes =================================== //

#include "utils324v221.h"    // Helper macros and other utilities.
#include "sys324v221.h"      // System-wide parameters module.
#include "psxc324v221.h"     // Playstation controller module.
#include "lcd324v221.h"      // Display subsystem module.
#include "led324v221.h"      // LEDs control module.
#include "mega324v221.h"     // Module for the primary MCU.
#include "tiny324v221.h"     // Module for the secondary MCU.
#include "tmrsrvc324v221.h"  // Module for the timer service subsystem.
#include "step324v221.h"   // Module for the stepper motor subsystem.
#include "spkr324v221.h"   // Module for the speaker subsystem.
#include "swatch324v221.h" // Module for the stop-watch service subsystem.
#include "usonic324v221.h" // Module for using the ultrasonic device.
#include "uart324v221.h"   // Module for using the UART device.
#include "adc324v221.h"    // Module for using the ADC device.
#include "isr324v221.h"    // Module for using the ISR subsystem.
#include "spi324v221.h"      // Module for using the SPI subsystem.
#include "spiflash324v221.h" // Module for using the SPIFLASH subsystem.
#include "ti324v221.h"     // Module for using the TI subsystem module.
#include "i2c324v221.h"    // Module for using the I2C subsystem.
#include "cmucam324v221.h" // Module for the CMUCam4 subsystem.
#include "pixy324v221.h"   // Module for the CMUcam5 (aka 'Pixy') subsystem.
#include "bbtk324v221.h"    // Module for the Behavior-Based Toolkit.
#include "cbot324v221.h"   // CEENBoT (TM) module.

// ========================= type declarations ============================== //
// Custom type declaration for obtaining the current revision of the API.
typedef struct CAPI_REV_TYPE {

                                // Print format: vX.XX.XXXc
    unsigned short int major;   // Major revision number (1 or more digits).
    unsigned short int minor;   // Minor revision number (2 digits exactly).
    unsigned short int build;   // Build number          (3 digits exactly).
    char status;                // Revision status       (1 character exactly).

} CAPI_REV;

// =========================== prototypes =================================== //
// Input  Args: 'pRev' - Provide to this argument the **ADDRESS OF** a structure
//                       of type 'CAPI_REV'.  The structure will be populated
//                       with relevant revision info.
// Output Args: 'pRev' - The structure is populated with relevant revision info.
//
// Globals  Read: None.
// Globals Write: None.
//
// Returns: Nothing.
//
// Desc: Function can be used to obtain the revision of the API after it has
//       been compiled into a static library.
//
extern void CAPI_get_revision( CAPI_REV *pRev );
// -------------------------------------------------------------------------- //
// Desc: Function returns a seed value derived from the current battery voltage.
//       Since we're 'somewhat' guaranteed' that this value will be mono-
//       tonically decreasing over time, we can always rely and 'eventually'
//       getting different seed values.  Only one seed value is returned per
//       session.  That is, you have to power-cycle the system to generate a 
//       new seed.  However, this doesn't guarantee that the seed will be unique
//       because the battery voltage may not even have changed over cycles.
extern unsigned short int CAPI_get_seed_val( void );

// ========================== external declarations ========================= //
extern BOOL seed_generated;     // Holds whether the seed has already been
                                // generated.

extern unsigned short int current_seed_val; // Holds the current seed value
                                            // once generated.

#endif /* __CAPI324V221_H__ */
