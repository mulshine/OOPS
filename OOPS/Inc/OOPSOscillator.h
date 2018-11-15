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
tNeuron*    tNeuronInit(void);
void        tNeuronReset(tNeuron* const);
float       tNeuronTick(tNeuron* const);
void        tNeuronSetMode  (tNeuron* const, NeuronMode mode);
void        tNeuronSetCurrent  (tNeuron* const, float current);
void        tNeuronSetK(tNeuron* const, float K);
void        tNeuronSetL(tNeuron* const, float L);
void        tNeuronSetN(tNeuron* const, float N);
void        tNeuronSetC(tNeuron* const, float C);
void        tNeuronSetV1(tNeuron* const, float V1);
void        tNeuronSetV2(tNeuron* const, float V2);
void        tNeuronSetV3(tNeuron* const, float V3);
void        tNeuronSetTimeStep(tNeuron* const, float timestep);

/* tCycle: Cycle/Sine waveform */
void        tCycleInit         (tCycle* const);
float       tCycleTick         (tCycle*  const);

// Set frequency in Hz.
int         tCycleSetFreq      (tCycle*  const, float freq);


/* tPhasor: Aliasing phasor [0.0, 1.0) */
tPhasor*    tPhasorInit        (void);
float       tPhasorTick        (tPhasor*  const);

// Set frequency in Hz.
int         tPhasorSetFreq     (tPhasor*  const, float freq);


/* tSawtooth: Anti-aliased Sawtooth waveform using wavetable interpolation. Wavetables constructed from sine components. */
tSawtooth*  tSawtoothInit      (void);
float       tSawtoothTick      (tSawtooth*  const);

// Set frequency in Hz.
int         tSawtoothSetFreq   (tSawtooth*  const, float freq);


/* tTriangle: Anti-aliased Triangle waveform using wavetable interpolation. Wavetables constructed from sine components. */
tTriangle*  tTriangleInit      (void);
float       tTriangleTick      (tTriangle*  const);

// Set frequency in Hz.
int         tTriangleSetFreq   (tTriangle*  const, float freq);


/* tSquare: Anti-aliased Square waveform using wavetable interpolation. Wavetables constructed from sine components. */
tSquare*    tSquareInit        (void);
float       tSquareTick        (tSquare*  const);

// Set frequency in Hz.
int         tSquareSetFreq     (tSquare*  const, float freq);


/* tNoise. WhiteNoise, PinkNoise. */
tNoise*     tNoiseInit         (NoiseType type);
float       tNoiseTick         (tNoise*  const);

#endif  // OOPSOSCILLATOR_H_INCLUDED
