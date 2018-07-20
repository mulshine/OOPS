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
        ps[i] = tPitchShifter_init(samplesPerBlock);
    }
    
    poly = tMPoly_init(NUM_SHIFTERS);
    tMPoly_setPitchGlideTime(poly, 500.0f);
    
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
        inBuffer[cur_read_block*numSamples+cc] = inL[cc];
    }
    
    for (int i = 0; i < NUM_SHIFTERS; ++i)
    {
        for (int cc = 0; cc < numSamples; ++cc)
        {
            tMPoly_tick(poly);
        }
        if (tMPoly_isOn(poly, i))
        {
            float freq = OOPS_midiToFrequency(tMPoly_getPitch(poly, i)) * oops.invSampleRate;
            tPitchShifter_ioSamples_toFreq(ps[i], &inBuffer[cur_read_block*numSamples], &outBuffer[i][cur_write_block*numSamples], numSamples, freq);
        }
        else
        {
            tPitchShifter_setPitchFactor(ps[i], 1.0f);
            tPitchShifter_ioSamples(ps[i], &inBuffer[cur_read_block*numSamples], &outBuffer[i][cur_write_block*numSamples], numSamples);
        }
    }
    
    /* MONO */
    for (int i = 0; i < NUM_SHIFTERS; ++i)
    {
        for (int cc = 0; cc < numSamples; ++cc)
        {
            if (i == 0) outL[cc] = outBuffer[i][cur_write_block*numSamples+cc]*wfGain;
            else outL[cc] += outBuffer[i][cur_write_block*numSamples+cc]*wfGain;
            outR[cc] = outL[cc];
        }
    }
    
    /* STEREO
    for (int cc = 0; cc < numSamples; ++cc)
    {
        outL[cc] = outBuffer[0][cur_write_block*numSamples+cc]*wfGain;
        outR[cc] = outBuffer[1][cur_write_block*numSamples+cc]*wfGain;
    }
    */
    cur_read_block++;
    if (cur_read_block >= TOTAL_BUFFERS)
        cur_read_block = 0;
    
    cur_write_block++;
    if (cur_write_block >= TOTAL_BUFFERS)
        cur_write_block = 0;
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
}


void    OOPSTest_noteOff         (int midiNoteNumber)
{
    tMPoly_noteOff(poly, midiNoteNumber);
}



void    OOPSTest_end             (void)
{

}
