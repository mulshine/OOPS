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


void    OOPSTest_init            (float sampleRate)
{
    OOPSInit(sampleRate, &randomNumberGenerator);

    carrier = tCycleInit();
    modulator = tCycleInit();
}

float   OOPSTest_tick            (float input)
{
    float sample = 0.0f;
    
    tCycleSetFreq(carrier, carrierFreq + modulatorGain * tCycleTick(modulator));
    
    sample = carrierGain * tCycleTick(carrier);

    return sample;
}


void    OOPSTest_block           (void)
{
    
    
    carrierGain = getSliderValue("CarrierGain");
    
    carrierFreq = 1000.0f * getSliderValue("CarrierFreq");
    
    modulatorGain = 1000.0f * getSliderValue("ModulatorGain");
    
    modulatorFreq = 1000.0f * getSliderValue("ModulatorFreq");
    tCycleSetFreq(modulator, modulatorFreq);
    
    setSliderModelValue("CarrierGain", carrierGain);
    setSliderModelValue("CarrierFreq", carrierFreq);
    setSliderModelValue("ModulatorGain", modulatorGain);
    setSliderModelValue("ModulatorFreq", modulatorFreq);
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
