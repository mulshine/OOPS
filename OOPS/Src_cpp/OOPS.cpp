/*
  ==============================================================================

    OOPSCore.c
    Created: 20 Jan 2017 12:08:14pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#if _WIN32 || _WIN64

#include "..\Inc\OOPS.h"

#else

#include "../Inc/OOPS.h"

#endif

#include "../Externals/mpool.h"

OOPS oops;

void OOPSInit(float sr, float(*random)(void))
{
    oops_pool_init(MPOOL_POOL_SIZE);
    
    oops.sampleRate = sr;
    
    oops.invSampleRate = 1.0f/sr;
    
    oops.random = random;
}


#define OOPSSampleRateChanged(THIS) oops.THIS.sampleRateChanged(&oops.THIS)

void OOPSSetSampleRate(float sampleRate)
{
    oops.sampleRate = sampleRate;
    oops.invSampleRate = 1.0f/sampleRate;
}

float OOPSGetSampleRate()
{
    return oops.sampleRate;
}

