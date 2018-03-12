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
// Desc: Header file for the ISR subsystem module.

#ifndef __ISR324V221_H__
#define __ISR324V221_H__

#include "sys324v221.h"
#include "utils324v221.h"
// TODO: #include "spkr324v221.h"

// ============================ defines ===================================== //
// Desc: Macro for declaring  ISRs via the CEENBoT-API.
#define CBOT_ISR( isr_name )    void isr_name( void )

// Desc: Alternative names for specifying ISR vector number:

// External Interrupts
#define ISR_INT0_VECT   ISR_VECT1
#define ISR_INT1_VECT   ISR_VECT2
#define ISR_INT2_VECT   ISR_VECT3

// Pin-Change Interrupts
#define ISR_PCINT0_VECT ISR_VECT4
#define ISR_PCINT1_VECT ISR_VECT5
#define ISR_PCINT2_VECT ISR_VECT6
#define ISR_PCINT3_VECT ISR_VECT7

// Watchdog
#define ISR_WDT_VECT    ISR_VECT8

// Timer 2
#define ISR_TIMER2_COMPA_VECT   ISR_VECT9
#define ISR_TIMER2_COMPB_VECT   ISR_VECT10
#define ISR_TIMER2_OVF_VECT     ISR_VECT11

// Timer 1
#define ISR_TIMER1_CAPT_VECT    ISR_VECT12
#define ISR_TIMER1_COMPA_VECT   ISR_VECT13
#define ISR_TIMER1_COMPB_VECT   ISR_VECT14
#define ISR_TIMER1_OVF_VECT     ISR_VECT15

// Timer 0
#define ISR_TIMER0_COMPA_VECT   ISR_VECT16
#define ISR_TIMER0_COMPB_VECT   ISR_VECT17
#define ISR_TIMER0_OVF_VECT     ISR_VECT18

// SPI
#define ISR_SPI_STC_VECT        ISR_VECT19

// USART0
#define ISR_USART0_RX_VECT      ISR_VECT20
#define ISR_USART0_UDRE_VECT    ISR_VECT21
#define ISR_USART0_TX_VECT      ISR_VECT22

// Analog Comparator
#define ISR_ANALOG_COMP_VECT    ISR_VECT23

// ADC
#define ISR_ADC_VECT            ISR_VECT24

// EEPROM
#define ISR_EE_READY_VECT       ISR_VECT25

// TWI (I2C)
#define ISR_TWI_VECT            ISR_VECT26

// FLASH
#define ISR_SPM_READY_VECT      ISR_VECT27

// USART1
#define ISR_USART1_RX_VECT      ISR_VECT28
#define ISR_USART1_UDRE_VECT    ISR_VECT29
#define ISR_USART1_TX_VECT      ISR_VECT30

// ========================= type declarations ============================== //
// Type declaration for declaring function pointers for CBOT ISRs.
typedef void ( *CBOT_ISR_FUNC_PTR )( void );

// Custom enumeration type for specifying the ISR vector to attach a custom
// ISR via 'ISR_attach()'.
typedef enum ISR_VECT_TYPE {

    ISR_VECT0 = 0,
    ISR_VECT1,
    ISR_VECT2,
    ISR_VECT3,
    ISR_VECT4,
    ISR_VECT5,
    ISR_VECT6,
    ISR_VECT7,
    ISR_VECT8,
    ISR_VECT9,
    ISR_VECT10,
    ISR_VECT11,
    ISR_VECT12,
    ISR_VECT13,
    ISR_VECT14,
    ISR_VECT15,
    ISR_VECT16,
    ISR_VECT17,
    ISR_VECT18,
    ISR_VECT19,
    ISR_VECT20,
    ISR_VECT21,
    ISR_VECT22,
    ISR_VECT23,
    ISR_VECT24,
    ISR_VECT25,
    ISR_VECT26,
    ISR_VECT27,
    ISR_VECT28,
    ISR_VECT29,
    ISR_VECT30,
    ISR_VECT31

} ISR_VECT;    

// ============================ prototypes ================================== //
// Input Args: None.
// Returns: A value of type 'SUBSYS_STATUS', which will be one of the
//          following enumerated constants:
//
//              'SUBSYS_CLOSED' - Indicates the subsystm is closed.
//
//              'SUBSYS_OPEN'   - The subsystm is successfully open and can
//                                now be used.
//
//              'SUBSYS_ALREADY_OPEN' - Indicates the subsystem has already
//                                      been previously opened and initialized
//                                      by another client (subsystem).
//
//              'SUBSYS_IN_USE' - Indicates the subsystem [cannot be closed]
//                                because it is being used, or there is a
//                                dependency that needs to be met.
//
//              'SUBSYS_NOT_AVAILABLE' - Attempting to open a subsystem whose
//                                       resource is not available.
//
//              'SUBSYS_ERROR' - Generic unknown error.
//
//              'SUBSYS_INIT_FAILED' - It means things started out good, but
//                                     something happened along the way (i.e.,
//                                     hardware dependency not responding).
//
//              'SUBSYS_DEPENDENCY_ERROR' - Could not open because a dependency
//                                          (on another subsystem) could not be
//                                          met b/c it is not available or does
//                                          not exist.
//
//              'SUBSYS_DEPENDENCY_CONFLICT' - Could not open because an
//                                             UNSHARABLE resource needed by
//                                             the subsystem is already in use.
//
//          Generally speaking, users are normally interested (and should
//          just check) if 'SUBSYS_OPEN' or 'SUBSYS_ALREADY_OPEN' was returned
//          while all others should be treated as errors.
//
// Desc: Open and initialize the ISR subsystem module.
//
//       Users should at least check to see if the function returns 
//       'SUBSYS_OPEN' or 'SUBSYS_ALREADY_OPEN' and proceed accordingly.  The
//       rest of the return status values should be treated as errors and
//       are mainly of interest during debugging, or for debugging purposes.
//
//       The typical 'open' sequence might look like:
//
//          SUBSYS_STATUS opstat;
//
//          // Attempt to open the subsystem in question.
//          opstat = XXXXX_open();
//
//          // Proceed only IF successfully opened.
//          if( ( opstat == SUBSYS_OPEN ) || ( opstat == SUBSYS_ALREADY_OPEN ) )
//          {
//                  /* Your code here */
//          }
//          else
//          {
//                  /* Spit out an error message, or somethin' */
//          }
//
//       It is up to the user to determine if only 'SUBSYS_OPEN' is necessary
//       and whether or not a 'SUBSYS_ALREADY_OPEN' is permissible.  If the
//       user requires exclusive use of a subsystem module, then he/she must
//       want to be the 'first' to open the device.  At this point, then, 
//       the user will want to make sure only 'SUBYS_OPEN' is returned and
//       treat everything else (even 'SUBSYS_ALREADY_OPEN') as an error.
//
extern SUBSYS_STATUS ISR_open( void );

// -------------------------------------------------------------------------- //
// TODO: (finish)
// Desc: Close and relinquish the ISR subsystem module.
extern void ISR_close( void );

// -------------------------------------------------------------------------- //
// TODO: (finish)
// Desc: Function allows the user to register a custom ISR with the module.
extern CBOT_ISR_FUNC_PTR ISR_attach( ISR_VECT vect, 
                                     CBOT_ISR_FUNC_PTR isr_function );
// ========================= external declarations ========================== //
extern CBOT_ISR_FUNC_PTR CBOT_ISR_vtable[];

#endif /* __ISR324V221_H__ */
