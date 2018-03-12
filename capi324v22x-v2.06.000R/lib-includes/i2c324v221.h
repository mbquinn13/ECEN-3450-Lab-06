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
// Desc: Header file for the I2C (TWI) API subsystem module.

#ifndef __I2C324V221_H__
#define __I2C324V221_H__

#include <avr/io.h>

#include "utils324v221.h"
#include "sys324v221.h"

// ============================= defines ==================================== //
// ======================== type declarations =============================== //
// Desc: Custom enumerated type declaration for specifying the bit-rate
//       prescaler bits used with the 'I2C_set_BRG()' function.
typedef enum I2C_PRESCALER_TYPE {

    I2C_PRESCALER_1 = 0,
    I2C_PRESCALER_4,
    I2C_PRESCALER_16,
    I2C_PRESCALER_64

} I2C_PRESCALER;

// Desc: Custom enumerated type declaration for specifying the status of 
//       the communication transaction in Master Transmit (MT) or Master
//       Receive (MR) mode.
//
//          'I2C_STAT_ACK_ERROR' is issued when a device refuses to 
//          acknowledge its address (due to an error, or because such
//          device doesn't exist on the I2C-bus.  In contrast, 'I2C_STAT_NO_ACK'
//          simply means that during 'data transmission' (in MT-mode), the
//          I2C-device has acknowledged that it has received the LAST BYTE
//          OR... that it's buffer is full and cannot accept any more data
//          for the time being, at which point you should either wait a bit
//          and try again, or restart the sequence, or STOP it.  Whether
//          'I2C_STAT_NO_ACK' is considered an 'ERROR' depends on the device
//          in question, and thus the datasheet of such device must be 
//          consulted for expected behavior.
//
typedef enum I2C_STATUS_TYPE {

    I2C_STAT_OK = 0,         // Everything went OK.

    I2C_STAT_START_ERROR,    // A Start-sequence error has occurred.

    I2C_STAT_ACK_ERROR,      // An acknowledge error has occurred.

    I2C_STAT_NO_ACK,         // Slave device did not acknowledge last
                             // transaction.

    I2C_STAT_ARB_ERROR,      // An arbitration error has occurred.

    I2C_STAT_CONFIG_ERROR,   // Device has not been configured via
                             // 'I2C_set_BRG()'.

    I2C_STAT_START_STOP_REQ, // Slave Mode only.  It means no operation took
                             // place because the MASTER I2C device is request-
                             // ing to STOP or RESTART.  In any case, this 
                             // particular can be used as an indicator to the
                             // programmer that a new communication transact-
                             // ion will take place, that should take appro-
                             // priate action to initialize parameters and/or
                             // state machines the user may have set up.

    I2C_STAT_BUSY,           // The I2C susbsystem is BUSY ( either MASTER or
                             // the SLAVE are in the middle of a transaction ).

    I2C_STAT_MASTER_BUSY,    // This state is returned when the I2C device
                             // attempts to perform a 'slave' operation while
                             // in the middle of a 'master' operation.

    I2C_STAT_SLAVE_BUSY,     // This state is returned when the I2C device
                             // attempts to perform a 'master' operation while
                             // in the middle of a 'slave' operation.

    I2C_STAT_NOT_MASTER,     // This state is returned by a function that 
                             // attempts to perform a 'MASTER' operation, but 
                             // couldn't because it is not in MASTER mode.

    I2C_STAT_NOT_SLAVE,      // This state is returned by a function that 
                             // attempts to perform a 'SLAVE' operation, but
                             // couldn't because it is not in SLAVE mode.

    I2C_STAT_UNKNOWN_ERROR,  // The name says it all.                             

    // Status codes reserved for 'I2C_GetMasterReq()'.
    I2C_STAT_MASTER_WANTS_TO_SEND,  // This status is returned by the
                                    // 'I2C_GetMasterReq()' function when the
                                    // MASTER has addressed 'you' because it
                                    // wants to send (i.e., 'write') data TO
                                    // you (the SLAVE device).

    I2C_STAT_MASTER_WANTS_TO_RECV,  // This status is returned by the
                                    // 'I2C_GetMasterReq()' function when the
                                    // MASTER has addressed 'you' because it
                                    // wants to receive (i.e., 'receive') data
                                    // FROM you (the SLAVE device).

    I2C_STAT_MASTER_IS_FINISHED,    // This status is returned by the
                                    // 'I2C_GetMasterReq()' function when the
                                    // MASTER has previously addressed 'you'
                                    // for either sending or receiving and 
                                    // it has indicated it is finished by 
                                    // either sending a 'STOP/RESTART' request
                                    // (when in SLAVE-RECEIVE mode), or 
                                    // a NOT-ACKNOWLEDGE has been received
                                    // for the last byte sent (when in 
                                    // SLAVE-TRANSMITTER mode).

    I2C_STAT_MASTER_GOT_LAST_OK,    // This status is returned by either
                                    // 'I2C_GetMasterReq()' function, or by
                                    // the 'I2C_SLVE_send()' function when the
                                    // last byte is sent with a NOT ACKNOWLEDGE.

    I2C_STAT_MASTER_UNKNOWN_REASON

} I2C_STATUS;

typedef enum I2C_MODE_TYPE {

    I2C_MODE_MT = 0,        // Master Transmit (MT) mode.
    I2C_MODE_MR,            // Master Receive  (MR) mode.
    I2C_MODE_SR,            // Slave  Recieve  (SR) mode.
    I2C_MODE_ST             // Slave  Transmit (ST) mode.

} I2C_MODE;

// Desc: Type declaration holds internal parameters of the I2C subsystem module.
typedef struct I2C_PARAMS_TYPE {

    BOOL configured;    // True when the BRG has been configured.

    BOOL master_active; // True when I2C is in the middle of a transaction
                        // operating as MASTER.

    BOOL slave_active;  // True when I2C is in the middle of a transaction
                        // operating as SLAVE.

    BOOL using_slave;   // True if the SLAVE portion of the I2C has been
                        // enable for 'use'.

    BOOL using_pullups; // TRUE if the pullups are engaged.

} I2C_PARAMS;
// =========================== prototypes =================================== //
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
// Desc: Function acquires and initializes resources for using the I2C subsystem
//       module.  It must be called before any other functions exposed by this
//       module are invoked.
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
extern SUBSYS_STATUS I2C_open( void );
// -------------------------------------------------------------------------- //
// TODO: (finish)
// Desc: Function deallocates and releases resources in use by the I2C subsystem
//       module.
extern void I2C_close( void );
// -------------------------------------------------------------------------- //
// Desc: Function allows the user to set the bit-rate value and prescaler value
//       of the bit-rate generator (BRG) to obtain the desired operating clock 
//       rate of the I2C.  This function must be called immediately upon 
//       'I2C_open()' and prior to 'I2C_enable()'.  The equation is as follows:
//
//       Let x = bit_rate value, and y = prescaler.   Then,
//
//          SCL_freq = ( f_cpu ) / ( 16 + 2*x*(4^y) )
//
//       where SCL_freq is the frequency of the I2C clock (when operating as
//       MASTER), and f_cpu is the CPU frequency.
//
extern void I2C_set_BRG( unsigned char bit_rate, I2C_PRESCALER prescaler );
// -------------------------------------------------------------------------- //
// Desc: Function enables the I2C subsystem on the MCU.  It must be called once
//       'I2C_open()' and 'I2C_set_BRG()' functions are invoked to enable
//       operation of the I2C subsystem once configured.  You CANNOT enable
//       the I2C unless it has been configured.
extern void I2C_enable( void );
// -------------------------------------------------------------------------- //
// Desc: Function disables the I2C subsystem on the MCU.
extern void I2C_disable( void );
// -------------------------------------------------------------------------- //
// Desc: Function enables the resistor pullups on the I2C bus.
extern void I2C_pullup_enable( void );
// -------------------------------------------------------------------------- //
// Desc: Function disables the resistor pullups on the I2C bus.
extern void I2C_pullup_disable( void );
// -------------------------------------------------------------------------- //
// Input  Args: 'slave_addr' - A value between 0 - 127, specifying the slave
//                             I2C device to begin a communication transaction
//                             with.
//
//              'mode' - Must be one of the following enumerated constants:
//
//                          'I2C_MODE_MT' - Master Transmit (MT) mode.  This 
//                                          means you wish to send data to the
//                                          specified slave device.
//
//                          'I2C_MODE_MR' - Master Receive (MR) mode. This 
//                                          means you wish to read data from the
//                                          specified slave device.
//
// Output Args: None.
// Globals  Read:   None.
// Globals Write:   None.
//
// Returns: A value of type 'I2C_STATUS' will be returned by the function,
//          restricted specifically to one of the following below:
//          
//              'I2C_STAT_OK' - If no problem has occurred.
//
//              'I2C_STAT_START_ERROR' - If there was an error STARTING the
//                                       communication transaction.  This 
//                                       occurs first before device acknowledge-
//                                       ment.
//
//              'I2C_STAT_ACK_ERROR' - If a device refused to acknowledge when
//                                     acknowledgement was expected.  This 
//                                     occurs when no I2C SLAVE device responded
//                                     to the specified I2C SLAVE address.
//
//              'I2C_STAT_CONFIG_ERROR' - A 'START' was NOT possible because
//                                        you have not configured the bit
//                                        rate generator via 'I2C_set_BRG()'
//                                        function.  Note that this may not
//                                        necessarily be an issue because the 
//                                        BRG is set up for 100KHz operation
//                                        when first opened by default.
//
// Desc: Function INITIATES a communication transaction sequence in the mode
//       specified with the slave device specified as MASTER.  This is the 
//       first function that must be invoked in order to 'begin' a communication
//       transaction.  This must be followed by one or more calls to 
//       'I2C_MSTR_send()' to send the data, or 'I2C_MSTR_get()' to get data
//       and MUST be concluded with a call to 'I2C_MSTR_stop()' to conclude
//       the current communication transaction that was started via 
//       'I2C_MSTR_start()'.
extern I2C_STATUS I2C_MSTR_start( unsigned char slave_addr, I2C_MODE mode );
// -------------------------------------------------------------------------- //
// Input  Args: 'data' - The data 'byte' to be sent.
// Output Args: None.
//
// Globals  Read: I2C_params
// Globals Write: I2C_params
//
// Returns: A value of type 'I2C_STATUS' will be returned by the function,
//          restricted specifically to one of the following below:
//          
//              'I2C_STAT_OK' - If no problem has occurred.
//
//              'I2C_STAT_NO_ACK'    - If a device refused to acknowledge when
//                                     acknowledgement was expected.  This means
//                                     the SLAVE did NOT acknowledged the last
//                                     data byte sent.  Whether this is consid-
//                                     an error or not depends on context.  The
//                                     SLAVE may have received the byte but did
//                                     not acknowledge on purpose to let you 
//                                     know its buffer is FULL and it cannot 
//                                     accept any more data.  Alternatively, it
//                                     could problably mean the SLAVE did NOT
//                                     receive the data after all.  It is up
//                                     to the user to determine how to handle
//                                     this particular status.
//
//              'I2C_STAT_NOT_MASTER - This status was returned because the
//                                     device is not in MASTER mode as it should
//                                     have been (i.e., you forgot to perhaps
//                                     call 'I2C_MSTR_start()' first).  This is
//                                     considered an ERROR.
//
//              'I2C_STAT_CONFIG_ERROR' - A 'send' was NOT possible because
//                                        you have not configured the bit
//                                        rate generator via 'I2C_set_BRG()'
//                                        function.  Note that this may not
//                                        necessarily be an issue because the 
//                                        BRG is set up for 100KHz operation
//                                        when first opened by default.
//
//              'I2C_STAT_UNKNOWN_ERROR' - The name says it all.
//
// Desc: Function can be used to send data to a slave device once a communi-
//       cantion transaction has been started with such device via a prior
//       call to 'I2C_MSTR_start()'.  Multiple bytes can be sent by simply
//       invoking this function multiple times (one for each byte).  The user
//       must make sure to invoke 'I2C_MSTR_stop()' once the communication
//       transaction is completed.  Alternatively, the user may wish to 
//       issue a 'restart' condition by invoking 'I2C_MSTR_start()'.
//
//       For sending mutiple bytes back-to-back consider using the
//       'I2C_MSTR_send_multiple()' function.
extern I2C_STATUS I2C_MSTR_send( unsigned char data );
// -------------------------------------------------------------------------- //
// Input  Args: 'pBuffer' - You must supply to this argument *THE ADDRESS*
//                          of a buffer that holds 'unsigned char', whose size
//                          must equal to the number of bytes that will be
//                          sent in one 'burst'.
//
//              'count' - Number of bytes to send from 'pBuffer'.
//
// Output Args: None.
//
// Globals  Read: None.
// Globals Write: None.
//
// Returns: A value of type 'I2C_STATUS' will be returned by the function,
//          restricted specifically to one of the following below.  Note that
//          these are the same status codes as 'I2C_MSTR_send()':
//          
//              'I2C_STAT_OK' - If no problem has occurred.
//
//              'I2C_STAT_NO_ACK'    - If a device refused to acknowledge when
//                                     acknowledgement was expected.  This means
//                                     the SLAVE did NOT acknowledged the last
//                                     data byte sent.  Whether this is consid-
//                                     an error or not depends on context.  The
//                                     SLAVE may have received the byte but did
//                                     not acknowledge on purpose to let you 
//                                     know its buffer is FULL and it cannot 
//                                     accept any more data.  Alternatively, it
//                                     could problably mean the SLAVE did NOT
//                                     receive the data after all.  It is up
//                                     to the user to determine how to handle
//                                     this particular status.
//
//              'I2C_STAT_NOT_MASTER - This status was returned because the
//                                     device is not in MASTER mode as it should
//                                     have been (i.e., you forgot to perhaps
//                                     call 'I2C_MSTR_start()' first).  This is
//                                     considered an ERROR.
//
//              'I2C_STAT_CONFIG_ERROR' - A 'send' was NOT possible because
//                                        you have not configured the bit
//                                        rate generator via 'I2C_set_BRG()'
//                                        function.  Note that this may not
//                                        necessarily be an issue because the 
//                                        BRG is set up for 100KHz operation
//                                        when first opened by default.
//
//              'I2C_STAT_UNKNOWN_ERROR' - The name says it all.
//
//
// Desc: Function can be used to send multiple bytes (specified by 'count'),
//       back-to-back to an I2C device.  A communication transaction must have
//       been previously initiated with the recipient device via a prior call
//       to 'I2C_MSTR_start()'.  Like 'I2C_MSTR_send()', the user must make 
//       sure to invoke 'I2C_MSTR_stop()' to complete the communication
//       transaction.  Alternatively, the user may wish to issue a 'restart'
//       condition by invoking 'I2C_MSTR_start()'.
//
//       It is *CRITICAL* that the value of 'count' does NOT exceed the
//       size of the buffer containing the data.  If the user violates this
//       condition, program behavior will be UNPREDICTABLE, leading to 
//       a program crash.
extern I2C_STATUS I2C_MSTR_send_multiple( unsigned char *pBuffer, 
                                          unsigned short int count );
// -------------------------------------------------------------------------- //
// Input  Args: 'pData' - The ADDRESS OF the location where the data will be
//                        returned to once received must be provided to this
//                        argument.
//
//              'acknowledge' - Must be either 'TRUE' or 'FALSE'.  You **MUST**
//                              acknowledge each byte received EXCEPT for the
//                              LAST ONE, as required by the I2C protocol.
//                              See 'Desc' section below for more info.
//
// Ouptut Args: None.
//
// Globals  Read: 'I2C_params'
// Globals Write: 'I2C_params'
//
// Returns: A value of type 'I2C_STATUS' will be returned by the function,
//          restricted specifically to one of the following below:
//
//              'I2C_STAT_OK' - If no problem has occurred.
//
//              'I2C_STAT_ACK_ERROR' - This status is returned when and ack-
//                                     nowledgement error has occurred.  This
//                                     normally happens when a different 
//                                     acknowedgment other than what was spec-
//                                     ified by the user has occurred.  Unlike
//                                     'I2C_STAT_NO_ACK', which may or may not
//                                     be considered an ERROR, this particular
//                                     status SHOULD be considered an ERROR.
//
//              'I2C_STAT_NOT_MASTER - This status was returned because the
//                                     device is not in MASTER mode as it should
//                                     have been (i.e., you forgot to perhaps
//                                     call 'I2C_MSTR_start()' first).  This is
//                                     considered an ERROR.
//
//              'I2C_STAT_CONFIG_ERROR' - A 'send' was NOT possible because
//                                        you have not configured the bit
//                                        rate generator via 'I2C_set_BRG()'
//                                        function.  Note that this may not
//                                        necessarily be an issue because the 
//                                        BRG is set up for 100KHz operation
//                                        when first opened by default.
//
// Desc: Function can be used to receive data (as MASTER) from a slave device
//       once communication transaction has been started with such device via
//       a prior call to 'I2C_MSTR_start()' in the corresponding mode.  Multiple
//       bytes can be received by simply invoking this function multiple times
//       (one for each byte).  
//
//       Note, that as per required by the I2C protocol, and when operating
//       as MASTER-RECEIVER (MR) that you *MUST* acknowledge each byte
//       received EXCEPT for the LAST ONE, which tells the slave device that
//       you're about to STOP, or you can't receive any more data because 
//       a 'buffer' may be full.
//
//       For example, suppose we wish to access on I2C device that responds
//       to address 0x78 and obtain data from it, so naturally the 'controlling'
//       device must be in 'MASTER RECEIVE' mode:
//  
//              ...
//
//          I2C_MSTR_start( 0x78, I2C_MODE_MR );
//
//          I2C_MSTR_get( &data, TRUE );    // Get first byte...
//          I2C_MSTR_get( &data, TRUE );    // Get second byte...
//          I2C_MSTR_get( &data, FALSE );   // Get LAST byte.
//
//          I2C_MSTR_stop();
//
//              ...
//
//       Failure to follow this procedure will result in UNPREDICTABLE BEHAVIOR.
//
//       Please keep in mind that the above example is a short 'snippet' to
//       convey the idea.  Users should always verify the status codes returned
//       by the function to make sure the transaction is proceeding
//       without errors as it should.
//
//       The user must make sure to invoke 'I2C_MSTR_stop()' once the 
//       communication transaction is completed.  Or alternatively, 
//       the user can issue a 'restart' condition by invoking 
//       'I2C_MSTR_start()'.
//
//       For receiving multiple bytes back-to-back consider
//       using the 'I2C_MSTR_get_multiple()' function.
extern I2C_STATUS I2C_MSTR_get( unsigned char *pData, BOOL acknowledge );
// -------------------------------------------------------------------------- //
// Input  Args: 'pBuffer' - You pass to this argument *THE ADDRESS* of the
//                          destination buffer where data will be written to.
//              'count' - This is the number of bytes expected, which will be
//                        written to the buffer.  The user must make sure the
//                        size of the destination buffer is large enough to
//                        accomodate 'count' bytes, or otherwise program will
//                        be subject to unpredictable behaviour (i.e., it will
//                        most likely crash).
//
// Output Args: None.
//
// Globals  Read: None.
// Globals Write: None.
// 
// Returns: A value of type 'I2C_STATUS' will be returned by the function,
//          restricted specifically to one of the following below (note that
//          these are the same status codes as those returned by 
//          'I2C_MSTR_get()':
//
//              'I2C_STAT_OK' - If no problem has occurred.
//
//              'I2C_STAT_ACK_ERROR' - This status is returned when and ack-
//                                     nowledgement error has occurred.  This
//                                     normally happens when a different 
//                                     acknowedgment other than what was spec-
//                                     ified by the user has occurred.  Unlike
//                                     'I2C_STAT_NO_ACK', which may or may not
//                                     be considered an ERROR, this particular
//                                     status SHOULD be considered an ERROR.
//
//              'I2C_STAT_NOT_MASTER - This status was returned because the
//                                     device is not in MASTER mode as it should
//                                     have been (i.e., you forgot to perhaps
//                                     call 'I2C_MSTR_start()' first).  This is
//                                     considered an ERROR.
//
//              'I2C_STAT_CONFIG_ERROR' - A 'send' was NOT possible because
//                                        you have not configured the bit
//                                        rate generator via 'I2C_set_BRG()'
//                                        function.  Note that this may not
//                                        necessarily be an issue because the 
//                                        BRG is set up for 100KHz operation
//                                        when first opened by default.
//
// Desc: This function can be used to receive multiple bytes back-to-back
//       from an I2C device.  The number of bytes received is determined by
//       the value of 'count'.  As with 'I2C_MSTR_get()', a communication
//       transaction must have been previously initiated with the source device
//       via a prior call to 'I2C_MSTR_start()'.  Also, like 'I2C_MSTR_get()',
//       the user must make sure to invoke 'I2C_MSTR_stop()' to complete the
//       communication transaction.  Alternatively, the user may wish to issue
//       a 'restart' condition by invoking 'I2C_MSTR_start()'.
//
//       Unlike 'I2C_MSTR_get()', the function automatically takes care
//       of 'acknowledging' each byte received except for the last one.  
//       Therefore, the user is freed from having to worry about acknowledging
//       which is required when using 'I2C_MSTR_get()'.
//
//       It is *CRITICAL* that the value of 'count' does NOT exceed the
//       size of the buffer where data will be written to.  If the user violates
//       this condition, the data memory contents of your program will be 
//       trashed, resulting in UNPREDICTABLE behaviour, and possible program
//       crash.
extern I2C_STATUS I2C_MSTR_get_multiple( unsigned char *pBuffer,
                                         unsigned short int count );
// -------------------------------------------------------------------------- //
// Input  Args: None.
// Output Args: None.
//
// Globals  Read: 'I2C_params'
// Globals Write: 'I2C_params'
//
// Returns: A value of type 'I2C_STATUS' will be returned by the function,
//          restricted specifically to one of the following below:
//
//              'I2C_STAT_OK' - If no problem has occurred.
//
//              'I2C_STAT_NOT_MASTER - This status was returned because the
//                                     device is not in MASTER mode as it should
//                                     have been (i.e., you forgot to perhaps
//                                     call 'I2C_MSTR_start()' first).  This is
//                                     considered an ERROR.
//
// Desc: Function completes a currently executing communication transaction
//       previously started via 'I2C_MSTR_start()'.  This completes the 
//       communication sequences and allows a new one to be started (by 
//       a new call via 'I2C_MSTR_start()').
extern I2C_STATUS I2C_MSTR_stop( void );
// -------------------------------------------------------------------------- //
// Input  Args: 'slave_addr' - This is the address that is 'self-assigned' to
//                             the SLAVE I2C device.  That is, the address it
//                             will RESPOND TO, when addressed by a MASTER 
//                             device on the bus.  It must be a value between
//                             1 and 127 (0 is reserved for broadcasting). 
//                             You must make sure the address value is unique
//                             from other I2C slave devices on the I2C Bus.
// Ouptut Args: None.
//
// Globals  Read: (??)
// Globals Write: (??)
//
// Returns: A value of type 'I2C_STATUS' will be returned by the function,
//          restricted specifically to one of the following below:
//
//              'I2C_STAT_OK' - If no problem has occurred.
//
//              'I2C_STAT_BUSY' - This status code is returned if you attempt
//                                to disable the SLAVE device in the middle
//                                of a transaction, be it as MASTER or SLAVE.
//
// Desc: Function activates the 'slave' I2C device for reception.  This function
//       performs three tasks: first, it sets the slave address that the I2C 
//       device will respond to; second, it enables the ability of the I2C
//       to acknowledge its own addres ( when addressed by a MASTER I2C device
//       on the bus ); and third, it enables interrupts so
//       that the user's ISR will be triggered when a slave operation occurs.
//       Consequently, SLAVE operation is 'interrupt' driven, and thus, the
//       user must declare his/her own 'TWI ISR' via ISR() macro [ NOT the
//       'CBOT_ISR()' since the CEENBOT-API does not need to 'hijack' 
//       this particular ISR ].
//
//       ** NOTE that you can only call this function when you're NOT in the
//          middle of a communication transaction!  Make sure you check the
//          returned status code to make sure you get 'I2C_STAT_OK' back! **
//
extern I2C_STATUS I2C_SLVE_enable( unsigned char slave_addr );
// -------------------------------------------------------------------------- //
// Input  Args: None.
// Output Args: None.
//
// Globals  Read: (??)
// Globals Write: (??)
//
// Returns: A value of type 'I2C_STATUS' will be returned by the function,
//          restricted specifically to one of the following below:
//
//              'I2C_STAT_OK' - If no problem has occurred.
//
//              'I2C_STAT_BUSY' - This status code is returned if you attempt
//                                to disable the SLAVE device in the middle
//                                of a transaction, be it as MASTER or SLAVE.
//
// Desc: This function disables the slave portion of the I2C device.  That is,
//       it disables the I2C's ability to respond to its own device address
//       when an attempt is made for such request by a 'MASTER' device on the
//       I2C bus.  It also disables the generation of interrupts for the I2C
//       device, which are used during SLAVE operation of the same.
//
//       ** NOTE that as with 'I2C_SLVE_enable()', you CANNOT call this function
//          in the middle of a communication transaction.  You must wait until
//          any communication transactions complete before invoking this 
//          function.  Use the 'I2C_isBusy()' function to determine this.
//          Make sure you check the returned status code to make sure you get
//          'I2C_STAT_OK' back! **
extern I2C_STATUS I2C_SLVE_disable( void );
// -------------------------------------------------------------------------- //
// Input  Args: 'pData' - You must pass to this argument *THE ADDRESS* where
//                        data will be written to once received from a MASTER
//                        device while in SLAVE mode.
//
//              'acknowledge' - The value of this argument must be either
//                              'TRUE' or 'FALSE'.  If 'TRUE', the I2C SLAVE
//                              device will 'ACKNOWLEDGE' the MASTER each time
//                              it receives a byte to let the MASTER device 
//                              know it has indeed received it.  Otherwise,
//                              after byte has been received the SLAVE will
//                              NOT ACKNOWLEDGE to let the MASTER known that
//                              either it can't receive any more data, or that
//                              some sort of error has occurred.  Note that 
//                              under normal operation, you should ALWAYS 
//                              ACKNOWLEDGE EVERY DATA BYTE RECEIVED -- PERIOD.
// Output Args: None.
//
// Globals  Read: (??)
// Globals Write: (??)
//
// Returns: A value of type 'I2C_STATUS' will be returned by the function,
//          restricted specifically to one of the following below:
//
//              'I2C_STAT_OK' - If no problem has occurred.
//
//              'I2C_STAT_MASTER_BUSY' - This status is returned when a slave
//                                       operation is attempted while the 
//                                       I2C is in the middle of a communicat-
//                                       ion transaction in MASTER mode that has
//                                       yet to complete.
//
//              'I2C_STAT_NO_ACK' - This status shows when you requested a data
//                                  byte from the master (and got it), but YOU
//                                  (the SLAVE) refused to send an acknowledge
//                                  back.  When receiving data from a MASTER
//                                  device should normally always acknowledge
//                                  EVERY data byte received unless some 
//                                  exceptional reason prevents you from doing
//                                  so.  So under normal operation in 
//                                  SLAVE-RECEIVER mode, you should not see
//                                  status unless you purposely fail to ack-
//                                  nowledge.
//
//              'I2C_STAT_START_STOP_REQ' - This status is returned when the
//                                          MASTER has either completed a 
//                                          transaction or wishes to begin a
//                                          new one.  In any case, this status
//                                          is returned to inform you that you
//                                          should call 'I2C_SLVE_finished()'
//                                          immediately after.
//
//              'I2C_STAT_UNKNOWN_ERROR' - Guess what this means... ;o)
//
// Desc: This function *MUST* be used *INSIDE* of your TWI ISR to receive data
//       when operating as SLAVE and the MASTER is transferring byte data 
//       to you.  You should always 'ACKNOWLEDGE' each byte received unless 
//       there is some exceptional reason for you not to do so.
//
//       You *MUST* also always check the 'I2C_STATUS' value returned for each
//       byte received and determine when 'I2C_STAT_START_STOP_REQ' is returned
//       at which point you MUST follow this function with a call to
//       'I2C_SLVE_finished()' to complete the transaction.
extern I2C_STATUS I2C_SLVE_get( unsigned char *pData, BOOL acknowledge );
// -------------------------------------------------------------------------- //
// Input  Args: 'data' - The data byte to send the MASTER device.
//
//              'acknowledge_expected' - The value of this argument must be
//                              either 'TRUE' or 'FALSE'.  When the MASTER is
//                              RECEIVING DATA from a SLAVE, the I2C protocol
//                              states that the MASTER should NOT ACKNOWLEDGE
//                              the LAST BYTE received from the SLAVE as a 
//                              form of 'acknowledgement' to the SLAVE that it
//                              HAS received the LAST BYTE.  (See description
//                              below for additional comments).
//
// Output Args: None.
//
// Returns: A value of type 'I2C_STATUS' will be returned by the function,
//          restricted specifically to one of the following below:
//
//              'I2C_STAT_OK' - If no problem has occurred.
//
//              'I2C_STAT_MASTER_BUSY' - This status is returned when a slave
//                                       operation is attempted while the 
//                                       I2C is in the middle of a communicat-
//                                       ion transaction in MASTER mode that has
//                                       yet to complete.
//
//              'I2C_STAT_NO_ACK'    - MASTER device did NOT acknowleged prev-
//                                     ious data byte sent. Whether this is 
//                                     considered an error or not depends on 
//                                     context.  The MASTER may have received 
//                                     the byte but did not acknowledge on 
//                                     purpose to let you know its buffer is 
//                                     FULL and it cannot accept any more data.
//                                     Also, a NOT ACKNOWLEDGE can be expected
//                                     when the MASTER KNOWS that it has rec-
//                                     eived the last byte in a multi-byte
//                                     sequence.  This is traditionally the 
//                                     case when both MASTER and SLAVE are 
//                                     aware of the size of the multi-byte
//                                     sequence.
//
//                                     Alternatively, it could problably mean 
//                                     the MASTER did NOT receive the data 
//                                     after all.  It is up to the user to 
//                                     determine how to handle this particular 
//                                     status, and the reason its meaning
//                                     'context' sensitive.
//
//              'I2C_STAT_ACK_ERROR' - This status is returned when and ack-
//                                     nowledgement error has occurred.  This
//                                     normally happens when a different 
//                                     acknowedgment other than what was spec-
//                                     ified by the user has occurred.  Unlike
//                                     'I2C_STAT_NO_ACK', which may or may not
//                                     be considered an ERROR, this particular
//                                     status SHOULD be considered an ERROR.
//
//              'I2C_STAT_UNKNOWN_ERROR' - The name says it all.
//
// Desc: This function *MUST* be used *INSIDE* of your TWI ISR to send data
//       when operating as SLAVE and the MASTER is expecting data from you.
//       You must always 'ACKNOWLEDGE' each byte transmitted EXCEPT FOR THE
//       LAST ONE!
//
//       For example:
//
//              ...
//          I2C_SLVE_send( 0x34, TRUE );
//          I2C_SLVE_send( 0x00, TRUE );
//          I2C_SLVE_send( 0xFE, FALSE ); // Last byte.
//              ...
//
//       Etc.
//
//       You *MUST* also always check the 'I2C_STATUS' value returned for each
//       byte sent to make sure that everything went ok.  The last byte should
//       return 'I2C_STAT_NO_ACK' as confirmation, and you should use this as
//       an indication to complete the transaction by calling
//       'I2C_SLVE_finished()'.
//
extern I2C_STATUS I2C_SLVE_send( unsigned char data, 
                                                BOOL acknowledge_expected );
// -------------------------------------------------------------------------- //
// Input  Args: 'keep_enabled' - This argument must be either 'TRUE' or
//                               'FALSE'.  If 'TRUE', the I2C SLAVE portion will
//                               remain addressable by a MASTER device on the
//                               bus.  If 'FALSE', the SLAVE portion will no
//                               longer acknowledge its own address when 
//                               addressed by a MASTER on the I2C Bus.
//
// Output Args: None.
//
// Globals  Read: (??)
// Globals Write: (??)
//
// Returns: A value of type 'I2C_STATUS' will be returned by the function,
//          restricted specifically to one of the following below:
//
//              'I2C_STAT_OK' - If no problem has occurred.
//
//              'I2C_STAT_NOT_SLAVE' - This status was returned because the
//                                     device is not in SLAVE mode as it should
//                                     have been (i.e., perhaps you forgot to
//                                     a MASTER transaction by calling
//                                     'I2C_MSTR_stop()').
//
//
// Desc: This function *MUST* be called inside of your TWI ISR to indicate when
//       a current 'slave' transaction has completed.  It also gives the user
//       the opportunity to disable the SLAVE device from being addressed in
//       subsequent communication transactions if the user so wishes for
//       whatever reason.  The function is also used to mark the 'end' of 
//       a present communication transaction in 'SLAVE' mode.  Forgetting 
//       to invoke this function to complete a SLAVE communication transaction
//       will prevent the user from commencing a new transaction in MASTER
//       mode.
//
//       You *MUST* always check the 'I2C_STATUS' value returned by the function
//       to ensure no issues have arisen in the process.
extern I2C_STATUS I2C_SLVE_finished( BOOL keep_enabled );
// -------------------------------------------------------------------------- //
// Returns: Function returns a value of type 'I2C_STATUS', containing one of
//          the following enumerated constats:
//
//              'I2C_STAT_MASTER_WANTS_TO_RECV' - The master I2C device is ex-
//                                                pecting that you (the slave)
//                                                send it some data.
//
//              'I2C_STAT_MASTER_WATNS_TO_SEND' - The master I2C device is ex-
//                                                pecting that you (the slave)
//                                                will receive some data.
//
//              'I2C_STAT_MASTER_IS_FINISHED' - The master I2C device is letting
//                                              you (the slave) know that it is
//                                              finished with the current trans-
//                                              action, and that so should you.
//
//              'I2C_STAT_MASTER_UNKNOWN_REASON' - This is essentially an error,
//                                                 and you should treat it as
//                                                 such.
//
// Desc: You *MUST* call this function inside of your TWI ISR to determine the
//       REASON the interrupt was triggered in the first place.  This inform-
//       ation is necessary for you to determine whether you should invoke
//       'I2C_SLVE_get()' to read data or 'I2C_SLVE_send()' to send data.  
//       Essentially, this function should be the -first- function you invoke
//       inside of your TWI ISR to determine what your next 'move' inside of
//       the ISR should be.  For example:
//
//       ISR( TWI_vect )
//       {
//
//          I2C_STATUS i2c_stat;
//
//          // Get the reason we're here...
//          i2c_stat = I2C_SLVE_GetMasterReq();
//
//          // Proceed accordingly...
//          switch( i2c_stat )
//          {
//
//              case I2C_STAT_MASTER_WANTS_TO_RECV:
//
//                  // ... do something involving 'I2C_SLVE_send()'.
//
//              break;
//
//              case I2C_STAT_MASTER_WANTS_TO_SEND:
//
//                  // ... do something involving 'I2C_SLVE_get()'.
//
//              break;
//
//              case I2C_STAT_MASTER_IS_FINISHED:
//
//                  I2C_SLVE_finished( TRUE );
//
//              break;
//
//              default:
//
//                  // ... code if ERROR ...
//
//          } // end switch()
//
//       } // end ISR()
//
extern I2C_STATUS I2C_SLVE_GetMasterReq( void );
// -------------------------------------------------------------------------- //
// Input  Args: None.
// Output Args: None.
// 
// Globals  Read: 'I2C_params'
// Globals Write: None.
//
// Returns: A value of type 'BOOL', which will be either 'TRUE' or 'FALSE'
//          depending on whether the I2C subsystem is busy either as MASTER,
//          SLAVE or BOTH.
//
// Desc: Function can be used to check whether the I2C device is in the middle
//       of a communication transaction either as MASTER or SLAVE.  The function
//       will return 'TRUE' if this is the case.  It is particularly useful
//       to determine the status of the I2C subsystem right before you open,
//       close, or enable/disable, or initiate/terminate a communication
//       transaction gracefully.
extern BOOL I2C_IsBusy( void );

// ======================= external declarations ============================ //
extern I2C_PARAMS I2C_params;

#endif /* __I2C324V221_H__ */
