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
// Desc: Header file for 'utils324firm-v221.c'.  The set of functions and 
//       services offered by these files constitute generic utilities that are
//       used by other *.c and *.h files.

#ifndef __UTILS324V221_H__
#define __UTILS324V221_H__

// *** SOME USEFUL HELPER MACROS ***

#define SET         1
#define CLEAR       0
#define SET_BIT     SET
#define CLEAR_BIT   CLEAR
#define CLR_BIT     CLEAR_BIT

// NOTE: Be caure with the xxxBV() familiy of functions.  The 'n' parameter
//       which refers to 'bit' positions, start from 0.  So if you want to
//       manipulate 'bit 2', for example to set bit 2 in some register called
//       __SOME_REG, then you would do 'SBV( 1, __SOME_REG )', etc, and 
//       **NOT** 'SBV( 2, __SOME_REG )', which will actually end up manip-
//       ulating bit 3 and not bit 2 as intended!

// Desc: 'Get Bit' Macro used to get the 'n'-th bit from 'reg' register value.
#define GBV( n, reg )      ((( reg >> n ) & 0x1 ))

// Desc: 'Set/Clear Bit Value' used to either 'set' or 'clear' the 'n'-th
//        bit position of the specified register given by 'reg'.  'v' must
//        be '0' (to clear it) or '1' (to set it).
#define SCBV( v, n, reg ) \
                ( reg = ( v ? (( 1 << n ) | reg ):(( ~( 1 << n ) ) & reg )))

// Desc: 'Set Bit Value' and 'Clear Bit Value'.  Short-cut macros that are
//        somewhat simpler implementations of 'SCBV()' directly.
#define SBV( n, reg )   SCBV( 1, n, reg )
#define CBV( n, reg )   SCBV( 0, n, reg )

// Desc: 'Toggle Bit Value'.  Macro can be used to toggle the state of the
//       'n'-th bit position of register specified by 'reg'.
#define TBV( n, reg )   ( reg ^= ( 1 << n ) )

// Desc: Macro utility for configuring port directionality.
//
// 'ddr_port' - Must be 'A', 'B', 'C', or 'D', depending on the available
//              ports.  In the case of the '324, you have A-D available.
// 'n'   - The 'bit' position affected on the specified port.
// 'dir' - Must be 'OUTPIN' or 'INPIN'.
//
// EX: To set the 6th and 7th bits in PORTB to both 'outputs', we can do: 
//              SBD( B, 6, OUTPIN );
//              SBD( B, 7, INPIN );
#define INPIN  0
#define OUTPIN 1
#define SBD( ddr_port, n, dir )   SCBV( dir, n, DDR ## ddr_port )

// TODO:  In the future move this to 'delay324v221.h'
//        Delay utilities module.
#ifndef __NO_DELAYS

    // NOTE: From now on, this should be defined at the compiler command line
    //       via '-D' flag (in 'avr-gcc').
    // #define F_CPU   20000000L   /* Required by 'delay.h'. */

    #include<util/delay.h>

    #define DELAY_ms( t )    _delay_ms( t )
    #define DELAY_us( t )    _delay_us( t )

#else

    #define DELAY_ms( t )
    #define DELAY_us( t )

#endif /* __NO_DELAYS */

// Enumeration declaration for custom boolean type.
typedef enum BOOL_TYPE { FALSE = 0, TRUE = 1 } BOOL;

#endif /* __UTILS324V221_H__ */
