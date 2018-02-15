/*
  ==============================================================================

    FM.c
    Created: 23 Jan 2017 9:39:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPSTest.h"
#include "MyTest.h"

float gain;

void    OOPSTest_init            (float sampleRate)
{
    OOPSInit(sampleRate, &randomNumberGenerator);

    neuron = tNeuronInit();
    
    env = tRampInit(10, 1);
    
    gain = 1.0f;
    
    setSliderModelValue("Gain", gain);
    setSliderModelValue("K", neuron->gK);
    setSliderModelValue("C", neuron->C);
    setSliderModelValue("L", neuron->gL);
    setSliderModelValue("N", neuron->gN);
    setSliderModelValue("V1", neuron->V[0]);
    setSliderModelValue("V2", neuron->V[1]);
    setSliderModelValue("V3", neuron->V[2]);
    setSliderModelValue("TimeStep", neuron->timeStep);

}

int count = 0;

float   OOPSTest_tick            (float input)
{
    neuron->current = tRampTick(env);
    
    return gain * tNeuronTick(neuron);
}
float k,c,l,n,v1,v2,v3,ts,g;

void    OOPSTest_block           (void)
{
    float val;
    
    val = getSliderValue("K");
    if (val != k)
    {
        k = val;
        neuron->gK = 128.0f * (k*2.0f - 1.0f);
    }
    
    val = getSliderValue("C");
    if (val != c)
    {
        c = val;
        neuron->C = c * 2.0f + 0.01;
    }
    
    val = getSliderValue("L");
    if (val != l)
    {
        l = val;
        neuron->gL = 128.0f * (l*2.0f);
    }
    
    val = getSliderValue("N");
    if (val != n)
    {
        n = val;
        neuron->gN = 128.0f * (n*2.0f - 1.0f);
    }
    
    val = getSliderValue("V1");
    if (val != v1)
    {
        v1 = val;
        neuron->V[0] = 128.0f * (v1*2.0f - 1.0f);
    }
    
    val = getSliderValue("V2");
    if (val != v2)
    {
        k = v2;
        neuron->V[1] = 128.0f * (v2*2.0f);
    }
    
    val = getSliderValue("V3");
    if (val != v3)
    {
        v3 = val;
        neuron->V[2] = 128.0f * (v3*2.0f - 1.0f);
    }
    
    val = getSliderValue("TimeStep");
    if (val != ts)
    {
        ts = val;
        neuron->timeStep = 1.0f / (128.0f* ts * 2.0f + 1.0f);
    }
    val = getSliderValue("Gain");
                            
    if (val != g)
    {
        g= val;
        gain = g;
    }
    
    setSliderModelValue("Gain", gain);
    setSliderModelValue("K", neuron->gK);
    setSliderModelValue("C", neuron->C);
    setSliderModelValue("L", neuron->gL);
    setSliderModelValue("N", neuron->gN);
    setSliderModelValue("V1", neuron->V[0]);
    setSliderModelValue("V2", neuron->V[1]);
    setSliderModelValue("V3", neuron->V[2]);
    setSliderModelValue("TimeStep", neuron->timeStep);
  
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
    cval *= 128.0f;
    
    if (cnum == 1)
    {
        neuron->gK = cval*2.0f - 128.0f;
    }
    else if (cnum == 2)
    {
        neuron->gL = cval*2.0f;
    }
    else if (cnum == 3)
    {
        neuron->gN = cval*2.0f - 128.0f;
    }
    else if (cnum == 4)
    {
        neuron->C = (cval/128.0f) * 2.0f + 0.01;
    }
    else if (cnum == 5)
    {
        neuron->V[0] = cval*2.0f - 128.0f;
    }
    else if (cnum == 6)
    {
        neuron->V[2] = cval*2.0f - 128;
    }
    else if (cnum == 7)
    {
        neuron->timeStep = 1.0f / (cval * 2.0f + 1.0f);
    }
    else if (cnum == 8)
    {
        gain = cval / 128.0f;
    }
}

void    OOPSTest_pitchBendInput  (int pitchBend)
{

}
int lastNote;
void    OOPSTest_noteOn          (int note, float velocity)
{
    tRampSetDest(env, 50.0f + note * 1.5f);
    lastNote = note;
}


void    OOPSTest_noteOff         (int note)
{
    if (lastNote == note) tRampSetDest(env, 0.0f);
}



void    OOPSTest_end             (void)
{

}
