/*
  ==============================================================================

    FM.c
    Created: 23 Jan 2017 9:39:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPSTest.h"
#include "MyTest.h"


static void oops_pool_report(void);
static void oops_pool_dump(void);
static void run_pool_test(void);

t808Snare snare;

void    OOPSTest_init            (float sampleRate, int blockSize)
{
    OOPSInit(sampleRate, blockSize, &randomNumberGenerator);
    
    t808Snare_init(&snare);
    
    oops_pool_report();
}

int timer = 0;

float prev = 0.0;

float   OOPSTest_tick            (float input)
{
    float sample = 0.0f;
    
    timer++;
    if (timer == (1 * oops.sampleRate))
    {
        timer = 0;
        t808Snare_on(&snare, 1.0);
    }

    sample = t808Snare_tick(&snare);
    
    return sample;
     
}

void    OOPSTest_block           (void)
{
    float val = getSliderValue("mix");
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

// OOPS POOL UTILITIES

void oops_pool_report(void)
{
    DBG(String(oops_pool_get_used()) + " of  " + String(oops_pool_get_size()));
}

void oops_pool_dump(void)
{
    float* buff = (float*)oops_pool_get_pool();
    int siz = oops_pool_get_size();
    siz /= sizeof(float);
    for (int i = 0; i < siz; i++)
    {
        DBG(String(buff[i]));
    }
}

static void run_pool_test(void)
{
    oops_pool_report();
    
    DBG("ALLOC BUFFER 1");
    int size = 50;
    float* buffer;
    buffer = (float*) oops_alloc(sizeof(float) * size);
    
    for (int i = 0; i < size; i++)
    {
        buffer[i] = (float)i;
        
    }
    
    oops_pool_report();
    
    DBG("ALLOC BUFFER 2");
    size = 25;
    
    buffer = (float*) oops_alloc(sizeof(float) * size);
    
    oops_pool_report();
    
    for (int i = 0; i < size; i++)
    {
        buffer[i] = (float)(i*2);
    }
    
    DBG("FREE BUFFER 2");
    oops_free(buffer);
    
    oops_pool_report();
    
    DBG("ALLOC BUFFER 3");
    size = 15;
    
    buffer = (float*) oops_alloc(sizeof(float) * size);
    
    for (int i = 0; i < size; i++)
    {
        buffer[i] = (float)(i*3);
    }
    
    oops_pool_report();
    
    oops_pool_dump();
}
