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
    vocoder = tVocoderInit();
    fakeVoice = tSawtoothInit();
    
    tSawtoothSetFreq(fakeVoice, 220.0f);

}

int count = 0;

float   OOPSTest_tick            (float input)
{
    float sample = tVocoderTick(vocoder, tSawtoothTick(osc), input);
    
    return sample;
}


void    OOPSTest_block           (void)
{
    
}

void    OOPSTest_controllerInput (int controller, float value)
{

}


void    OOPSTest_pitchBendInput  (int pitchBend)
{

}

void    OOPSTest_noteOn          (int midiNoteNumber, float velocity)
{
    DBG("note on: " + String(midiNoteNumber));
    tSawtoothSetFreq(osc, OOPS_midiToFrequency(midiNoteNumber));
}



void    OOPSTest_noteOff         (int midiNoteNumber)
{

}



void    OOPSTest_end             (void)
{

}
