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
// Desc: Header file for the SPKR subsystem module.

#ifndef __SPKR324V221_H__
#define __SPKR324V221_H__


#include <avr/io.h>
#include <stdlib.h>
#include <math.h>

#include "sys324v221.h"
#include "utils324v221.h"
#include "isr324v221.h"
#include "tmrsrvc324v221.h"


// =============================== defines ================================== //
// Desc: The following macro is used to convert from Hz to Hz*10 as required
//       by the 'SPKR_tone()' function.  The reason this is needed is because
//       there is NO-floating arithmetic performed in the SPKR module, and
//       floating-point (decimal) accuracy is achieved by 'clever' integer
//       scaling.
//
#define SPKR_FREQ( f )  ( ( SPKR_FREQ )(( f ) * 10L ))

// Desc: TODO
#define SPKR_DIATONE( n )   ( diatonic_LUT[ n ] )

#define SPKR_stop_tone()    SPKR_tone( 0 )
#define SPKR_stop_beep()    SPKR_beep( 0 )

// Some useful constants.
#define SEMITONES_FROM_ROOT_C0  72
#define MAX_OCTAVE_VALUE        5   /* Starting from 0, so 6 total. */
#define MAX_TRANSPOSE_VALUE     12
#define MIN_TRANSPOSE_VALUE     -12

// Important frequency-related constants.
#define ROOT_NOTE_FREQ      16.35
#define SEMITONE_CONSTANT   0.5018f
#define WHOLETONE_CONSTANT  1.0036f

// **Predefined Frequency Table:

// Octave 0:
#define SPKR_TONE_C0    SPKR_FREQ( 16.35 )
#define SPKR_TONE_C0_S  SPKR_FREQ( 17.32 )
#define SPKR_TONE_D0    SPKR_FREQ( 18.35 )
#define SPKR_TONE_D0_S  SPKR_FREQ( 19.45 )
#define SPKR_TONE_E0    SPKR_FREQ( 20.60 )
#define SPKR_TONE_F0    SPKR_FREQ( 21.83 )
#define SPKR_TONE_F0_S  SPKR_FREQ( 23.12 )
#define SPKR_TONE_G0    SPKR_FREQ( 24.50 )
#define SPKR_TONE_G0_S  SPKR_FREQ( 25.96 )
#define SPKR_TONE_A0    SPKR_FREQ( 27.50 )
#define SPKR_TONE_A0_S  SPKR_FREQ( 29.14 )
#define SPKR_TONE_B0    SPKR_FREQ( 30.87 )

// Octave 1:
#define SPKR_TONE_C1    SPKR_FREQ( 32.7 )
#define SPKR_TONE_C1_S  SPKR_FREQ( 34.65 )
#define SPKR_TONE_D1    SPKR_FREQ( 36.71 )
#define SPKR_TONE_D1_S  SPKR_FREQ( 38.89 )
#define SPKR_TONE_E1    SPKR_FREQ( 41.20 )
#define SPKR_TONE_F1    SPKR_FREQ( 43.65 )
#define SPKR_TONE_F1_S  SPKR_FREQ( 46.25 )
#define SPKR_TONE_G1    SPKR_FREQ( 49.00 )
#define SPKR_TONE_G1_S  SPKR_FREQ( 51.91 )
#define SPKR_TONE_A1    SPKR_FREQ( 55 )
#define SPKR_TONE_A1_S  SPKR_FREQ( 58.27 )
#define SPKR_TONE_B1    SPKR_FREQ( 51.74 )

// Octave 2:
#define SPKR_TONE_C2    SPKR_FREQ( 65.41 )
#define SPKR_TONE_C2_S  SPKR_FREQ( 69.30 )
#define SPKR_TONE_D2    SPKR_FREQ( 73.42 )
#define SPKR_TONE_D2_S  SPKR_FREQ( 77.78 )
#define SPKR_TONE_E2    SPKR_FREQ( 82.41 )
#define SPKR_TONE_F2    SPKR_FREQ( 87.31 )
#define SPKR_TONE_F2_S  SPKR_FREQ( 92.50 )
#define SPKR_TONE_G2    SPKR_FREQ( 98.00 )
#define SPKR_TONE_G2_S  SPKR_FREQ( 103.83 )
#define SPKR_TONE_A2    SPKR_FREQ( 110 )
#define SPKR_TONE_A2_S  SPKR_FREQ( 116.54 )
#define SPKR_TONE_B2    SPKR_FREQ( 123.47 )

// Octave 3:
#define SPKR_TONE_C3    SPKR_FREQ( 130.81 )
#define SPKR_TONE_C3_S  SPKR_FREQ( 138.59 )
#define SPKR_TONE_D3    SPKR_FREQ( 146.83 )
#define SPKR_TONE_D3_S  SPKR_FREQ( 155.56 )
#define SPKR_TONE_E3    SPKR_FREQ( 164.81 )
#define SPKR_TONE_F3    SPKR_FREQ( 174.61 )
#define SPKR_TONE_F3_S  SPKR_FREQ( 185.00 )
#define SPKR_TONE_G3    SPKR_FREQ( 196.00 )
#define SPKR_TONE_G3_S  SPKR_FREQ( 207.65 )
#define SPKR_TONE_A3    SPKR_FREQ( 220 )
#define SPKR_TONE_A3_S  SPKR_FREQ( 233.08 )
#define SPKR_TONE_B3    SPKR_FREQ( 246.94 )

// Octave 4:
#define SPKR_TONE_C4    SPKR_FREQ( 261.63 )
#define SPKR_TONE_C4_S  SPKR_FREQ( 277.18 )
#define SPKR_TONE_D4    SPKR_FREQ( 293.66 )
#define SPKR_TONE_D4_S  SPKR_FREQ( 311.13 )
#define SPKR_TONE_E4    SPKR_FREQ( 329.63 )
#define SPKR_TONE_F4    SPKR_FREQ( 349.23 )
#define SPKR_TONE_F4_S  SPKR_FREQ( 369.99 )
#define SPKR_TONE_G4    SPKR_FREQ( 392.00 )
#define SPKR_TONE_G4_S  SPKR_FREQ( 415.30 )
#define SPKR_TONE_A4    SPKR_FREQ( 440.00 )
#define SPKR_TONE_A4_S  SPKR_FREQ( 466.16 )
#define SPKR_TONE_B4    SPKR_FREQ( 493.88 )

// Octave 5:
#define SPKR_TONE_C5    SPKR_FREQ( 523.25 )
#define SPKR_TONE_C5_S  SPKR_FREQ( 554.37 )
#define SPKR_TONE_D5    SPKR_FREQ( 587.33 )
#define SPKR_TONE_D5_S  SPKR_FREQ( 622.25 )
#define SPKR_TONE_E5    SPKR_FREQ( 659.26 )
#define SPKR_TONE_F5    SPKR_FREQ( 698.46 )
#define SPKR_TONE_F5_S  SPKR_FREQ( 739.99 )
#define SPKR_TONE_G5    SPKR_FREQ( 783.99 )
#define SPKR_TONE_G5_S  SPKR_FREQ( 830.61 )
#define SPKR_TONE_A5    SPKR_FREQ( 880.00 )
#define SPKR_TONE_A5_S  SPKR_FREQ( 932.33 )
#define SPKR_TONE_B5    SPKR_FREQ( 987.77 )

// ========================== type declarations ============================= //
// Desc: Custom enumerated type declaration for specifying the mode in which
//       the SPKR module should be opened.
typedef enum SPKR_MODE_TYPE {

    SPKR_DISABLED_MODE = 0, // Speaker module is not enabled.
    SPKR_BEEP_MODE,         // Simple mode for 'beeps' only.
    SPKR_TONE_MODE          // Advanced mode for 'musical notes'.

} SPKR_MODE;

// Desc: Custom type declaration for specifying the speaker frequency.
typedef signed long int SPKR_FREQ;

// Desc: Custom type declaration for specifying delays from the SPKR context.
typedef TIMER16 SPKR_TIME;

// Desc: Custom type declarations for specifying musical notes via
//       'SPKR_note()' function. Note this corresponds to the 'chromatic'
//       musical scale (12 tones).
typedef enum SPKR_NOTE_TYPE {

    SPKR_NOTE_C = 0,
    SPKR_NOTE_C_S,
    SPKR_NOTE_D,
    SPKR_NOTE_D_S,
    SPKR_NOTE_E,
    SPKR_NOTE_F,
    SPKR_NOTE_F_S,
    SPKR_NOTE_G,
    SPKR_NOTE_G_S,
    SPKR_NOTE_A,
    SPKR_NOTE_A_S,
    SPKR_NOTE_B,
    SPKR_NOTE_NONE

} SPKR_NOTE;    

// Desc: Custom type declaration for specifying notes of the diatonic
//       scale only.  These values are meant to be used with the
//       'SPKR_map_diatone()' function.
typedef enum SPKR_DNOTE_TYPE {

    SPKR_DNOTE_C = 0,
    SPKR_DNOTE_D,
    SPKR_DNOTE_E,
    SPKR_DNOTE_F,
    SPKR_DNOTE_G,
    SPKR_DNOTE_A,
    SPKR_DNOTE_B

} SPKR_DNOTE;

// Desc: Custom type declaration for specifying 'octaves' via 'SPKR_note()'
//       function.
typedef enum SPKR_OCTV_TYPE {

    SPKR_OCTV0 = 0,
    SPKR_OCTV1,
    SPKR_OCTV2,
    SPKR_OCTV3,
    SPKR_OCTV4,
    SPKR_OCTV5

} SPKR_OCTV;

typedef enum SPKR_PLAYMODE_TYPE {

    SPKR_ONE_SHOT = 0,
    SPKR_LOOP

} SPKR_PLAYMODE;

// Desc: Custom type declaration for specifying playable musical notes.
typedef struct SPKR_PLAYNOTE_TYPE {

    unsigned char note;             // Specifies a speaker note.
    unsigned char octave;           // Octave.
      signed char transp;           // Transposition (if any).
    SPKR_TIME duration_ms;          // Specifies overall note duration.
    unsigned char len;              // % of 'duration' for which note is 
                                    //  'audible'.

} SPKR_PLAYNOTE;

// Desc: Custom type declaration for specifying a musical 'measure'.
typedef struct SPKR_MEASURE_TYPE {

    SPKR_PLAYNOTE *pNotes;   // Points to array of 'SPKR_PLAYNOTE' notes.
    unsigned char num_notes; // Number of notes in 'pNotes'.
    unsigned char times;    // Number of times to repeat each measure.

} SPKR_MEASURE;

// Desc: Custom type declaration for specifying a 'song'.
typedef struct SPKR_SONG_TYPE {

    SPKR_MEASURE *pMeasures;    // Points to array of 'MEASURES'.
    unsigned char num_measures; // Number of measures in 'song'.
    unsigned char times;       // Number of times to repeat the song.

} SPKR_SONG;

// ============================== prototypes ================================ //
// Input  Args: 'spkr_mode' - Must be one of the following enumerated constants:
//                            
//                              'SPKR_BEEP_MODE' - For 'beep-only' mode.
//                              'SPKR_TONE_MODE' - For generating musical notes.
//
// Output Args: None.
//
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
// Desc: Function opens and initializes the SPKR subsystem module.  You MUST
//       specify the mode in which the SPKR subsystem module is to open.  It 
//       is possible to call 'SPKR_open()' twice (once per mode) to open both
//       modes simultaneously (BEEP and TONE).
//
//       When opening the SPKR subsystem TONE mode, there is a dependency
//       on the 16-bit timer, which can be potentially in use by another
//       subsystem module (such as the STOPWATCH subsystem module).  If this
//       resource is not available then initialization will fail (most likely
//       a 'SUBSYS_DEPENDENCY_ERROR' will be issued).
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
extern SUBSYS_STATUS SPKR_open( SPKR_MODE spkr_mode );
// -------------------------------------------------------------------------- //
// Input  Args: 'spkr_mode' - Must be one of the following enumerated constants:
//                            
//                              'SPKR_BEEP_MODE' - For 'beep-only' mode.
//                              'SPKR_TONE_MODE' - For generating musical notes.
//
// Desc: Function closes and releases the SPKR subsystem module.  There MUST
//       be a corresponding 'SPKR_close()' for each 'SPKR_open()' for a given
//       mode.  If you call 'SPKR_open()' to start both modes, then there must
//       be (eventually) two corresponding calls to 'SPKR_close()' (one for 
//       each mode previously open) to close them both.
extern void SPKR_close( SPKR_MODE spkr_mode );
// -------------------------------------------------------------------------- //
// Input  Args: 'tone_freq' -- This is the 'tone frequency' value which MUST
//                             be multiplied by 10.  For example, if you
//                             wish to emit a tone at 440Hz, then you must
//                             supply 4400 as a value to this function.
//
//                             A value of 0 will stop the TONE generation.
//                             (that is, it will turn OFF the tone).
//
// Output Args: None.
// Globals Written: 'spkr_freq' - This function writes to the 'spkr_freq' 
//                                global value used by the timing system to
//                                generate the tone.
//
// Globals Read: None.
//
// Returns: Nothing.
//
// Desc: Function can be used to emit a tone at a given frequency specified
//       by 'tone_freq'.  Note that the value supplied for this parameter must
//       be the actual frequency multiplied by 10.  For example, if we wish
//       to emit A4 which is ~440Hz, then you must supply 4400 as the parameter.
//       
//          SPKR_tone( 4400 );
//
//       Alternatively, you can use the 'SPKR_FREQ()' macro which does this
//       for you.
//
//          SPKR_tone( SPKR_FREQ( 400 ) );
//
//       Calling the function again with a value of '0' for 'tone_freq' will
//       turn the previously emitting tone OFF.
//
extern void SPKR_tone( SPKR_FREQ tone_freq );
// -------------------------------------------------------------------------- //
// Input Args: 'tone_freq' - This is the 'tone_frequency' value which MUST
//                            be multiplied by 10.  For example, if you wish to
//                            emit a tone at 440Hz, then you must supply 
//                            4400 as a value to this function (See below).
// Output Args: None.
//
// Globals Written: 'spkr_freq' - This function writes to the 'spkr_freq'
//                                global value used by the timing system to
//                                generate the tone.
//
// Globals Read: None.
//
// Returns: Nothing.
//
// Desc: Function plays a tone at the specified frequency.  The tone will 
//       occupy a duration specified by 'duration_ms', however it will only
//       remain active for a percentage of that duration as specified by 'len'.
//
//       The 'tone_freq' value must be the value desired multiplied times 10.
//       You can use the 'SPKR_FREQ()' macro to do the conversion for you, so
//       you can either do:
//
//          SPKR_play_tone( ( 440 * 10 ), 250, 90 );
//
//      Or...
//
//          SPKR_play_note( SPKR_FREQ( 440 ), 250, 90 );
//
//
extern void SPKR_play_tone( SPKR_FREQ tone_freq, 
                            SPKR_TIME duration_ms, 
                            unsigned short int len );
// -------------------------------------------------------------------------- //
// Input Args: note - This must be a value between 0 and 11 representing 
//                    all the 12 tones in the chromatic scale for any given
//                    octave:
//
//                      0 = C       (SPKR_NOTE_C)
//                      1 = C#      (SPKR_NOTE_C_S)
//                      2 = D       (SPKR_NOTE_D)
//                      3 = D#      (SPKR_NOTE_D_S)
//                      4 = E       (SPKR_NOTE_E)
//                      5 = F       (SPKR_NOTE_F)
//                      6 = F#      (SPKR_NOTE_F_S)
//                      7 = G       (SPKR_NOTE_G)
//                      8 = G#      (SPKR_NOTE_G_S)
//                      9 = A       (SPKR_NOTE_A)
//                     10 = A#      (SPKR_NOTE_A_S)
//                     11 = B       (SPKR_NOTE_B)
//
//                  Instead of numerical values, you can also specify the
//                  predefined macro-constants (given in parenthesis above).
//                  Note that these numerical values map linearly to the entire
//                  chromatic scale.  Sometimes, however, you'll just want to
//                  specify a linear mapping that corresponds to the diatonic
//                  scale (i.e., white keys on the piano only).  For this, you
//                  can use the 'SPKR_map_diatone()' conversion function, which
//                  re-maps the values 0 - 6 to the notes C, D, E, F, G, A, B 
//                  respectively.  (See example below).
//
//              octave - This must be a value between 0 and 5 (6 octaves total):
//
//                    0 = Octave 0  (SPKR_OCTV0)
//                    1 = Octave 1  (SPKR_OCTV1)
//                    2 = Octave 2  (SPKR_OCTV2)
//                    3 = Octave 3  (SPKR_OCTV3)  [ Middle Octave ]
//                    4 = Octave 4  (SPKR_OCTV4)
//                    5 = Octave 5  (SPKR_OCTV5)
//
//                  Instead of the numerical values, you can also specify the
//                  predefined macro-constants (given in parenthesis above).
//
//              transp - This can be a value between -12 to 12, representing
//                       the number of 'semitones' to shift the scale by.  A
//                       positive value will shift the notes by that many
//                       semitones, and a negative will shift it down.
//
// Desc: Play a musical note from the chromatic scale at the specified octave
//       and given transposition.
//
// Example:
//
//          To play Middle-C:
//
//              SPKR_tone( SPKR_NOTE_C, SPKR_OCTV3, 0 );
//
//          To play Middle-D:
//
//              SPKR_tone( SPKR_NOTE_D, SPKR_OCTV3, 0 );
//
//          To transpose Middle-C by 4 semitones up (so that C sounds like E):
//
//              SPKR_tone( SPKR_NOTE_C, SPKR_OCTV3, 4 );
//
//          To play all tones in middle octave in succession (chromatic scale):
//
//              for( i = 0; i < 13; ++i )
//              {
//
//                  SPKR_tone( i, SPKR_OCTV3, 0 );
//                  TMRSRVC_delay( 500 );            // Delay before next note.
//
//              }
//
//
//          To play all 'diatonic' ONLY tones in middle octave in succession:
//
//              for( i = 0; i < 8; ++i )
//              {
//                  
//                  SPKR_tone( SPKR_map_diatone( i ), SPKR_OCTV3, 0 );
//                  TMRSRVC_delay( 500 );
//
//              }
//
extern void SPKR_note( SPKR_NOTE note, SPKR_OCTV octave, 
                                            signed short int transp );
// -------------------------------------------------------------------------- //
// Input Args: note - This must be a value between 0 and 11 representing 
//                    all the 12 tones in the chromatic scale for any given
//                    octave:
//
//                      0 = C       (SPKR_NOTE_C)
//                      1 = C#      (SPKR_NOTE_C_S)
//                      2 = D       (SPKR_NOTE_D)
//                      3 = D#      (SPKR_NOTE_D_S)
//                      4 = E       (SPKR_NOTE_E)
//                      5 = F       (SPKR_NOTE_F)
//                      6 = F#      (SPKR_NOTE_F_S)
//                      7 = G       (SPKR_NOTE_G)
//                      8 = G#      (SPKR_NOTE_G_S)
//                      9 = A       (SPKR_NOTE_A)
//                     10 = A#      (SPKR_NOTE_A_S)
//                     11 = B       (SPKR_NOTE_B)
//
//                  Instead of numerical values, you can also specify the
//                  predefined macro-constants (given in parenthesis above).
//                  Note that these numerical values map linearly to the entire
//                  chromatic scale.  Sometimes, however, you'll just want to
//                  specify a linear mapping that corresponds to the diatonic
//                  scale (i.e., white keys on the piano only).  For this, you
//                  can use the 'SPKR_map_diatone()' conversion function, which
//                  re-maps the values 0 - 6 to the notes C, D, E, F, G, A, B 
//                  respectively.  (See example below).
//
//              octave - This must be a value between 0 and 5 (6 octaves total):
//
//                    0 = Octave 0  (SPKR_OCTV0)
//                    1 = Octave 1  (SPKR_OCTV1)
//                    2 = Octave 2  (SPKR_OCTV2)
//                    3 = Octave 3  (SPKR_OCTV3)  [ Middle Octave ]
//                    4 = Octave 4  (SPKR_OCTV4)
//                    5 = Octave 5  (SPKR_OCTV5)
//
//                  Instead of the numerical values, you can also specify the
//                  predefined macro-constants (given in parenthesis above).
//
//              transp - This can be a value between -12 to 12, representing
//                       the number of 'semitones' to shift the scale by.  A
//                       positive value will shift the notes by that many
//                       semitones, and a negative will shift it down.
//
//              duration_ms - The number of milliseconds that the given note
//                            will occupy regardless of whether the note is
//                            active (audible) or not.  The maximum value is
//                            32767 milliseconds (32.767 seconds).
//
//              len - An integer between 0 to 100 representing the percentage
//                    of the 'duration_ms' for which the note will be active
//                    (that is, audible).
//
// Desc: This function represents the highest-level musical tone generating
//       function.  It is used to play a 'note' a the specified 'octave' and
//       given 'transposition' for a length of time within a given duration.
//
//       The 'duration_ms' parameter is used to control the length of time that
//       the musical note will 'occupy'.  However, this alone does not dictate
//       for how long the given note is actually 'active' or 'audible'.  
//       That part is controlled by the 'len' parameter, which represents 
//       a percentage (0% to 100%) of the 'duration' given for which the note 
//       will be active.
//
//       For example, if you specify a note for a duration of 500ms, and an
//       'active' length of 75%, then the note will remain on for 375ms (75%
//       of 500), and inactive (silent) for the remaining 125ms, etc.
//
// Example:
//
//       The follwing 'for()' loop will play melodic interval C-E-G three times
//       , with each note staying ON for 80% of the 250ms interval:
//
//          for( i = 0; i < 3; ++i )
//          {
//
//              SPKR_play_note( SPKR_NOTE_C, SPKR_OCTV3, 0, 250, 80 );
//              SPKR_play_note( SPKR_NOTE_E, SPKR_OCTV3, 0, 250, 80 );
//              SPKR_play_note( SPKR_NOTE_G, SPKR_OCTV3, 0, 250, 80 );
//
//          }
//
//       In contranst if we want each note to still occupy 250ms, but be active
//       for a very short time to simulate 'staccato' notes, you would have
//       the notes active, for say 20% of the time:
//
//          for( i = 0; i < 3; ++i )
//          {
//
//              SPKR_play_note( SPKR_NOTE_C, SPKR_OCTV3, 0, 250, 20 );
//              SPKR_play_note( SPKR_NOTE_E, SPKR_OCTV3, 0, 250, 20 );
//              SPKR_play_note( SPKR_NOTE_G, SPKR_OCTV3, 0, 250, 20 );
//
//          }
//
//       Etc.
//
extern void SPKR_play_note( SPKR_NOTE note, SPKR_OCTV octave, 
                            signed short int transp, 
                            SPKR_TIME duration_ms,  unsigned short int len );
// -------------------------------------------------------------------------- //
// Desc: Function plays a 'song'.  A 'song' is a properly constructed structure
//       of various elements.  First you take a collection of notes called
//       'play notes' which constitutes the notes of your song.  Then you 
//       collect these into 'measures'.  You can construct as many measures as
//       you want (or more precisely -- as memory will allow).  Then you collect
//       this measures into one cohesive  unit called a 'song'.  This 'song' is
//       what you pass to this function.  Then function then goes measure-by-
//       measure and plays the notes in each measure.  An example is given below
//       which makes the procedure clear.
//
// Example: The following example gives a simple example on constructing the
//          song.  Say we want to play the notes, C-E-G (Melodic C-Major) twice
//          followed by the notes F-A-C (Melodic F-Major) 3 times:
//
//              // First construct the collection of notes.
//              SPKR_PLAYNOTE measure_1[] = {
//
//                  //  <note>,       <octv>   <transp> <duration> <len>
//                  { SPKR_NOTE_C, SPKR_OCTV3,    0,       250,      80 },
//                  { SPKR_NOTE_E, SPKR_OCTV3,    0,       250,      80 },
//                  { SPKR_NOTE_G, SPKR_OCTV3,    0,       250,      20 }
//
//              };
//
//              SPKR_PLAYNOTE measure_2[] = {
//
//                  //  <note>,       <octv>   <transp> <duration> <len>
//                  { SPKR_NOTE_F, SPKR_OCTV3,    0,       250,      80 },
//                  { SPKR_NOTE_A, SPKR_OCTV3,    0,       250,      80 },
//                  { SPKR_NOTE_C, SPKR_OCTV3,    0,       250,      20 }
//
//              };
//
//              // Next we collect our measures together.
//              SPKR_MEASURE measures[] = {
//
//                  // <playnotes> <num_notes> <repeat>
//                  { measure_1,       3,          2 },
//                  { measure_2,       3,          3 }
//
//              };
//
//              // Finally construct the 'Song' itself.
//              SPKR_SONG   song[] = {
//
//                  // <measures>   <num_measures>  <repeat>
//                  { measures,          2,            1 }
//
//              };
//
//              // With the 'song' construcuted, we can now play it.
//              SPKR_play_song( song );
//
//          Note in the example there are three principal ingredients:
//
//              1) One or more arrays of 'SPKR_PLAYNOTE' structures.  In the 
//                 above example, we declared two such arrays, each one repre-
//                 senting a 'measure'.  Note the parameters for each entry
//                 in the 'SPKR_PLAYNOTE' structure follows the exact same 
//                 convention as the arguments passed to the 'SPKR_play_note()'
//                 function.  Refer to that function for detailed info on 
//                 the meaning of each entry.
//
//              2) One or more arrays of 'SPKR_MEASURE' structures.  In the
//                 above example we collected the individual 'measures' into
//                 a single collection of 'measures'.  The first column is
//                 the name of the 'SPKR_PLAYNOTE' structure, the second is the
//                 number of notes (3 in each case), and the third is the number
//                 of times that each measure should be repeated.  In our case
//                 we want the first measure repeated twice, and the second 
//                 repeated 3 times.  So the notes will play:
//
//                      C-E-G-C-E-G-F-A-C-F-A-C-F-A-C.
//
//              3) An array of 'SPKR_SONG' structures containing your collection
//                 of measures, however number you may have.  In our example,
//                 we have a single such container, hence only one entry.
//                 The first column lists the name of the 'meausures' array,
//                 while the second the number of such measures contained in
//                 it, and the last one, the number of times to repeat the
//                 collection of measures.
//
//                 Obviously this means there are two opportunities for repet-
//                 ition.  One is on a 'per-measure' scale, and another one
//                 on a 'per-song' scale.
//              
// 
extern void SPKR_play_song( SPKR_SONG *pSong );
// -------------------------------------------------------------------------- //
extern SPKR_NOTE SPKR_map_diatone( SPKR_DNOTE dia_note );
// -------------------------------------------------------------------------- //
// Desc: Function takes care of the 'timing' aspects needed to generate beeps
//       at various frequencies using DDS.  It is placed here 'publicly' so
//       that it can be referenced by the timer service.
extern void SPKR_beep_clk( void );
// -------------------------------------------------------------------------- //
// Input Args: 'beep_frequency' - Enter a 'ball-park' frequency value between
//                                0 and 500 Hz.  A value of '0' will turn the
//                                BEEP OFF.  
//
//                                (Hidden Feature:  You can enter values larger
//                                 than 500 to emit a 500Hz-tone that 
//                                 toggles ON and OFF at various speeds.  You'll
//                                 have to 'experiment' to hear what sort of
//                                 'pattern' you get, but the higher above 500
//                                 Hz you go, the faster the toggling pattern
//                                 of the tone).
//
// Desc: Function can be used to emit a beep at a given frequency specified
//       by 'beep_freq'.  Note that only integer values can be used, and thus
//       the function is only useful for emitting beeps at arbitrary frequencies
//       but not useful for generating the full range of 'musical notes'.  For
//       that you have to use the SPKR module in 'TONE MODE'.  This function,
//       in contrast, can only be used in 'BEEP MODE'.
//
//       Unlike the 'TONE-MODE' functions, which require frequency values to
//       be scaled by 10 (using the 'SPKR_FREQ()' macro-function), you do NOT
//       have to scale the frequency values for this fuction.
extern void SPKR_beep( SPKR_FREQ beep_frequency );
// -------------------------------------------------------------------------- //
// Desc: This function is similar to the 'SPKR_play_note()' function except
//       it is not very accurate and is meant for generating beeps with 
//       'ball-park' frequency values.  Like 'SPKR_play_note()', 'duration_ms'
//       specifies the duration of time for which the given beep will occupy
//       (whether audible/active or not).  The 'len' parameter specifies the
//       percentage of the 'duration' for which the beep will be audible.
//
//       For example, if you specify 500ms beep, and 75% length, then the beep
//       will be audible for 375ms (active), and remain quiet for the remaining
//       125ms.
extern void SPKR_play_beep( SPKR_FREQ beep_frequency,
                            SPKR_TIME duration_ms,
                            unsigned short int len );

// ======================== external declarations =========================== //
extern volatile BOOL tone_mode_active;          // Denotes 'TONE' mode's active.
extern volatile BOOL beep_mode_active;          // Denotes 'BEEP' mode's active.

// Used in 'beep-mode':
extern volatile signed short int beep_accum16;  // DDS speaker accumulator.
extern volatile signed short int beep_freq;     // Beep frequency (in Hz).

// Used in 'tone-mode':
extern volatile signed long int spkr_accum32;   // DDS speaker accumulator.
extern volatile signed long int spkr_freq;      // Tone freq (times 10).
extern const SPKR_FREQ note_LUT[ 12 ];          // Musical note LUT relative
                                                // Octave 3.

extern const unsigned char diatonic_LUT[ 8 ];

extern SPKR_FREQ  *p_Freq_LUT;                  // Dynamically generated freq.
                                                // look-up-table for musical
                                                // notes in increments of 
                                                // semitones from C0 to B5 (
                                                // 6 octaves).

#endif /* __SPKR324V221_H__ */
