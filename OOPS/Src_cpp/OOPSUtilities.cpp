/*
  ==============================================================================

    OOPSUtilities.c
    Created: 20 Jan 2017 12:02:17pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPSUtilities.h"
#include "OOPSWavetables.h"
#include "OOPS.h"

#if N_COMPRESSOR
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ Compressor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

tCompressor*    tCompressorInit(int tauAttack, int tauRelease)
{
    tCompressor* c = &oops.tCompressorRegistry[oops.registryIndex[T_COMPRESSOR]++];
    
    c->tauAttack = tauAttack;
    c->tauRelease = tauRelease;
    
    c->x_G[0] = 0.0f, c->x_G[1] = 0.0f,
    c->y_G[0] = 0.0f, c->y_G[1] = 0.0f,
    c->x_T[0] = 0.0f, c->x_T[1] = 0.0f,
    c->y_T[0] = 0.0f, c->y_T[1] = 0.0f;
    
    c->T = 0.0f; // Threshold
    c->R = 1.0f; // compression Ratio
    c->M = 0.0f; // decibel Make-up gain
    c->W = 0.0f; // decibel Width of knee transition
    
    return c;
}

/*
tCompressor*    tCompressorInit(void)
{
    tCompressor* c = &oops.tCompressorRegistry[oops.registryIndex[T_COMPRESSOR]++];
    
    c->tauAttack = 100;
    c->tauRelease = 100;
    
    c->T = 0.0f; // Threshold
    c->R = 0.5f; // compression Ratio
    c->M = 3.0f; // decibel Width of knee transition
    c->W = 1.0f; // decibel Make-up gain
    
    return c;
}
 */
int ccount = 0;
float tCompressorTick(tCompressor* c, float in)
{
    float slope, overshoot, c_dB;
    float alphaAtt, alphaRel;
    
    float in_db = 20.0f * log10f( fmaxf( fabsf( in), 0.000001f)), out_db = 0.0f;
    
    c->y_T[1] = c->y_T[0];
    
    slope = c->R - 1.0f; // feed-forward topology; was 1/C->R - 1 
    
    overshoot = in_db - c->T;
    
    
    
    if (overshoot <= -(c->W * 0.5f))
        out_db = in_db;
    else if ((overshoot > -(c->W * 0.5f)) && (overshoot < (c->W * 0.5f)))
        out_db = in_db + slope * (powf((overshoot + c->W*0.5f),2) / (2.0f * c->W)); // .^ 2 ???
    else if (overshoot >= (c->W * 0.5f))
        out_db = in_db + slope * overshoot;
    
    
    
    c->x_T[0] = out_db - in_db;
    
    alphaAtt = expf(-1.0f/(0.001f * c->tauAttack * oops.sampleRate));
    alphaRel = expf(-1.0f/(0.001f * c->tauRelease * oops.sampleRate));
    
    if (c->x_T[0] > c->y_T[1])
        c->y_T[0] = alphaAtt * c->y_T[1] + (1-alphaAtt) * c->x_T[0];
    else
        c->y_T[0] = alphaRel * c->y_T[1] + (1-alphaRel) * c->x_T[0];
    
    float attenuation = powf(10.0f, ((c->M - c->y_T[0])/20.0f));
    
    return attenuation * in;
    

}


#endif
#if N_ENVELOPE
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ Envelope ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
tEnvelope*    tEnvelopeInit(float attack, float decay, oBool loop)
{
    tEnvelope* env = &oops.tEnvelopeRegistry[oops.registryIndex[T_ENVELOPE]++];
    
    env->exp_buff = exp_decay;
    env->inc_buff = attack_decay_inc;
    env->buff_size = sizeof(exp_decay);
    
    env->loop = loop;
    
    if (attack > 8192.0f)
        attack = 8192.0f;
    if (attack < 0.0f)
        attack = 0.0f;
    
    if (decay > 8192.0f)
        decay = 8192.0f;
    if (decay < 0.0f)
        decay = 0.0f;
    
    int16_t attackIndex = ((int16_t)(attack * 8.0f))-1;
    int16_t decayIndex = ((int16_t)(decay * 8.0f))-1;
    int16_t rampIndex = ((int16_t)(2.0f * 8.0f))-1;
    
    if (attackIndex < 0)
        attackIndex = 0;
    if (decayIndex < 0)
        decayIndex = 0;
    if (rampIndex < 0)
        rampIndex = 0;
    
    env->inRamp = OFALSE;
    env->inAttack = OFALSE;
    env->inDecay = OFALSE;
    
    env->attackInc = env->inc_buff[attackIndex];
    env->decayInc = env->inc_buff[decayIndex];
    env->rampInc = env->inc_buff[rampIndex];
    
    return env;
    
}

int     tEnvelopeSetAttack(tEnvelope* const env, float attack)
{
    int16_t attackIndex;
    
    if (attack < 0.0f) {
        attackIndex = 0.0f;
    } else if (attack < 8192.0f) {
        attackIndex = ((int16_t)(attack * 8.0f))-1;
    } else {
        attackIndex = ((int16_t)(8192.0f * 8.0f))-1;
    }
    
    env->attackInc = env->inc_buff[attackIndex];
    
    return 0;
}

int     tEnvelopeSetDecay(tEnvelope* const env, float decay)
{
    int16_t decayIndex;
    
    if (decay < 0.0f) {
        decayIndex = 0.0f;
    } else if (decay < 8192.0f) {
        decayIndex = ((int16_t)(decay * 8.0f))-1;
    } else {
        decayIndex = ((int16_t)(8192.0f * 8.0f))-1;
    }
    
    env->decayInc = env->inc_buff[decayIndex]; 
    
    return 0;
}

int     tEnvelopeLoop(tEnvelope* const env, oBool loop)
{
    env->loop = loop;
    
    return 0;
}


int     tEnvelopeOn(tEnvelope* const env, float velocity)
{
    if (env->inAttack || env->inDecay) // In case envelope retriggered while it is still happening.
    {
        env->rampPhase = 0;
        env->inRamp = OTRUE;
        env->rampPeak = env->next;
    }
    else // Normal start.
    {
        env->inAttack = OTRUE;
    }
    
    
    env->attackPhase = 0;
    env->decayPhase = 0;
    env->inDecay = OFALSE;
    env->gain = velocity;
    
    return 0;
}

float   tEnvelopeTick(tEnvelope* const env)
{
    if (env->inRamp)
    {
        if (env->rampPhase > UINT16_MAX)
        {
            env->inRamp = OFALSE;
            env->inAttack = OTRUE;
            env->next = 0.0f;
        }
        else
        {
            env->next = env->rampPeak * env->exp_buff[(uint32_t)env->rampPhase];
        }
        
        env->rampPhase += env->rampInc;
    }
    
    if (env->inAttack)
    {
        
        // If attack done, time to turn around.
        if (env->attackPhase > UINT16_MAX)
        {
            env->inDecay = OTRUE;
            env->inAttack = OFALSE;
            env->next = env->gain * 1.0f;
        }
        else
        {
            // do interpolation !
            env->next = env->gain * env->exp_buff[UINT16_MAX - (uint32_t)env->attackPhase]; // inverted and backwards to get proper rising exponential shape/perception
        }
        
        // Increment envelope attack.
        env->attackPhase += env->attackInc;
        
    }
    
    if (env->inDecay)
    {
        
        // If decay done, finish.
        if (env->decayPhase >= UINT16_MAX)
        {
            env->inDecay = OFALSE;
            
            if (env->loop)
            {
                env->attackPhase = 0;
                env->decayPhase = 0;
                env->inAttack = OTRUE;
            }
            else
            {
                env->next = 0.0f;
            }
            
        } else {
            
            env->next = env->gain * (env->exp_buff[(uint32_t)env->decayPhase]); // do interpolation !
        }
        
        // Increment envelope decay;
        env->decayPhase += env->decayInc;
    }
    
    return env->next;
}

#endif // N_ENVELOPE

#if N_ENVELOPEFOLLOW
/* Envelope Follower */
tEnvelopeFollower*    tEnvelopeFollowerInit(float attackThreshold, float decayCoeff)
{
    tEnvelopeFollower* e = &oops.tEnvelopeFollowerRegistry[oops.registryIndex[T_ENVELOPEFOLLOW]++];
    
    e->y = 0.0f;
    e->a_thresh = attackThreshold;
    e->d_coeff = decayCoeff;
    
    return e;
    
}

float   tEnvelopeFollowerTick(tEnvelopeFollower* const ef, float x)
{
    if (x < 0.0f ) x = -x;  /* Absolute value. */
    
    if ((x >= ef->y) && (x > ef->a_thresh)) ef->y = x;                      /* If we hit a peak, ride the peak to the top. */
    else                                    ef->y = ef->y * ef->d_coeff;    /* Else, exponential decay of output. */
    
    //ef->y = envelope_pow[(uint16_t)(ef->y * (float)UINT16_MAX)] * ef->d_coeff; //not quite the right behavior - too much loss of precision?
    //ef->y = powf(ef->y, 1.000009f) * ef->d_coeff;  // too expensive
    
    if( ef->y < VSF)   ef->y = 0.0f;
    
    return ef->y;
}

int     tEnvelopeFollowerDecayCoeff(tEnvelopeFollower* const ef, float decayCoeff)
{
    return ef->d_coeff = decayCoeff;
}

int     tEnvelopeFollowerAttackThresh(tEnvelopeFollower* const ef, float attackThresh)
{
    return ef->a_thresh = attackThresh;
}
#endif // N_ENVELOPEFOLLOW

#if N_RAMP
/* Ramp */
tRamp*    tRampInit(float time, int samples_per_tick)
{
    tRamp* ramp = &oops.tRampRegistry[oops.registryIndex[T_RAMP]];
    
    ramp->inv_sr_ms = 1.0f/(oops.sampleRate*0.001f);
    ramp->curr = 0.0f;
    ramp->dest = 0.0f;
    ramp->time = time;
    ramp->samples_per_tick = samples_per_tick;
    ramp->inc = ((ramp->dest - ramp->curr) / ramp->time * ramp->inv_sr_ms) * (float)ramp->samples_per_tick;
    
    ramp->sampleRateChanged = &tRampSampleRateChanged;
    
    oops.registryIndex[T_RAMP] += 1;
    
    return ramp;
}

int     tRampSetTime(tRamp* const r, float time)
{
    r->time = time;
    r->inc = ((r->dest-r->curr)/r->time * r->inv_sr_ms)*((float)r->samples_per_tick);
    return 0;
}

int     tRampSetDest(tRamp* const r, float dest)
{
    r->dest = dest;
    r->inc = ((r->dest-r->curr)/r->time * r->inv_sr_ms)*((float)r->samples_per_tick);
    return 0;
}

float   tRampTick(tRamp* const r) {
    
    r->curr += r->inc;
    
    if (((r->curr >= r->dest) && (r->inc > 0.0f)) || ((r->curr <= r->dest) && (r->inc < 0.0f))) r->inc = 0.0f;
    
    return r->curr;
}

void    tRampSampleRateChanged(tRamp* const r)
{
    r->inv_sr_ms = 1.0f / (oops.sampleRate * 0.001f);
    r->inc = ((r->dest-r->curr)/r->time * r->inv_sr_ms)*((float)r->samples_per_tick);
}

#endif //N_RAMP
