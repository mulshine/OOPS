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
tCycle* osc1;
tCycle* osc2;
tCycle* osc3;

void oops_pool_report(void)
{
    DBG(String(oops_pool_get_used()) + " of  " + String(oops_pool_get_size()));
}

void    OOPSTest_init            (float sampleRate)
{
    OOPSInit(sampleRate, &randomNumberGenerator);

    oops_pool_report();
    
    osc1 = (tCycle*) oops_alloc(TCYCLE_SIZE);
    osc2 = (tCycle*) oops_alloc(TCYCLE_SIZE);
    osc3 = (tCycle*) oops_alloc(TCYCLE_SIZE);
    
    oops_pool_report();

    oops_free(osc2);
    
    oops_pool_report();
}

int count = 0;

float   OOPSTest_tick            (float input)
{
    return 0.0;
}

void    OOPSTest_block           (void)
{
    
  
}


void    OOPSTest_controllerInput (int cnum, float cval)
{

}

void    OOPSTest_pitchBendInput  (int pitchBend)
{
    
}

int lastNote;
void    OOPSTest_noteOn          (int note, float velocity)
{
}


void    OOPSTest_noteOff         (int note)
{
}



void    OOPSTest_end             (void)
{

}
