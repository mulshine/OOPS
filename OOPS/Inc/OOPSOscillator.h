/*
  ==============================================================================

    OOPSOscillator.h
    Created: 20 Jan 2017 12:00:58pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef OOPSOSCILLATOR_H_INCLUDED
#define OOPSOSCILLATOR_H_INCLUDED

#include "OOPSCore.h"

/* tNeuron */
void        tNeuron_init        (tNeuron* const);
void        tNeuron_reset       (tNeuron* const);
float       tNeuron_Tick        (tNeuron* const);
void        tNeuron_setMode     (tNeuron* const, NeuronMode mode);
void        tNeuron_setCurrent  (tNeuron* const, float current);
void        tNeuron_setK        (tNeuron* const, float K);
void        tNeuron_setL        (tNeuron* const, float L);
void        tNeuron_setN        (tNeuron* const, float N);
void        tNeuron_setC        (tNeuron* const, float C);
void        tNeuron_setV1       (tNeuron* const, float V1);
void        tNeuron_setV2       (tNeuron* const, float V2);
void        tNeuron_setV3       (tNeuron* const, float V3);
void        tNeuron_setTimeStep (tNeuron* const, float timestep);

/* tCycle: Cycle/Sine waveform */
void        tCycle_init         (tCycle*  const);
float       tCycle_tick         (tCycle*  const);

// Set frequency in Hz.
int         tCycle_setFreq      (tCycle*  const, float freq);


/* tPhasor: Aliasing phasor [0.0, 1.0) */
void        tPhasor_init        (tPhasor*  const);
float       tPhasor_tick        (tPhasor*  const);

// Set frequency in Hz.
int         tPhasor_setFreq     (tPhasor*  const, float freq);


/* tSawtooth: Anti-aliased Sawtooth waveform using wavetable interpolation. Wavetables constructed from sine components. */
void        tSawtooth_init      (tSawtooth*  const);
float       tSawtooth_tick      (tSawtooth*  const);

// Set frequency in Hz.
int         tSawtooth_setFreq   (tSawtooth*  const, float freq);

/* tTriangle: Anti-aliased Triangle waveform using wavetable interpolation. Wavetables constructed from sine components. */
void        tTriangle_init      (tTriangle*  const);
float       tTriangle_tick      (tTriangle*  const);

// Set frequency in Hz.
int         tTriangle_setFreq   (tTriangle*  const, float freq);


/* tSquare: Anti-aliased Square waveform using wavetable interpolation. Wavetables constructed from sine components. */
void        tSquare_init        (tSquare*  const);
float       tSquare_tick        (tSquare*  const);

// Set frequency in Hz.
int         tSquare_setFreq     (tSquare*  const, float freq);


/* tNoise. WhiteNoise, PinkNoise. */
void        tNoise_init          (tNoise* const, NoiseType type);
float       tNoise_tick          (tNoise*  const);

#endif  // OOPSOSCILLATOR_H_INCLUDED
