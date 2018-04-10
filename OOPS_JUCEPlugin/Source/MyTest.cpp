/*
  ==============================================================================

    FM.c
    Created: 23 Jan 2017 9:39:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPSTest.h"
#include "MyTest.h"

#define USE_RAMP 0

float gain;

void    OOPSTest_init            (float sampleRate)
{
    OOPSInit(sampleRate, &randomNumberGenerator);

    neuron = tNeuronInit();
    
    env = tRampInit(10, 1);
    //ramp[3] = tRampInit(10, 1);
    //ramp[6] = tRampInit(10, 1);
    
    gain = 1.0f;
    

    for (int i = 0; i < NUM_RAMP; i++)
    {
        ramp[i] = tRampInit(10, 1);
    }

    compressor = tCompressorInit();
    compressor->M = 1.0f;
    compressor->T = 0.0f;
    compressor->tauAttack = 25.0f;
    compressor->tauRelease = 250.0f;
    compressor->R = 12.0f;
    compressor->W = 6.0f;

}

int count = 0;

float   OOPSTest_tick            (float input)
{
    float sample = 0.0f;
    
    neuron->current = tRampTick(env);
    
#if USE_RAMP
    neuron->gK = tRampTick(ramp[0]);
    neuron->gL = tRampTick(ramp[1]);
    neuron->gN = tRampTick(ramp[2]);
    tNeuronSetC(neuron, tRampTick(ramp[3]));
    neuron->V[0] = tRampTick(ramp[4]);
    neuron->V[2] = tRampTick(ramp[5]);
    neuron->timeStep = tRampTick(ramp[6]);
    
    
    if (++count == 50)
    {
        count = 0;
        
        for (int i = 0; i < NUM_RAMP; i++)
        {
            DBG("r"+String(i+1)+" = "+String(tRampSample(ramp[i])));
        }
    }
#endif

    sample = tCompressorTick(compressor, tNeuronTick(neuron));
    //sample = tNeuronTick(neuron);
    
    return sample;
}
float k,c,l,n,v1,v2,v3,ts,g;

void    OOPSTest_block           (void)
{

}

typedef enum NeuronParam
{
    NeuronK = 0,
    NeuronC,
    NeuronL,
    NeuronN,
    NeuronV1,
    NeuronV2,
    NeuronV3,
    NeuronTimeStep,
    NeuronCurrent,
    NeuronParamNil
    
} NeuronParam;


void    OOPSTest_controllerInput (int cnum, float cval)
{
    //DBG("cnum: " + String(cnum) + " val: " + String(cval));
    float in = cval;
    cval *= 128.0f;

    if (cnum == 1)
    {
        //tNeuronSetK(neuron, 70.0f + in * 50.0f);
        //tRampSetDest(ramp[0], cval*2.0f);
    }
    else if (cnum == 2)
    {
        tNeuronSetL(neuron, -in);
        //tRampSetDest(ramp[1], cval*2.0f);
    }
    else if (cnum == 3)
    {
        tNeuronSetN(neuron, 128.0f + cval * 3.0f);
        //tRampSetDest(ramp[2], cval*2.0f - 128.0f);
    }
    else if (cnum == 4)
    {
        //tRampSetDest(ramp[3], (cval/128.0f) * 2.0f + 0.01);
        tNeuronSetC(neuron, in * 2.0f + 0.01);
    }
    else if (cnum == 5)
    {
        tNeuronSetV1(neuron, cval*2.0f - 128.0f);
        //tRampSetDest(ramp[4], cval*2.0f - 128.0f);
    }
    else if (cnum == 6)
    {
        tNeuronSetV3(neuron, cval*2.0f - 128.0f);
        //tRampSetDest(ramp[5], cval*2.0f - 128.0f);
    }
    else if (cnum == 7)
    {
        tNeuronSetTimeStep(neuron, 1.0f / (cval * 2.0f + 1.0f));
        //tCycleSetFreq(osc, 10.0f + cval * 5.0f);
        //tRampSetDest(ramp[6], 1.0f / (cval * 2.0f + 1.0f));
    }
    else if (cnum == 8)
    {
        gain = 1.0f;
    }

    
    
}

void    OOPSTest_pitchBendInput  (int pitchBend)
{
    DBG("pitch bend: " + String(pitchBend));
}

int lastNote;

void    OOPSTest_noteOn          (int note, float velocity)
{
    tRampSetDest(env, 50.0f + note * 1.5f);
    DBG("note on: " + String(note));
    lastNote = note;
}


void    OOPSTest_noteOff         (int note)
{
    if (lastNote == note) tRampSetDest(env, 0.0f);
}



void    OOPSTest_end             (void)
{
    // it never ends
}
