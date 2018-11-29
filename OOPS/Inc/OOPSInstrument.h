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
void        tTalkbox_init        (tTalkbox* const);
void        tTalkbox_free        (tTalkbox* const);
float       tTalkbox_tick        (tTalkbox* const, float synth, float voice);
void        tTalkbox_update      (tTalkbox* const);
void        tTalkbox_suspend     (tTalkbox* const);
void        tTalkbox_lpcDurbin   (float *r, int p, float *k, float *g);
void        tTalkbox_lpc         (float *buf, float *car, int32_t n, int32_t o);
void		tTalkbox_setQuality  (tTalkbox* const, float quality);



/* tVocoder */
void        tVocoder_init        (tVocoder* const);
void        tVocoder_free        (tVocoder* const);
float       tVocoder_tick        (tVocoder* const, float synth, float voice);
void        tVocoder_update      (tVocoder* const);
void        tVocoder_suspend     (tVocoder* const);

/* tPluck */
void        tPluck_init          (tPluck*  const, float lowestFrequency); //float delayBuff[DELAY_LENGTH]);
void        tPluck_free          (tPluck*  const);

float       tPluck_tick          (tPluck*  const);

// Pluck the string.
void        tPluck_pluck         (tPluck*  const, float amplitude);

// Start a note with the given frequency and amplitude.;
void        tPluck_noteOn        (tPluck*  const, float frequency, float amplitude );

// Stop a note with the given amplitude (speed of decay).
void        tPluck_noteOff       (tPluck*  const, float amplitude );

// Set instrument parameters for a particular frequency.
void        tPluck_setFrequency  (tPluck*  const, float frequency );

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void        tPluck_controlChange (tPluck*  const, int number, float value);

// tPluck Utilities.
float       tPluck_getLastOut    (tPluck*  const);

/* tStifKarp */
typedef enum SKControlType
{
    SKPickPosition = 0,
    SKStringDamping,
    SKDetune,
    SKControlTypeNil
} SKControlType;

void        tStifKarp_init               (tStifKarp* const, float lowestFrequency); // float delayBuff[2][DELAY_LENGTH]);
void        tStifKarp_free               (tStifKarp* const);
                                    
float       tStifKarp_tick               (tStifKarp*  const);

// Pluck the string.
void        tStifKarp_pluck              (tStifKarp*  const, float amplitude);

// Start a note with the given frequency and amplitude.;
void        tStifKarp_noteOn             (tStifKarp*  const, float frequency, float amplitude );

// Stop a note with the given amplitude (speed of decay).
void        tStifKarp_noteOff            (tStifKarp*  const, float amplitude );

// Set instrument parameters for a particular frequency.
void        tStifKarp_setFrequency       (tStifKarp*  const, float frequency );

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
// Use SKPickPosition, SKStringDamping, or SKDetune for type.
void        tStifKarp_controlChange      (tStifKarp*  const, SKControlType type, float value);

// Set the stretch "factor" of the string (0.0 - 1.0).
void        tStifKarp_setStretch         (tStifKarp*  const, float stretch );

// Set the pluck or "excitation" position along the string (0.0 - 1.0).
void        tStifKarp_setPickupPosition  (tStifKarp*  const, float position );

// Set the base loop gain.
void        tStifKarp_setBaseLoopGain    (tStifKarp*  const, float aGain );

// tStifKarp utilities.
float       tStifKarp_getLastOut         (tStifKarp*  const);

// 808 Cowbell
void    t808Cowbell_init             (t808Cowbell* const);
void    t808Cowbell_free             (t808Cowbell* const);
float   t808Cowbell_tick             (t808Cowbell* const);
void    t808Cowbell_on               (t808Cowbell* const, float vel);
void    t808Cowbell_setDecay         (t808Cowbell* const, float decay);
void    t808Cowbell_setHighpassFreq  (t808Cowbell* const, float freq);
void    t808Cowbell_setBandpassFreq  (t808Cowbell* const, float freq);
void    t808Cowbell_setFreq          (t808Cowbell* const, float freq);
void    t808Cowbell_setOscMix        (t808Cowbell* const, float oscMix);


// 808 Hihat
void        t808Hihat_init               (t808Hihat* const);
void        t808Hihat_free               (t808Hihat* const);

float       t808Hihat_tick               (t808Hihat* const);
void        t808Hihat_on                 (t808Hihat* const, float vel);
void        t808Hihat_setOscNoiseMix     (t808Hihat* const, float oscNoiseMix);
void        t808Hihat_setDecay           (t808Hihat* const, float decay);
void        t808Hihat_setHighpassFreq    (t808Hihat* const, float freq);
void        t808Hihat_etOscBandpassFreq  (t808Hihat* const, float freq);
void        t808Hihat_setOscFreq         (t808Hihat* const, float freq);


// 808 Snare
void        t808Snare_init                  (t808Snare* const);
void        t808Snare_free                  (t808Snare* const);

float       t808Snare_tick                  (t808Snare* const);
void        t808Snare_on                    (t808Snare* const, float vel);
void        t808Snare_setTone1Freq          (t808Snare* const, float freq);
void        t808Snare_setTone2Freq          (t808Snare* const, float freq);
void        t808Snare_setTone1Decay         (t808Snare* const, float decay);
void        t808Snare_setTone2Decay         (t808Snare* const, float decay);
void        t808Snare_setNoiseDecay         (t808Snare* const, float decay);
void        t808Snare_setToneNoiseMix       (t808Snare* const, float toneNoiseMix);
void        t808Snare_setNoiseFilterFreq    (t808Snare* const, float noiseFilterFreq);
void        t808Snare_setNoiseFilterQ       (t808Snare* const, float noiseFilterQ);

#endif  // OOPSINSTRUMENT_H_INCLUDED
