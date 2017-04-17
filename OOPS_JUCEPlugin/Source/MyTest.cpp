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
    myCompressor = tCompressorInit();
    
    rampOut = tRampInit(10.0f, 1);
    tRampSetDest(rampOut, 1.0f);
}

float   OOPSTest_tick            (float input)
{
    float sample = 0.0f;
    
    sample = tCompressorTick(myCompressor, input);
    
    return sample;
}

void    OOPSTest_block           (void)
{
    
    
    float val = getSliderValue("Threshold");
    
    if (myT != val)
    {
        myT = val;
        
        myCompressor->T = -60.0f + 60.0f * val;
        
        setSliderModelValue("Threshold", myCompressor->T);
    }
    
    val = getSliderValue("Ratio");
    
    if (myR != val)
    {
        myR = val;
    
        myCompressor->R = 1.0f + val * 23.0f;
        
        setSliderModelValue("Ratio", myCompressor->R);
    }
    
    val = getSliderValue("Width");
    
    if (myW != val)
    {
        myW = val;
        
        myCompressor->W = 24.0f * val;
        
        setSliderModelValue("Width", myCompressor->W);
    }
    
    val = getSliderValue("Makeup");
    
    if (myM != val)
    {
        myM = val;
        
        myCompressor->M = -24.0f + 48.0f * val;
        
       setSliderModelValue("Makeup", myCompressor->M);
    }
    
    val = getSliderValue("Attack");
    
    if (myAtt != val)
    {
        myAtt = val;
        
        myCompressor->tauAttack = 1.0f + 5000.0f * val;
        
        setSliderModelValue("Attack", myCompressor->tauAttack);
    }
    
    val = getSliderValue("Release");
    
    if (myRel != val)
    {
        myRel = val;
    
        myCompressor->tauRelease = 1.0f + 5000.0f * val;
    
        setSliderModelValue("Release", myCompressor->tauRelease);
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
