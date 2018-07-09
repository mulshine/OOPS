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

// 808 Cowbell
t808Cowbell*    t808CowbellInit             (void);
float           t808CowbellTick             (t808Cowbell* const);

void            t808CowbellOn               (t808Cowbell* const, float vel);
void            t808CowbellSetDecay         (t808Cowbell* const, float decay);
void            t808CowbellSetHighpassFreq  (t808Cowbell* const, float freq);
void            t808CowbellSetBandpassFreq  (t808Cowbell* const, float freq);
void            t808CowbellSetFreq          (t808Cowbell* const, float freq);
void            t808CowbellSetOscMix        (t808Cowbell* const, float oscMix);


// 808 Hihat
t808Hihat*  t808HihatInit               (void);
float       t808HihatTick               (t808Hihat* const);

void        t808HihatOn                 (t808Hihat* const, float vel);
void        t808HihatSetOscNoiseMix     (t808Hihat* const, float oscNoiseMix);
void        t808HihatSetDecay           (t808Hihat* const, float decay);
void        t808HihatSetHighpassFreq    (t808Hihat* const, float freq);
void        t808HihatSetOscBandpassFreq (t808Hihat* const, float freq);
void        t808HihatSetOscFreq         (t808Hihat* const, float freq);


// 808 Snare
t808Snare*  t808SnareInit               (void);
float       t808SnareTick               (t808Snare* const);

void        t808SnareOn                 (t808Snare* const, float vel);
void        t808SnareSetTone1Freq       (t808Snare* const, float freq);
void        t808SnareSetTone2Freq       (t808Snare* const, float freq);
void        t808SnareSetTone1Decay      (t808Snare* const, float decay);
void        t808SnareSetTone2Decay      (t808Snare* const, float decay);
void        t808SnareSetNoiseDecay      (t808Snare* const, float decay);
void        t808SnareSetToneNoiseMix    (t808Snare* const, float toneNoiseMix);
void        t808SnareSetNoiseFilterFreq (t808Snare* const, float noiseFilterFreq);
void        t808SnareSetNoiseFilterQ    (t808Snare* const, float noiseFilterQ);

#endif  // OOPSINSTRUMENT_H_INCLUDED
