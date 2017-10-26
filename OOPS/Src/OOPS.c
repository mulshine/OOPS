/*
  ==============================================================================

    OOPSCore.c
    Created: 20 Jan 2017 12:08:14pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPS.h"

OOPS oops;

void OOPSInit(float sr, float(*random)(void))
{
    oops.sampleRate = sr;
    
    oops.invSampleRate = 1.0f/sr;
    
    oops.random = random;
    
    for (int i = 0; i < T_INDEXCNT; i++)
        oops.registryIndex[i] = 0;
}


#define OOPSSampleRateChanged(THIS) oops.THIS.sampleRateChanged(&oops.THIS)

void OOPSSetSampleRate(float sampleRate)
{
    oops.sampleRate = sampleRate;
    oops.invSampleRate = 1.0f/sampleRate;
    
    
#if N_PHASOR 
		for (int i = 0; i < oops.registryIndex[T_PHASOR]; i++)         OOPSSampleRateChanged(tPhasorRegistry[i]);
#endif
    
#if N_CYCLE 
		for (int i = 0; i < oops.registryIndex[T_CYCLE]; i++)          OOPSSampleRateChanged(tCycleRegistry[i]);
#endif
    
#if N_SAWTOOTH 
		for (int i = 0; i < oops.registryIndex[T_SAWTOOTH]; i++)       OOPSSampleRateChanged(tSawtoothRegistry[i]);
#endif
    
#if N_TRIANGLE 
		for (int i = 0; i < oops.registryIndex[T_TRIANGLE]; i++)       OOPSSampleRateChanged(tTriangleRegistry[i]);
#endif
    
#if N_SQUARE 
		for (int i = 0; i < oops.registryIndex[T_SQUARE]; i++)         OOPSSampleRateChanged(tSquareRegistry[i]);
#endif
    
#if N_TWOPOLE 
		for (int i = 0; i < oops.registryIndex[T_TWOPOLE]; i++)        OOPSSampleRateChanged(tTwoPoleRegistry[i]);
#endif

#if N_BUTTERWORTH 
		for (int i = 0; i < oops.registryIndex[T_BUTTERWORTH]; i++)    OOPSSampleRateChanged(tButterworthRegistry[i]);
#endif
    
#if N_TWOZERO 
		for (int i = 0; i < oops.registryIndex[T_TWOZERO]; i++)        OOPSSampleRateChanged(tTwoZeroRegistry[i]);
#endif
    
#if N_BIQUAD 
		for (int i = 0; i < oops.registryIndex[T_BIQUAD]; i++)         OOPSSampleRateChanged(tBiQuadRegistry[i]);
#endif
    
#if N_HIGHPASS 
		for (int i = 0; i < oops.registryIndex[T_HIGHPASS]; i++)       OOPSSampleRateChanged(tHighpassRegistry[i]);
#endif
    
#if N_ADSR 
		for (int i = 0; i < oops.registryIndex[T_ADSR]; i++)           OOPSSampleRateChanged(tADSRRegistry[i]);
#endif
    
#if N_RAMP 
		for (int i = 0; i < oops.registryIndex[T_RAMP]; i++)           OOPSSampleRateChanged(tRampRegistry[i]);
#endif

#if N_PRCREV 
		for (int i = 0; i < oops.registryIndex[T_PRCREV]; i++)         OOPSSampleRateChanged(tPRCRevRegistry[i]);
#endif
    
#if N_NREV 
		for (int i = 0; i < oops.registryIndex[T_NREV]; i++)           OOPSSampleRateChanged(tNRevRegistry[i]);
#endif
    
#if N_PLUCK 
		for (int i = 0; i < oops.registryIndex[T_PLUCK]; i++)          OOPSSampleRateChanged(tPluckRegistry[i]);
#endif
   
#if N_STIFKARP 
		 for (int i = 0; i < oops.registryIndex[T_STIFKARP]; i++)       OOPSSampleRateChanged(tStifKarpRegistry[i]);
#endif

#if N_NEURON 
		 for (int i = 0; i < oops.registryIndex[T_NEURON]; i++)         OOPSSampleRateChanged(tNeuronRegistry[i]);
#endif
    
#if N_COMPRESSOR
        for (int i = 0; i < oops.registryIndex[T_COMPRESSOR]; i++)         OOPSSampleRateChanged(tCompressorRegistry[i]);
#endif
}

float OOPSGetSampleRate()
{
    return oops.sampleRate;
}

