/*
  ==============================================================================

    OOPSDelay.c
    Created: 20 Jan 2017 12:01:24pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPSDelay.h"
#include "OOPS.h"

#if N_DELAY
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ Delay ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
tDelay*    tDelayInit (uint32_t delay)
{
    tDelay* d = &oops.tDelayRegistry[oops.registryIndex[T_DELAY]++];
    
    d->maxDelay = DELAY_LENGTH;
    
    if (delay < 0.0f)               d->delay = 0.0f;
    else if (delay > d->maxDelay)   d->delay = d->maxDelay;
    else                            d->delay = delay;
    
    d->inPoint = 0;
    d->outPoint = 0;
    
    d->lastIn = 0.0f;
    d->lastOut = 0.0f;
    
    d->gain = 1.0f;
    
    tDelaySetDelay(d, d->delay);
    
    return d;
    
}

float   tDelayTick (tDelay* const d, float input)
{
    // Input
    d->lastIn = input;
    d->buff[d->inPoint] = input * d->gain;
    if (++(d->inPoint) == d->maxDelay)     d->inPoint = 0;
    
    // Output
    d->lastOut = d->buff[d->outPoint];
    if (++(d->outPoint) == d->maxDelay)    d->outPoint = 0;
    
    return d->lastOut;
}


int     tDelaySetDelay (tDelay* const d, uint32_t delay)
{
    if (delay >= d->maxDelay)    d->delay = d->maxDelay;
    else                         d->delay = delay;
    
    // read chases write
    if ( d->inPoint >= delay )  d->outPoint = d->inPoint - d->delay;
    else                        d->outPoint = d->maxDelay + d->inPoint - d->delay;
    
    return 0;
}

float tDelayTapOut (tDelay* const d, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return d->buff[tap];
    
}

void tDelayTapIn (tDelay* const d, float value, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    d->buff[tap] = value;
}

float tDelayAddTo (tDelay* const d, float value, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return (d->buff[tap] += value);
}

uint32_t   tDelayGetDelay (tDelay* const d)
{
    return d->delay;
}

float   tDelayGetLastOut (tDelay* const d)
{
    return d->lastOut;
}

float   tDelayGetLastIn (tDelay* const d)
{
    return d->lastIn;
}

void tDelaySetGain (tDelay* const d, float gain)
{
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

float tDelayGetGain (tDelay* const d)
{
    return d->gain;
}
#endif //N_DELAY

#if N_DELAYL
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DelayL ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
tDelayL*    tDelayLInit (float delay)
{
    tDelayL* d = &oops.tDelayLRegistry[oops.registryIndex[T_DELAYL]++];
    
    d->maxDelay = DELAY_LENGTH;
    
    if (delay < 0.0f)               d->delay = 0.0f;
    else if (delay > d->maxDelay)   d->delay = d->maxDelay;
    else                            d->delay = delay;
    
    d->gain = 1.0f;
    
    d->lastIn = 0.0f;
    d->lastOut = 0.0f;

    d->inPoint = 0;
    d->outPoint = 0;
    
    tDelayLSetDelay(d, d->delay);
    
    return d;
}

float   tDelayLTick (tDelayL* const d, float input)
{
    d->buff[d->inPoint] = input * d->gain;
    
    // Increment input pointer modulo length.
    if (++(d->inPoint) == d->maxDelay )    d->inPoint = 0;
    
    
    // First 1/2 of interpolation
    d->lastOut = d->buff[d->outPoint] * d->omAlpha;
    
    // Second 1/2 of interpolation
    if (d->outPoint + 1 < d->maxDelay)
        d->lastOut += d->buff[d->outPoint+1] * d->alpha;
    else
        d->lastOut += d->buff[0] * d->alpha;
    
    // Increment output pointer modulo length.
    if ( ++(d->outPoint) == d->maxDelay )   d->outPoint = 0;
    
    return d->lastOut;
}

int     tDelayLSetDelay (tDelayL* const d, float delay)
{
    if (delay < 0.0f)               d->delay = 0.0f;
    else if (delay <= d->maxDelay)  d->delay = delay;
    else                            d->delay = d->maxDelay;
    
    float outPointer = d->inPoint - d->delay;
    
    while ( outPointer < 0 )
        outPointer += d->maxDelay; // modulo maximum length
    
    d->outPoint = (uint32_t) outPointer;   // integer part
    
    d->alpha = outPointer - d->outPoint; // fractional part
    d->omAlpha = 1.0f - d->alpha;
    
    if ( d->outPoint == d->maxDelay ) d->outPoint = 0;
    
    return 0;
}

float tDelayLTapOut (tDelayL* const d, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return d->buff[tap];
    
}

void tDelayLTapIn (tDelayL* const d, float value, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    d->buff[tap] = value;
}

float tDelayLAddTo (tDelayL* const d, float value, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return (d->buff[tap] += value);
}

float   tDelayLGetDelay (tDelayL *d)
{
    return d->delay;
}

float   tDelayLGetLastOut (tDelayL* const d)
{
    return d->lastOut;
}

float   tDelayLGetLastIn (tDelayL* const d)
{
    return d->lastIn;
}

void tDelayLSetGain (tDelayL* const d, float gain)
{
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

float tDelayLGetGain (tDelayL* const d)
{
    return d->gain;
}
#endif //N_DELAYL

#if N_DELAYA
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DelayA ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
tDelayA*    tDelayAInit (float delay)
{
    tDelayA* d = &oops.tDelayARegistry[oops.registryIndex[T_DELAYA]++];
    
    d->maxDelay = DELAY_LENGTH;
    
    if (delay < 0.0f)               d->delay = 0.0f;
    else if (delay > d->maxDelay)   d->delay = d->maxDelay;
    else                            d->delay = delay;
    
    d->gain = 1.0f;
    
    d->lastIn = 0.0f;
    d->lastOut = 0.0f;
    
    d->inPoint = 0;
    d->outPoint = 0;
    
    tDelayASetDelay(d, d->delay);
    
    d->apInput = 0.0f;
    
    return d;
    
}

float   tDelayATick (tDelayA* const d, float input)
{
    d->buff[d->inPoint] = input * d->gain;
    
    // Increment input pointer modulo length.
    if ( ++(d->inPoint) >= d->maxDelay )    d->inPoint = 0;
    
    // Do allpass interpolation delay.
    float out = d->lastOut * -d->coeff;
    out += d->apInput + ( d->coeff * d->buff[d->outPoint] );
    d->lastOut = out;
    
    // Save allpass input
    d->apInput = d->buff[d->outPoint];
    
    // Increment output pointer modulo length.
    if (++(d->outPoint) >= d->maxDelay )   d->outPoint = 0;
    
    return d->lastOut;
}

int     tDelayASetDelay (tDelayA* const d, float delay)
{
    if (delay < 0.5f)               d->delay = 0.5f;
    else if (delay <= d->maxDelay)  d->delay = delay;
    else                            d->delay = d->maxDelay;
    
    // outPoint chases inPoint
    float outPointer = (float)d->inPoint - d->delay + 1.0f;
    
    while ( outPointer < 0 )    outPointer += d->maxDelay;  // mod max length
    
    d->outPoint = (uint32_t) outPointer;         // integer part
    
    if ( d->outPoint >= d->maxDelay )   d->outPoint = 0;
    
    d->alpha = 1.0f + (float)d->outPoint - outPointer; // fractional part
    
    if ( d->alpha < 0.5f )
    {
        // The optimal range for alpha is about 0.5 - 1.5 in order to
        // achieve the flattest phase delay response.
        
        d->outPoint += 1;
        
        if ( d->outPoint >= d->maxDelay ) d->outPoint -= d->maxDelay;
        
        d->alpha += 1.0f;
    }
    
    d->coeff = (1.0f - d->alpha) / (1.0f + d->alpha);  // coefficient for allpass
    
    return 0;
}

float tDelayATapOut (tDelayA* const d, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return d->buff[tap];
    
}

void tDelayATapIn (tDelayA* const d, float value, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    d->buff[tap] = value;
}

float tDelayAAddTo (tDelayA* const d, float value, uint32_t tapDelay)
{
    int32_t tap = d->inPoint - tapDelay - 1;
    
    // Check for wraparound.
    while ( tap < 0 )   tap += d->maxDelay;
    
    return (d->buff[tap] += value);
}

float   tDelayAGetDelay (tDelayA* const d)
{
    return d->delay;
}

float   tDelayAGetLastOut (tDelayA* const d)
{
    return d->lastOut;
}

float   tDelayAGetLastIn (tDelayA* const d)
{
    return d->lastIn;
}

void tDelayASetGain (tDelayA* const d, float gain)
{
    if (gain < 0.0f)    d->gain = 0.0f;
    else                d->gain = gain;
}

float tDelayAGetGain (tDelayA* const d)
{
    return d->gain;
}

#endif // N_DELAYA
