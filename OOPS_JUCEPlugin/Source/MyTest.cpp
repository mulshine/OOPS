/*
  ==============================================================================

    FM.c
    Created: 23 Jan 2017 9:39:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPSTest.h"
#include "MyTest.h"

int cur_read_block = 2, cur_write_block = 0;

/**********************************************/

void    OOPSTest_init            (float sampleRate, int samplesPerBlock)
{
    OOPSInit(sampleRate, samplesPerBlock, &randomNumberGenerator);

    for (int i = 0; i < NUM_SHIFTERS; ++i)
    {
        ps[i] = tPitchShifter_init(inBuffer, outBuffer, 9182, 1024);
    }
    saw = tSawtoothInit();
    tSawtoothSetFreq(saw, 200.0f);
    poly = tMPoly_init(1);
    tMPoly_setPitchGlideTime(poly, 10.0f);
    ramp = tRampInit(10.0f, 1);
    fs = tFormantShifterInit();
    
    setSliderValue("GAIN", 0.0f);
    setSliderValue("PITCH FACTOR 1", 1.0f/7.0f);
    setSliderValue("PITCH FACTOR 2", 1.0f/7.0f);
}

float   OOPSTest_tick            (float input)
{
    
}

void    OOPSTest_block           (float* inL, float* inR, float* outL, float* outR, int numSamples)
{
    float s1 = getSliderValue("GAIN");
    wfGain = s1;
    float s2 = getSliderValue("PITCH FACTOR 1");
    wfPitchFactor[0] = (s2 * 3.5f) + 0.5f;
    float s3 = getSliderValue("PITCH FACTOR 2");
    wfPitchFactor[1] = (s3 * 3.5f) + 0.5f;
    
    for (int cc = 0; cc < numSamples; ++cc)
    {
        outL[cc] = tFormantShifterTick(fs, inL[cc], s1*2.0f-1.0f);
        outR[cc] = outL[cc];
    }
    

    /* STEREO
    for (int cc = 0; cc < numSamples; ++cc)
    {
        tMPoly_tick(poly);
        float freq = OOPS_midiToFrequency(tMPoly_getPitch(poly, 0));
        tRampTick(ramp);
        float out = tPitchShifter_tick(ps[0], inL[cc], freq);
        
        outL[cc] = out * tRampSample(ramp);
            //tPitchShifter_ioSamples_toFreq(ps[i], &inBuffer[cur_read_block*numSamples], &outBuffer[i][cur_write_block*numSamples], numSamples, freq);

        outR[cc] = outL[cc];
    } */
}

void    OOPSTest_block           (int numSamples)
{

}


void    OOPSTest_controllerInput (int controller, float value)
{

}

void    OOPSTest_pitchBendInput  (int pitchBend)
{
    tMPoly_pitchBend(poly, pitchBend);
}

void    OOPSTest_noteOn          (int midiNoteNumber, float velocity)
{
    tMPoly_noteOn(poly, midiNoteNumber, velocity);
    if (tMPoly_isOn(poly, 0) == 1) tRampSetDest(ramp, 1.0f);
}


void    OOPSTest_noteOff         (int midiNoteNumber)
{
    tMPoly_noteOff(poly, midiNoteNumber);
    if (tMPoly_isOn(poly, 0) == 0) tRampSetDest(ramp, 0.0f);
}



void    OOPSTest_end             (void)
{

}
