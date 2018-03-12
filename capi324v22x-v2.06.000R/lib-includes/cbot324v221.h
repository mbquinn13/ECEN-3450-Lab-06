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
// Desc: Header for the CEENBoT (TM) module.

#ifndef __CBOT324V221_H__
#define __CBOT324V221_H__

// ============================ includes ==================================== //
#include<avr/io.h>
#include<avr/interrupt.h>

#include "capi324v221.h"

// ============================= prototypes ================================= //
// TODO
// Desc: This function is the user's version of 'main()'.  This is where all
//       of the user's code will reside in, and thus it MUST be defined by the
//       user.  The standard 'main()' function is used internally by the API
//       to initialize necessary subsystems.  Note that although this PROTOTYPE
//       is designated as 'private', it's definition is made publicly.
extern void CBOT_main( void );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Initializes the CEENBoT's API.
extern void CBOT_init( void );
// -------------------------------------------------------------------------- //
// Desc: This function is needed here to force the linker to bring in the 
//       'CBOT_isr' object module to force undefined references to interrupt
//       service routines to be resolved.

extern void CBOT_ISR_init( void );

#endif /* __CBOT324V221_H__ */
