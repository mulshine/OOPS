/*
  ==============================================================================

    FM.c
    Created: 23 Jan 2017 9:39:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPSTest.h"
#include "MyTest.h"

void    OOPSTest_init            (float sampleRate)
{
    OOPSInit(sampleRate, &randomNumberGenerator);
    
    osc = tSawtoothInit();
    
    tSawtoothSetFreq(osc, 220.0f);
}

int count = 0;

float   OOPSTest_tick            (float input)
{
    float sample = 0.0f;
    
    sample = tSawtoothTick(osc);
    
    return sample;
}


void    OOPSTest_block           (void)
{
    float val = getSliderValue("Freq");
    
    float freq = val * 1000.0f + 1.0f;
    
    tSawtoothSetFreq(osc, freq);
    
    // UI 
    setSliderModelValue("Freq", freq);
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
