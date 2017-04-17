/*
  ==============================================================================

    OOPSReverb.c
    Created: 20 Jan 2017 12:02:04pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPSReverb.h"
#include "OOPS.h"

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ PRCRev ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
#if N_PRCREV
tPRCRev*    tPRCRevInit(float t60)
{
    tPRCRev* r = &oops.tPRCRevRegistry[oops.registryIndex[T_PRCREV]++];
    
    if (t60 <= 0.0) t60 = 0.001f;
    
    r->inv_441 = 1.0f/44100.0f;
    
    int lengths[4] = { 341, 613, 1557, 2137 }; // Delay lengths for 44100 Hz sample rate.
    double scaler = oops.sampleRate * r->inv_441;
    
    int delay, i;
    if (scaler != 1.0)
    {
        for (i=0; i<4; i++)
        {
            delay = (int) scaler * lengths[i];
            
            if ( (delay & 1) == 0)          delay++;
            
            while ( !OOPS_isPrime(delay) )  delay += 2;
            
            lengths[i] = delay;
        }
    }
    
    r->allpassDelays[0] = tDelayInit(lengths[0]);
    r->allpassDelays[1] = tDelayInit(lengths[1]);
    r->combDelay = tDelayInit(lengths[2]);
    
    tPRCRevSetT60(r, t60);
    
    r->allpassCoeff = 0.7f;
    r->mix = 0.5f;
    
    r->sampleRateChanged = &tPRCRevSampleRateChanged;
    
    return r;
    
}

void    tPRCRevSetT60(tPRCRev* const r, float t60)
{
    if ( t60 <= 0.0 ) t60 = 0.001f;
    
    r->t60 = t60;
    
    r->combCoeff = pow(10.0, (-3.0 * tDelayGetDelay(r->combDelay) * oops.invSampleRate / t60 ));
    
}

void    tPRCRevSetMix(tPRCRev* const r, float mix)
{
    r->mix = mix;
}

float   tPRCRevTick(tPRCRev* const r, float input)
{
    float temp, temp0, temp1, temp2;
    float out;
    
    r->lastIn = input;
    
    temp = tDelayGetLastOut(r->allpassDelays[0]);
    temp0 = r->allpassCoeff * temp;
    temp0 += input;
    tDelayTick(r->allpassDelays[0], temp0);
    temp0 = -( r->allpassCoeff * temp0) + temp;
    
    temp = tDelayGetLastOut(r->allpassDelays[1]);
    temp1 = r->allpassCoeff * temp;
    temp1 += temp0;
    tDelayTick(r->allpassDelays[1], temp1);
    temp1 = -(r->allpassCoeff * temp1) + temp;
    
    temp2 = temp1 + ( r->combCoeff * tDelayGetLastOut(r->combDelay));
    
    out = r->mix * tDelayTick(r->combDelay, temp2);
    
    temp = (1.0 - r->mix) * input;
    
    out += temp;
    
    r->lastOut = out;
    
    return out;
}

void     tPRCRevSampleRateChanged (tPRCRev* const r)
{
    r->combCoeff = pow(10.0, (-3.0 * tDelayGetDelay(r->combDelay) * oops.invSampleRate / r->t60 ));
}
#endif // N_PRCREV

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ NRev ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */

#if N_NREV
tNRev*    tNRevInit(float t60)
{
    tNRev* r = &oops.tNRevRegistry[oops.registryIndex[T_NREV]++];
    
    if (t60 <= 0.0) t60 = 0.001f;
    
    r->inv_441 = 1.0f/44100.0f;
    
    int lengths[15] = {1433, 1601, 1867, 2053, 2251, 2399, 347, 113, 37, 59, 53, 43, 37, 29, 19}; // Delay lengths for 44100 Hz sample rate.
    double scaler = oops.sampleRate / 25641.0f;
    
    int delay, i;
    
    for (i=0; i < 15; i++)
    {
        delay = (int) scaler * lengths[i];
        if ( (delay & 1) == 0)
            delay++;
        while ( !OOPS_isPrime(delay) )
            delay += 2;
        lengths[i] = delay;
    }
    
    for ( i=0; i<6; i++ )
    {
        r->combDelays[i] = tDelayInit(lengths[i]);
        r->combCoeffs[i] = pow(10.0, (-3 * lengths[i] * oops.invSampleRate / t60));
    }
    
    for ( i=0; i<8; i++ )
        r->allpassDelays[i] = tDelayInit(lengths[i+6]);
    
    for ( i=0; i<2; i++ )
    {
        tDelaySetDelay(r->allpassDelays[i], lengths[i]);
        tDelaySetDelay(r->combDelays[i], lengths[i+2]);
    }
    
    tNRevSetT60(r, t60);
    r->allpassCoeff = 0.7f;
    r->mix = 0.3f;
    
    r->sampleRateChanged = &tNRevSampleRateChanged;
    
    return r;
    
}


void    tNRevSetT60(tNRev* const r, float t60)
{
    if (t60 <= 0.0)           t60 = 0.001f;
    
    r->t60 = t60;
    
    for (int i=0; i<6; i++)   r->combCoeffs[i] = pow(10.0, (-3.0 * tDelayGetDelay(r->combDelays[i]) * oops.invSampleRate / t60 ));
    
}

void    tNRevSetMix(tNRev* const r, float mix)
{
    r->mix = mix;
}

float   tNRevTick(tNRev* const r, float input)
{
    r->lastIn = input;
    
    float temp, temp0, temp1, temp2, out;
    int i;
    
    temp0 = 0.0;
    for ( i=0; i<6; i++ )
    {
        temp = input + (r->combCoeffs[i] * tDelayGetLastOut(r->combDelays[i]));
        temp0 += tDelayTick(r->combDelays[i],temp);
    }
    
    for ( i=0; i<3; i++ )
    {
        temp = tDelayGetLastOut(r->allpassDelays[i]);
        temp1 = r->allpassCoeff * temp;
        temp1 += temp0;
        tDelayTick(r->allpassDelays[i], temp1);
        temp0 = -(r->allpassCoeff * temp1) + temp;
    }
    
    // One-pole lowpass filter.
    r->lowpassState = 0.7f * r->lowpassState + 0.3f * temp0;
    temp = tDelayGetLastOut(r->allpassDelays[3]);
    temp1 = r->allpassCoeff * temp;
    temp1 += r->lowpassState;
    tDelayTick(r->allpassDelays[3], temp1 );
    temp1 = -(r->allpassCoeff * temp1) + temp;
    
    temp = tDelayGetLastOut(r->allpassDelays[4]);
    temp2 = r->allpassCoeff * temp;
    temp2 += temp1;
    tDelayTick(r->allpassDelays[4], temp2 );
    out = r->mix * ( -( r->allpassCoeff * temp2 ) + temp );
    
    /*
     temp = tDelayLGetLastOut(&r->allpassDelays[5]);
     temp3 = r->allpassCoeff * temp;
     temp3 += temp1;
     tDelayLTick(&r->allpassDelays[5], temp3 );
     lastFrame_[1] = effectMix_*( - ( r->allpassCoeff * temp3 ) + temp );
     */
    
    temp = ( 1.0f - r->mix ) * input;
    
    out += temp;
    
    r->lastOut = out;
    
    return out;
}



void     tNRevSampleRateChanged (tNRev* const r)
{
    for (int i=0; i<6; i++)   r->combCoeffs[i] = pow(10.0, (-3.0 * tDelayGetDelay(r->combDelays[i]) * oops.invSampleRate / r->t60 ));
}

#endif // N_NREV
