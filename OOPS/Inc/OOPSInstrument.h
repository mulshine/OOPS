/*
  ==============================================================================

    OOPSInstrument.h
    Created: 20 Jan 2017 12:01:54pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef OOPSINSTRUMENT_H_INCLUDED
#define OOPSINSTRUMENT_H_INCLUDED

#include "OOPSMath.h"
#include "OOPSCore.h"

/* tTalkbox */
tTalkbox*   tTalkboxInit        (void);
float       tTalkboxTick        (tTalkbox* const, float synth, float voice);
void        tTalkboxUpdate      (tTalkbox* const);
void        tTalkboxSuspend     (tTalkbox* const);
void        tTalkboxLpcDurbin   (float *r, int p, float *k, float *g);
void        tTalkboxLpc         (float *buf, float *car, int32_t n, int32_t o);



/* tVocoder */
tVocoder*   tVocoderInit        (void);
float       tVocoderTick        (tVocoder* const, float synth, float voice);
void        tVocoderUpdate      (tVocoder* const);
void        tVocoderSuspend     (tVocoder* const);

/* tPluck */
tPluck*     tPluckInit          (float lowestFrequency, float delayBuff[DELAY_LENGTH]);
float       tPluckTick          (tPluck*  const);

// Pluck the string.
void        tPluckPluck         (tPluck*  const, float amplitude);

// Start a note with the given frequency and amplitude.;
void        tPluckNoteOn        (tPluck*  const, float frequency, float amplitude );

// Stop a note with the given amplitude (speed of decay).
void        tPluckNoteOff       (tPluck*  const, float amplitude );

// Set instrument parameters for a particular frequency.
void        tPluckSetFrequency  (tPluck*  const, float frequency );

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void        tPluckControlChange (tPluck*  const, int number, float value);

// tPluck Utilities.
float       tPluckGetLastOut    (tPluck*  const);

/* tStifKarp */
typedef enum SKControlType
{
    SKPickPosition = 0,
    SKStringDamping,
    SKDetune,
    SKControlTypeNil
} SKControlType;

tStifKarp*  tStifKarpInit               (float lowestFrequency, float delayBuff[2][DELAY_LENGTH]);
float       tStifKarpTick               (tStifKarp*  const);

// Pluck the string.
void        tStifKarpPluck              (tStifKarp*  const, float amplitude);

// Start a note with the given frequency and amplitude.;
void        tStifKarpNoteOn             (tStifKarp*  const, float frequency, float amplitude );

// Stop a note with the given amplitude (speed of decay).
void        tStifKarpNoteOff            (tStifKarp*  const, float amplitude );

// Set instrument parameters for a particular frequency.
void        tStifKarpSetFrequency       (tStifKarp*  const, float frequency );

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
// Use SKPickPosition, SKStringDamping, or SKDetune for type.
void        tStifKarpControlChange      (tStifKarp*  const, SKControlType type, float value);

// Set the stretch "factor" of the string (0.0 - 1.0).
void        tStifKarpSetStretch         (tStifKarp*  const, float stretch );

// Set the pluck or "excitation" position along the string (0.0 - 1.0).
void        tStifKarpSetPickupPosition  (tStifKarp*  const, float position );

// Set the base loop gain.
void        tStifKarpSetBaseLoopGain    (tStifKarp*  const, float aGain );

// tStifKarp utilities.
float       tStifKarpGetLastOut         (tStifKarp*  const);


#endif  // OOPSINSTRUMENT_H_INCLUDED
