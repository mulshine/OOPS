/*
  ==============================================================================

    FM.c
    Created: 23 Jan 2017 9:39:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPSTest.h"
#include "MyTest.h"

float carrierGain, modulatorGain, carrierFreq, modulatorFreq;


#define FUND 110.0f

void    OOPSTest_init            (float sampleRate)
{
    OOPSInit(sampleRate, &randomNumberGenerator);

    compressor = tCompressorInit();
    delay = tDelayLInit(1);
    
    compressor->M = 24.0f;
    compressor->R = 3.0f;
    compressor->T = -24.0f;
    compressor->W = 24.0f;
    compressor->tauAttack = 0.0f;
    compressor->tauRelease = 0.0f;
    
    bandpass = tSVFInit(SVFTypeBandpass, 440.0f, 0.9f);
}

float   OOPSTest_tick            (float input)
{
    float sample = input;
    
    sample = tDelayLTick(delay, sample);
    
    // sample = tCompressorTick(compressor, sample);
    
    sample = tSVFTick(bandpass, sample);
    
    sample = OOPS_softClip(sample, 0.8f);

    return sample;
}


void    OOPSTest_block           (void)
{
    float val = getSliderValue("Delay");
    
    if (val != delayVal)
    {
        delayVal = val;
        tDelayLSetDelay(delay, 1.0f + 2000.0f * delayVal);
        
        setSliderModelValue("Delay", 1.0f + 2000.0f * delayVal);
    }
    
    val = getSliderValue("Freq");
    
    if (val != delayVal)
    {
        freqVal = val;
        tSVFSetFreq(bandpass, 1.0f + 4000.0f * freqVal);
        
        setSliderModelValue("Freq", 1.0f + 4000.0f * freqVal);
    }
    
    val = getSliderValue("Q");
    
    if (val != delayVal)
    {
        qVal = val;
        tSVFSetQ(bandpass, 0.0f + 9.9999 * qVal);
        
        setSliderModelValue("Q", 0.0f + 9.9999 * qVal);
    }
}

void    OOPSTest_controllerInput (int controller, float value)
{

}


void    OOPSTest_pitchBendInput  (int pitchBend)
{

}

void    OOPSTest_noteOn          (int midiNoteNumber, float velocity)
{

}



void    OOPSTest_noteOff         (int midiNoteNumber)
{

}



void    OOPSTest_end             (void)
{

}
