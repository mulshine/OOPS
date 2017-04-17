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

    rampOut = tRampInit(5, 1);
    tRampSetDest(rampOut, 1.0f);
    
    snare = t808SnareInit();
    hihat = t808HihatInit();
    cowbell = t808CowbellInit();
    
}

float   OOPSTest_tick            (float input)
{
    float sample = 0.0f;
    
    sample = t808SnareTick(snare) + t808HihatTick(hihat) + t808CowbellTick(cowbell);
    
    sample *= 0.5f;
    
    return sample;
}

void    OOPSTest_block           (void)
{
    
    bool buttonState = getButtonState("Snare");
    
    if (buttonState)
    {
        setButtonState("Snare", false);
        t808SnareOn(snare, 0.95f);
    }
    
    buttonState = getButtonState("Hihat");
    
    if (buttonState)
    {
        setButtonState("Hihat", false);
        t808HihatOn(hihat, 0.95f);
    }
    
    
    buttonState = getButtonState("Cowbell");
    
    if (buttonState)
    {
        setButtonState("Cowbell", false);
        t808CowbellOn(cowbell, 0.95f);
    }
    
    
    


}

void    OOPSTest_controllerInput (int controller, float value)
{

}


void    OOPSTest_pitchBendInput  (int pitchBend)
{
    DBG("pitchBend: " + String(pitchBend));
}

void    OOPSTest_noteOn          (int midiNoteNumber, float velocity)
{
    DBG("noteOn: " + String(midiNoteNumber) + " " +String(velocity));

}



void    OOPSTest_noteOff         (int midiNoteNumber)
{
    DBG("noteOff: " + String(midiNoteNumber));

}



void    OOPSTest_end             (void)
{

}
