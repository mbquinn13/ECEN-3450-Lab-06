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
// Desc: Header file for the SPI subsystem module.  It contains functions/res-
//       ources for using the SPI subsystem.  Presently only a limited set of
//       features are implemented to support other aspects of the CEENBoT (TM)
//       API.

#ifndef __SPI324V221_H__
#define __SPI324V221_H__

#define __SPI324MODULE__

#include<avr/io.h>

#include "utils324v221.h"
#include "sys324v221.h"

// The following macro-definitions are for currently assigned
// SPI slave addresses, and their assigned devices.

// Assigned devices.
#define SPI_ADDR_LCD        SPI_DEV_ADDR0    
#define SPI_ADDR_PSXC       SPI_DEV_ADDR1
#define SPI_ADDR_ATTINY0    SPI_DEV_ADDR2
#define SPI_ADDR_SPIFLASH   SPI_DEV_ADDR6
#define SPI_ADDR_NA         SPI_DEV_ADDR7

// User (open) devices.
#define SPI_ADDR_SMARTDEV0  SPI_DEV_ADDR3
#define SPI_ADDR_SMARTDEV1  SPI_DEV_ADDR4
#define SPI_ADDR_SMARTDEV2  SPI_DEV_ADDR5

// The following macro is used for declaring SPI user configuration functions,
// also referred to as 'SPI_CFGR' or SPI (C)on(F)i(G)uration (R)outine.
#define SPI_CFGR( spi_cfgr )    void spi_cfgr( void )

// Don't care data (for receiving cases).
#define SPI_NULL_DATA    0x00

// ========================== type declarations ============================= //
// Custom type for for SPI 'Slave Device' addresses.
typedef enum SPI_SSADDR_TYPE
{

    SPI_DEV_ADDR0 = 0,  // Currently assigned to LCD device.
    SPI_DEV_ADDR1,      // Currently assigned to PSX Controller. (PSXC).
    SPI_DEV_ADDR2,      // Currently assigned to ATtiny MCU.
    SPI_DEV_ADDR3,      // Available.
    SPI_DEV_ADDR4,      // Available.
    SPI_DEV_ADDR5,      // Available.
    SPI_DEV_ADDR6,      // 4MBit on-board SPI-FLASH.
    SPI_DEV_ADDR7       // Reserved.  Used as 'message separator'.

} SPI_SSADDR;

typedef void ( *SPI_CONFIGFUNC )( void );

// ============================= prototypes ================================= //
// Input  Args: TODO
// Output Args:
// Globals  Read:
// Globals Write:
// Returns:
//
// Desc: Initializes and configures the SPI as needed for proper CEENBoT (TM)
//       operation.
extern SUBSYS_STATUS SPI_open( void );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Function deallocates and releases SPI resources.
extern void SPI_close( void );
// -------------------------------------------------------------------------- //
// Input  Args: TODO
// Output Args:
// Globals  Read:
// Globals Write:
// Returns:
//
// Desc:
extern void SPI_set_slave_addr( SPI_SSADDR slaveSelectAddr );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Function is used to transmit data to the selected slave device once
//       the SPI is up and running.
extern void SPI_transmit( SPI_SSADDR slaveSelectAddr, unsigned char data );
// -------------------------------------------------------------------------- //
// TODO
// Desc: Function to receive data from the selected SPI slave device.  Note that
//       because of the synchronous nature of the SPI subsystem.  You CANNOT
//       receive WITHOUT transmitting! -- this is why this function has a 'data'
//       parameter.  Sometimes a specific 'data' value may be needed (if this
//       data serves as a command on 'what to receive', or it can simply be
//       dummy (or don't care arbitrary data).  In either case, you MUST supply
//       SOME value, even if it's just '0'.
extern 
    unsigned char SPI_receive( SPI_SSADDR slaveSelectAddr, unsigned char data );
// -------------------------------------------------------------------------- //
// Input  Args: 'ssAddr' - Must be one of the following enumerated constants:
//                          'SPI_DEV_ADDRn', where 'n' is from 0-7.
//
//                         You can also used some of the defined 'aliases'
//                         which accomplish the same thing, but are provided
//                         for clarity.  These include the preassigned spi
//                         addresses given by:
//
//                          'SPI_ADDR_LCD'
//                          'SPI_ADDR_PSXC'
//                          'SPI_ADDR_ATTINY0'
//
//                         and the user-defined spi addresses, which include:
//                          'SPI_ADDR_SMARTDEV0'
//                          'SPI_ADDR_SMARTDEV1'
//                          'SPI_ADDR_SMARTDEV2'
//
//               'pConfigFunction' - Provide to this argument, the name of the
//                                   configuration function that is to be 
//                                   associated with the specified 'ssAddr'
//                                   device.  The function provided MUST be
//                                   declared and defined using the 'SPI_CFGR()'
//                                   macro.  'Desc' below for example usage.
//
// Desc: Function used for assigning custom configuration routines that is
//       specific to a given SPI slave device.  Since different SPI devices
//       may require different configuration options, this function makes it
//       possible to assign a 'custom' configuration that is invoked to set
//       up the SPI device to a desired configuration before transmitting or
//       receiving data.  The function must be declared in a specific form, and
//       the 'SPI_CFGR()' macro is provided for this specific purpose.
//
//       The purpose of this facility is to allow for communications with, and
//       allow for the accomodation of, multiple SPI devices that use a hetero-
//       geneous configuration structure as opposed to all devices using the 
//       same configuration (homogeneous) structure.
//
// Ex:  Suppose you have a custom device assigned to be used with 'Smart Device
//      0'. You would then, in your program, define the following custom
//      configuration routine.
//
//          SPI_CFGR( mySpiConfig )
//          {
//
//             // ATmega324 configuration code for SPI here... 
//
//          }
//
//      Then, somewhere in your initialization code, PRIOR to using the SPI
//      device, you would have to tell the SPI subsystem to use this specific
//      function with 'Smart Device 0' by invoking this function as follows:
//
//          SPI_set_config_func( SPI_ADDR_SMARTDEV0, mySpiConfig );
//
//      That's it.  Then, when you invoke the function: 'SPI_set_slave_addr()'
//      function, the function will internally call 'mySpiConfig()' to set and
//      configure the SPI as needed before use.
//
//      This header file, and also the functions in the *.c module contain
//      some example for those SPI that are already assigned for custom use.
extern
  void SPI_set_config_func( SPI_SSADDR ssAddr, SPI_CONFIGFUNC pConfigFunction );

// ---------- SPI Configuration Routine Prototype Declarations -------------- //

extern SPI_CFGR( SPI_ATtiny0_config );
extern SPI_CFGR( SPI_PSXC_config );
extern SPI_CFGR( SPI_LCD_config );
extern SPI_CFGR( SPI_default_config );
extern SPI_CFGR( SPI_FLASH_config );

// ========================= external globals =============================== //
extern volatile SPI_SSADDR     curr_spi_addr;
extern          SPI_CONFIGFUNC spi_config_func_LUT[];

#endif /* __SPI324V221_H__ */
