/*
  ==============================================================================

    MyTest.h
    Created: 23 Jan 2017 9:39:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef FM_H_INCLUDED
#define FM_H_INCLUDED

tCompressor* compressor;

#define NUM_RAMP 7

float current_base;
tNeuron* neuron;
tRamp* current_ramp;
tRamp* tsr;
tRamp* ramp[NUM_RAMP];

tCycle* osc;




#endif  // FM_H_INCLUDED
