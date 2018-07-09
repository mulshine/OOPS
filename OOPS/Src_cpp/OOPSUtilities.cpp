/*
  ==============================================================================

    OOPSUtilities.c
    Created: 20 Jan 2017 12:02:17pm
    Author:  Michael R Mulshine

  ==============================================================================
*/


#if _WIN32 || _WIN64

#include "..\Inc\OOPSUtilities.h"
#include "..\Inc\OOPSWavetables.h"
#include "..\Inc\OOPS.h"
#include "..\Externals\d_fft_mayer.h"

#else

#include "../Inc/OOPSUtilities.h"
#include "../Inc/OOPSWavetables.h"
#include "../Inc/OOPS.h"
#include "../Externals/d_fft_mayer.h"


#endif

#define LOGTEN 2.302585092994

float mtof(float f)
{
    if (f <= -1500.0f) return(0);
    else if (f > 1499.0f) return(mtof(1499.0f));
    else return (8.17579891564f * exp(0.0577622650f * f));
}

float ftom(float f)
{
    return (f > 0 ? 17.3123405046f * log(.12231220585f * f) : -1500.0f);
}

float powtodb(float f)
{
    if (f <= 0) return (0);
    else
    {
        float val = 100 + 10.f/LOGTEN * log(f);
        return (val < 0 ? 0 : val);
    }
}

float rmstodb(float f)
{
    if (f <= 0) return (0);
    else
    {
        float val = 100 + 20.f/LOGTEN * log(f);
        return (val < 0 ? 0 : val);
    }
}

float dbtopow(float f)
{
    if (f <= 0)
        return(0);
    else
    {
        if (f > 870.0f)
            f = 870.0f;
        return (exp((LOGTEN * 0.1f) * (f-100.0f)));
    }
}

float dbtorms(float f)
{
    if (f <= 0)
        return(0);
    else
    {
        if (f > 485.0f)
            f = 485.0f;
    }
    return (exp((LOGTEN * 0.05f) * (f-100.0f)));
}
#if N_ENV

/* ---------------- env~ - simple envelope follower. ----------------- */
tEnv* tEnvInit(int ws, int hs)
{
    tEnv* x = &oops.tEnvRegistry[oops.registryIndex[T_ENV]++];
    
    int period = hs, npoints = ws;
    
    int i;
    
    if (npoints < 1) npoints = 1024;
    if (period < 1) period = npoints/2;
    if (period < npoints / MAXOVERLAP + 1)
        period = npoints / MAXOVERLAP + 1;

    x->x_npoints = npoints;
    x->x_phase = 0;
    x->x_period = period;
    
    x->windowSize = npoints;
    x->hopSize = period;
    
    for (i = 0; i < MAXOVERLAP; i++) x->x_sumbuf[i] = 0;
    for (i = 0; i < npoints; i++)
        x->buf[i] = (1.0f - cos((2 * 3.14159f * i) / npoints))/npoints;
    for (; i < npoints+INITVSTAKEN; i++) x->buf[i] = 0;
    
    x->x_f = 0;
    
    x->x_allocforvs = INITVSTAKEN;
    
    // ~ ~ ~ dsp ~ ~ ~
    if (x->x_period % oops.blockSize)
    {
        x->x_realperiod = x->x_period + oops.blockSize - (x->x_period % oops.blockSize);
    }
    else
    {
        x->x_realperiod = x->x_period;
    }
    // ~ ~ ~ ~ ~ ~ ~ ~
    
    return (x);
}

float tEnvTick (tEnv* x)
{
    return powtodb(x->x_result);
}

void tEnvProcessBlock(tEnv* x, float* in)
{
    int n = oops.blockSize;
    
    int count;
    t_sample *sump;
    in += n;
    for (count = x->x_phase, sump = x->x_sumbuf;
         count < x->x_npoints; count += x->x_realperiod, sump++)
    {
        t_sample *hp = x->buf + count;
        t_sample *fp = in;
        t_sample sum = *sump;
        int i;
        
        for (i = 0; i < n; i++)
        {
            fp--;
            sum += *hp++ * (*fp * *fp);
        }
        *sump = sum;
    }
    sump[0] = 0;
    x->x_phase -= n;
    if (x->x_phase < 0)
    {
        x->x_result = x->x_sumbuf[0];
        for (count = x->x_realperiod, sump = x->x_sumbuf;
             count < x->x_npoints; count += x->x_realperiod, sump++)
            sump[0] = sump[1];
        sump[0] = 0;
        x->x_phase = x->x_realperiod - n;
    }
}
#endif // N_ENV

#if N_COMPRESSOR
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ Compressor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

/*
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
*/
tCompressor*    tCompressorInit(void)
{
    tCompressor* c = &oops.tCompressorRegistry[oops.registryIndex[T_COMPRESSOR]++];
    
    c->tauAttack = 100;
    c->tauRelease = 100;
	
	  c->isActive = OFALSE;
    
    c->T = 0.0f; // Threshold
    c->R = 0.5f; // compression Ratio
    c->M = 3.0f; // decibel Width of knee transition
    c->W = 1.0f; // decibel Make-up gain
    
    return c;
}

int ccount = 0;
float tCompressorTick(tCompressor* c, float in)
{
    float slope, overshoot;
    float alphaAtt, alphaRel;
    
    float in_db = 20.0f * log10f( fmaxf( fabsf( in), 0.000001f)), out_db = 0.0f;
    
    c->y_T[1] = c->y_T[0];
    
    slope = c->R - 1.0f; // feed-forward topology; was 1/C->R - 1 
    
    overshoot = in_db - c->T;

    
    if (overshoot <= -(c->W * 0.5f))
		{
        out_db = in_db;
			  c->isActive = OFALSE;
		}
    else if ((overshoot > -(c->W * 0.5f)) && (overshoot < (c->W * 0.5f)))
		{
        out_db = in_db + slope * (powf((overshoot + c->W*0.5f),2) / (2.0f * c->W)); // .^ 2 ???
			  c->isActive = OTRUE;
		}
    else if (overshoot >= (c->W * 0.5f))
		{
        out_db = in_db + slope * overshoot;
			  c->isActive = OTRUE;
		}
    
    
    
    c->x_T[0] = out_db - in_db;
    
    alphaAtt = expf(-1.0f/(0.001f * c->tauAttack * oops.sampleRate));
    alphaRel = expf(-1.0f/(0.001f * c->tauRelease * oops.sampleRate));
    
    if (c->x_T[0] > c->y_T[1])
        c->y_T[0] = alphaAtt * c->y_T[1] + (1-alphaAtt) * c->x_T[0];
    else
        c->y_T[0] = alphaRel * c->y_T[1] + (1-alphaRel) * c->x_T[0];
    
    float attenuation = powf(10.0f, ((c->M - c->y_T[0])/20.0f));
    
    /*
    if (++ccount > 5000)
    {
        
        ccount = 0;
        DBG(".5width: " + String(c->W * 0.5f));
        DBG("slope: " + String(slope) + " overshoot: " + String(overshoot));
        DBG("attenuation: " + String(attenuation));
    }
    */
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
    int32_t attackIndex;
    
    if (attack < 0.0f) {
        attackIndex = 0.0f;
    } else if (attack < 8192.0f) {
        attackIndex = ((int32_t)(attack * 8.0f))-1;
    } else {
        attackIndex = ((int32_t)(8192.0f * 8.0f))-1;
    }
    
    env->attackInc = env->inc_buff[attackIndex];
    
    return 0;
}

int     tEnvelopeSetDecay(tEnvelope* const env, float decay)
{
    int32_t decayIndex;
    
    if (decay < 0.0f) {
        decayIndex = 0.0f;
    } else if (decay < 8192.0f) {
        decayIndex = ((int32_t)(decay * 8.0f)) - 1;
    } else {
        decayIndex = ((int32_t)(8192.0f * 8.0f)) - 1; 
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

#if N_ADSR
/* ADSR */
tADSR*    tADSRInit(float attack, float decay, float sustain, float release)
{
    tADSR* adsr = &oops.tADSRRegistry[oops.registryIndex[T_ADSR]++];
    
    adsr->exp_buff = exp_decay;
    adsr->inc_buff = attack_decay_inc;
    adsr->buff_size = sizeof(exp_decay);
    
    if (attack > 8192.0f)
        attack = 8192.0f;
    if (attack < 0.0f)
        attack = 0.0f;
    
    if (decay > 8192.0f)
        decay = 8192.0f;
    if (decay < 0.0f)
        decay = 0.0f;
    
    if (sustain > 1.0f)
        sustain = 1.0f;
    if (sustain < 0.0f)
        sustain = 0.0f;
    
    if (release > 8192.0f)
        release = 8192.0f;
    if (release < 0.0f)
        release = 0.0f;
    
    int16_t attackIndex = ((int16_t)(attack * 8.0f))-1;
    int16_t decayIndex = ((int16_t)(decay * 8.0f))-1;
    int16_t releaseIndex = ((int16_t)(release * 8.0f))-1;
    int16_t rampIndex = ((int16_t)(2.0f * 8.0f))-1;
    
    if (attackIndex < 0)
        attackIndex = 0;
    if (decayIndex < 0)
        decayIndex = 0;
    if (releaseIndex < 0)
        releaseIndex = 0;
    if (rampIndex < 0)
        rampIndex = 0;
    
    adsr->inRamp = OFALSE;
    adsr->inAttack = OFALSE;
    adsr->inDecay = OFALSE;
    adsr->inSustain = OFALSE;
    adsr->inRelease = OFALSE;
    
    adsr->sustain = sustain;
    
    adsr->attackInc = adsr->inc_buff[attackIndex];
    adsr->decayInc = adsr->inc_buff[decayIndex];
    adsr->releaseInc = adsr->inc_buff[releaseIndex];
    adsr->rampInc = adsr->inc_buff[rampIndex];
    
    return adsr;
    
}

int     tADSRSetAttack(tADSR* const adsr, float attack)
{
    int32_t attackIndex;
    
    if (attack < 0.0f) {
        attackIndex = 0.0f;
    } else if (attack < 8192.0f) {
        attackIndex = ((int32_t)(attack * 8.0f))-1;
    } else {
        attackIndex = ((int32_t)(8192.0f * 8.0f))-1;
    }
    
    adsr->attackInc = adsr->inc_buff[attackIndex];
    
    return 0;
}

int     tADSRSetDecay(tADSR* const adsr, float decay)
{
    int32_t decayIndex;
    
    if (decay < 0.0f) {
        decayIndex = 0.0f;
    } else if (decay < 8192.0f) {
        decayIndex = ((int32_t)(decay * 8.0f)) - 1;
    } else {
        decayIndex = ((int32_t)(8192.0f * 8.0f)) - 1;
    }
    
    adsr->decayInc = adsr->inc_buff[decayIndex];
    
    return 0;
}

int     tADSRSetSustain(tADSR *const adsr, float sustain)
{
    if (sustain > 1.0f)      adsr->sustain = 1.0f;
    else if (sustain < 0.0f) adsr->sustain = 0.0f;
    else                     adsr->sustain = sustain;
    
    return 0;
}

int     tADSRSetRelease(tADSR* const adsr, float release)
{
    int32_t releaseIndex;
    
    if (release < 0.0f) {
        releaseIndex = 0.0f;
    } else if (release < 8192.0f) {
        releaseIndex = ((int32_t)(release * 8.0f)) - 1;
    } else {
        releaseIndex = ((int32_t)(8192.0f * 8.0f)) - 1;
    }
    
    adsr->releaseInc = adsr->inc_buff[releaseIndex];
    
    return 0;
}

int tADSROn(tADSR* const adsr, float velocity)
{
    if ((adsr->inAttack || adsr->inDecay) || (adsr->inSustain || adsr->inRelease)) // In case ADSR retriggered while it is still happening.
    {
        adsr->rampPhase = 0;
        adsr->inRamp = OTRUE;
        adsr->rampPeak = adsr->next;
    }
    else // Normal start.
    {
        adsr->inAttack = OTRUE;
    }
    
    adsr->attackPhase = 0;
    adsr->decayPhase = 0;
    adsr->releasePhase = 0;
    adsr->inDecay = OFALSE;
    adsr->inSustain = OFALSE;
    adsr->inRelease = OFALSE;
    adsr->gain = velocity;
    
    return 0;
}

int tADSROff(tADSR* const adsr)
{
    if (adsr->inRelease) return 0;
    
    adsr->inAttack = OFALSE;
    adsr->inDecay = OFALSE;
    adsr->inSustain = OFALSE;
    adsr->inRelease = OTRUE;
    
    adsr->releasePeak = adsr->next;
    
    return 0;
}

float   tADSRTick(tADSR* const adsr)
{
    if (adsr->inRamp)
    {
        if (adsr->rampPhase > UINT16_MAX)
        {
            adsr->inRamp = OFALSE;
            adsr->inAttack = OTRUE;
            adsr->next = 0.0f;
        }
        else
        {
            adsr->next = adsr->rampPeak * adsr->exp_buff[(uint32_t)adsr->rampPhase];
        }
        
        adsr->rampPhase += adsr->rampInc;
    }
    
    if (adsr->inAttack)
    {
        
        // If attack done, time to turn around.
        if (adsr->attackPhase > UINT16_MAX)
        {
            adsr->inDecay = OTRUE;
            adsr->inAttack = OFALSE;
            adsr->next = adsr->gain * 1.0f;
        }
        else
        {
            // do interpolation !
            adsr->next = adsr->gain * adsr->exp_buff[UINT16_MAX - (uint32_t)adsr->attackPhase]; // inverted and backwards to get proper rising exponential shape/perception
        }
        
        // Increment ADSR attack.
        adsr->attackPhase += adsr->attackInc;
        
    }
    
    if (adsr->inDecay)
    {
        
        // If decay done, sustain.
        if (adsr->decayPhase >= UINT16_MAX)
        {
            adsr->inDecay = OFALSE;
            adsr->inSustain = OTRUE;
            adsr->next = adsr->gain * adsr->sustain;
        }
        
        else
        {
            adsr->next = adsr->gain * (adsr->sustain + ((adsr->exp_buff[(uint32_t)adsr->decayPhase]) * (1 - adsr->sustain))); // do interpolation !
        }
        
        // Increment ADSR decay.
        adsr->decayPhase += adsr->decayInc;
    }

    if (adsr->inRelease)
    {
        // If release done, finish.
        if (adsr->releasePhase >= UINT16_MAX)
        {
            adsr->inRelease = OFALSE;
            adsr->next = 0.0f;
        }
        else {
            
            adsr->next = adsr->releasePeak * (adsr->exp_buff[(uint32_t)adsr->releasePhase]); // do interpolation !
        }
        
        // Increment envelope release;
        adsr->releasePhase += adsr->releaseInc;
    }

    return adsr->next;
}

#endif // N_ADSR

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
		ramp->minimum_time = ramp->inv_sr_ms * samples_per_tick;
    ramp->curr = 0.0f;
    ramp->dest = 0.0f;
		if (time < ramp->minimum_time)
		{
			ramp->time = ramp->minimum_time;
		}
		else
		{
			ramp->time = time;
		}
    ramp->samples_per_tick = samples_per_tick;
    ramp->inc = ((ramp->dest - ramp->curr) / ramp->time * ramp->inv_sr_ms) * (float)ramp->samples_per_tick;
    
    ramp->sampleRateChanged = &tRampSampleRateChanged;
    
    oops.registryIndex[T_RAMP] += 1;
    
    return ramp;
}

int     tRampSetTime(tRamp* const r, float time)
{
		if (time < r->minimum_time)
		{
			r->time = r->minimum_time;
		}
		else
		{
			r->time = time;
		}
		
		r->time = time;
    r->inc = ((r->dest-r->curr)/r->time * r->inv_sr_ms) * ((float)r->samples_per_tick);
    return 0;
}

int     tRampSetDest(tRamp* const r, float dest)
{
    r->dest = dest;
    r->inc = ((r->dest-r->curr)/r->time * r->inv_sr_ms) * ((float)r->samples_per_tick);
    return 0;
}

float   tRampTick(tRamp* const r) {
    
    r->curr += r->inc;
    
    if (((r->curr >= r->dest) && (r->inc > 0.0f)) || ((r->curr <= r->dest) && (r->inc < 0.0f))) r->inc = 0.0f;
    
    return r->curr;
}

float   tRampSample(tRamp* const r) {
  
    return r->curr;
}

void    tRampSampleRateChanged(tRamp* const r)
{
    r->inv_sr_ms = 1.0f / (oops.sampleRate * 0.001f);
    r->inc = ((r->dest-r->curr)/r->time * r->inv_sr_ms)*((float)r->samples_per_tick);
}

#endif //N_RAMP

#if N_POLY
/* Poly Handler */

static void nodeListInit(tPoly* poly)
{
    for (int16_t i = 0; i < 128; i++)
    {
        poly->midiNodes[i].midiNote.pitch = i;
        poly->midiNodes[i].midiNote.velocity = 0;
        poly->midiNodes[i].midiNote.on = OFALSE;
    }
}

// Initially everything is off, init as such
static void offListInit(tPoly* poly)
{
    tMidiNode* prevNode = NULL;
    tMidiNode* curNode = &poly->midiNodes[0];
    poly->offListFirst = curNode;
    
    for (int16_t i = 1; i < 128; i++)
    {
        curNode->prev = prevNode;
        curNode->next = &poly->midiNodes[i];
        
        prevNode = curNode;
        curNode = &poly->midiNodes[i];
    }
    // Set the final node
    curNode->prev = prevNode;
    curNode->next = NULL;
}

static void onListInit(tPoly* poly)
{
    poly->onListFirst = NULL;
}

tPoly*    tPolyInit()
{
    tPoly* poly = &oops.tPolyRegistry[oops.registryIndex[T_POLY]++];
    nodeListInit(poly);
    offListInit(poly);
    onListInit(poly);
    
    return poly;
}


tMidiNote* tPolyGetMidiNote(tPoly* poly, int8_t voiceIndex)
{
    tMidiNote* midiNote = NULL;
    
    tMidiNode* currNode = poly->onListFirst;
    
    int8_t i = 0;
    while (i < voiceIndex && currNode != NULL)
    {
        currNode = currNode->next;
        i++;
    }
    
    if (currNode != NULL)
        midiNote = &(currNode->midiNote);
    
    return midiNote;
}

static void removeNoteFromOffList(tPoly* poly, int8_t midiNoteNumber)
{
    // If this has no prev, this is the first node on the OFF list
    if (poly->midiNodes[midiNoteNumber].prev == NULL)
        poly->offListFirst = poly->midiNodes[midiNoteNumber].next;
    
    // Awkward handling to avoid deref null pointers
    if (poly->midiNodes[midiNoteNumber].prev != NULL)
        poly->midiNodes[midiNoteNumber].prev->next = poly->midiNodes[midiNoteNumber].next;
    
    if (poly->midiNodes[midiNoteNumber].next != NULL)
        poly->midiNodes[midiNoteNumber].next->prev = poly->midiNodes[midiNoteNumber].prev;
    
    poly->midiNodes[midiNoteNumber].next = NULL;
    poly->midiNodes[midiNoteNumber].prev = NULL;
}

static void prependNoteToOnList(tPoly* poly, int midiNoteNumber)
{
    if (poly->onListFirst != NULL)
    {
        poly->midiNodes[midiNoteNumber].next = poly->onListFirst;
        poly->onListFirst->prev = &poly->midiNodes[midiNoteNumber];
    }
    poly->onListFirst = &poly->midiNodes[midiNoteNumber];
}


// TODO: Fail gracefully on odd MIDI situations
//       For example, getting a note off for an already on note and vice-versa
void tPolyNoteOn(tPoly* poly, int midiNoteNumber, float velocity)
{
    removeNoteFromOffList(poly, midiNoteNumber);
    // Set the MIDI note on accordingly
    poly->midiNodes[midiNoteNumber].midiNote.velocity = velocity;
    poly->midiNodes[midiNoteNumber].midiNote.on = OTRUE;
    prependNoteToOnList(poly, midiNoteNumber);
}

// Unfortunately similar code to removeNoteFromOffList without any clear way of factoring out to a helper function
static void removeNoteFromOnList(tPoly* poly, int8_t midiNoteNumber)
{
    // If this has no prev, this is the first node on the OFF list
    if (poly->midiNodes[midiNoteNumber].prev == NULL)
        poly->onListFirst = poly->midiNodes[midiNoteNumber].next;
    
    // Awkward handling to avoid deref null pointers
    if (poly->midiNodes[midiNoteNumber].prev != NULL)
        poly->midiNodes[midiNoteNumber].prev->next = poly->midiNodes[midiNoteNumber].next;
    
    if (poly->midiNodes[midiNoteNumber].next != NULL)
        poly->midiNodes[midiNoteNumber].next->prev = poly->midiNodes[midiNoteNumber].prev;
    
    poly->midiNodes[midiNoteNumber].next = NULL;
    poly->midiNodes[midiNoteNumber].prev = NULL;
}

// Unfortunately similar code to prependNoteToOnList without any clear way of factoring out to a helper function
static void prependNoteToOffList(tPoly* poly, int midiNoteNumber)
{
    if (poly->offListFirst != NULL)
    {
        poly->midiNodes[midiNoteNumber].next = poly->offListFirst;
        poly->offListFirst->prev = &poly->midiNodes[midiNoteNumber];
    }
    poly->offListFirst = &poly->midiNodes[midiNoteNumber];
}


void tPolyNoteOff(tPoly* poly, int midiNoteNumber)
{
    removeNoteFromOnList(poly, midiNoteNumber);
    // Set the MIDI note on accordingly
    poly->midiNodes[midiNoteNumber].midiNote.velocity = 0;
    poly->midiNodes[midiNoteNumber].midiNote.on = OFALSE;
    prependNoteToOffList(poly, midiNoteNumber);
}

#endif // N_POLYHANDLER

#if N_STACK
// If stack contains note, returns index. Else returns -1;
int tStack_contains(tStack* const ns, uint16_t noteVal)
{
    for (int i = 0; i < ns->size; i++)
    {
        if (ns->data[i] == noteVal)    return i;
    }
    return -1;
}

void tStack_add(tStack* const ns, uint16_t noteVal)
{
    uint8_t j;
    
    int whereToInsert = 0;
    if (ns->ordered)
    {
        for (j = 0; j < ns->size; j++)
        {
            if (noteVal > ns->data[j])
            {
                if ((noteVal < ns->data[j+1]) || (ns->data[j+1] == -1))
                {
                    whereToInsert = j+1;
                    break;
                }
            }
        }
    }
    
    //first move notes that are already in the stack one position to the right
    for (j = ns->size; j > whereToInsert; j--)
    {
        ns->data[j] = ns->data[(j - 1)];
    }
    
    //then, insert the new note into the front of the stack
    ns->data[whereToInsert] = noteVal;
    
    ns->size++;
}

int tStack_addIfNotAlreadyThere(tStack* const ns, uint16_t noteVal)
{
    uint8_t j;

    int added = 0;
    
    if (tStack_contains(ns, noteVal) == -1)
    {
        int whereToInsert = 0;
        if (ns->ordered)
        {
            for (j = 0; j < ns->size; j++)
            {
                if (noteVal > ns->data[j])
                {
                    if ((noteVal < ns->data[j+1]) || (ns->data[j+1] == -1))
                    {
                        whereToInsert = j+1;
                        break;
                    }
                }
            }
        }
        
        //first move notes that are already in the stack one position to the right
        for (j = ns->size; j > whereToInsert; j--)
        {
            ns->data[j] = ns->data[(j - 1)];
        }
        
        //then, insert the new note into the front of the stack
        ns->data[whereToInsert] = noteVal;
        
        ns->size++;
        
        added = 1;
    }
    
    return added;
}


// Remove noteVal. return 1 if removed, 0 if not
int tStack_remove(tStack* const ns, uint16_t noteVal)
{
    uint8_t k;
    int foundIndex = tStack_contains(ns, noteVal);
    int removed = 0;
    
    if (foundIndex >= 0)
    {
        for (k = 0; k < (ns->size - foundIndex); k++)
        {
            if ((k+foundIndex) >= (ns->capacity - 1))
            {
                ns->data[k + foundIndex] = -1;
            }
            else
            {
                ns->data[k + foundIndex] = ns->data[k + foundIndex + 1];
                if ((k + foundIndex) == (ns->size - 1))
                {
                    ns->data[k + foundIndex + 1] = -1;
                }
            }
            
        }
        // in case it got put on the stack multiple times
        foundIndex--;
        ns->size--;
        removed = 1;
    }
    
    
    
    return removed;
}

// Doesn't change size of data types
void tStack_setCapacity(tStack* const ns, uint16_t cap)
{
    if (cap <= 0)
        ns->capacity = 1;
    else if (cap <= STACK_SIZE)
        ns->capacity = cap;
    else
        ns->capacity = STACK_SIZE;
    
    for (int i = cap; i < STACK_SIZE; i++)
    {
        if ((int)ns->data[i] != -1)
        {
            ns->data[i] = -1;
            ns->size -= 1;
        }
    }
    
    if (ns->pos >= cap)
    {
        ns->pos = 0;
    }
}

int tStack_getSize(tStack* const ns)
{
    return ns->size;
}

void tStack_clear(tStack* const ns)
{
    for (int i = 0; i < STACK_SIZE; i++)
    {
        ns->data[i] = -1;
    }
    ns->pos = 0;
    ns->size = 0;
}

// Next item in order of addition to stack. Return 0-31 if there is a next item to move to. Returns -1 otherwise.
int tStack_next(tStack* const ns)
{
    int step = 0;
    if (ns->size != 0) // if there is at least one note in the stack
    {
        if (ns->pos > 0) // if you're not at the most recent note (first one), then go backward in the array (moving from earliest to latest)
        {
            ns->pos--;
        }
        else
        {
            ns->pos = (ns->size - 1); // if you are the most recent note, go back to the earliest note in the array
        }
        
        step = ns->data[ns->pos];
        return step;
    }
    else
    {
        return -1;
    }
}

int tStack_get(tStack* const ns, int which)
{
    return ns->data[which];
}

int tStack_first(tStack* const ns)
{
    return ns->data[0];
}

tStack* tStack_init(void)
{
    tStack* ns = &oops.tStackRegistry[oops.registryIndex[T_STACK]++];
    
    ns->ordered = OFALSE;
    ns->size = 0;
    ns->pos = 0;
    ns->capacity = STACK_SIZE;
    
    for (int i = 0; i < STACK_SIZE; i++) ns->data[i] = -1;
    
    return ns;
}

#endif // N_STACK

#if N_MPOLY

tMPoly* tMPoly_init(void)
{
    tMPoly* poly = &oops.tMPolyRegistry[oops.registryIndex[T_MPOLY]++];
    
    poly->numVoices = NUM_VOICES;
    poly->numVoicesActive = NUM_VOICES;
    poly->lastVoiceToChange = 0;
    
    // Arp mode stuff
    poly->currentVoice = 0;
    poly->maxLength = 128;
    poly->currentNote = -1;

    //default learned CCs and notes are just the CCs 1-128 - notes are skipped
    for (int i = 0; i < 128; i++)
    {
        poly->notes[i][0] = 0;
        poly->notes[i][1] = 0;
    }
    
    for (int i = 0; i < NUM_VOICES; i ++)
    {
        poly->voices[i][0] = -1;
    }
    
    poly->stack = tStack_init();
    poly->orderStack = tStack_init();
    
    return poly;
}


//instead of including in dacsend, should have a separate pitch bend ramp, that is added when the ramps are ticked and sent to DAC
void tMPoly_pitchBend(tMPoly* poly, uint8_t pitchBend)
{

}

void tMPoly_noteOn(tMPoly* poly, int note, uint8_t vel)
{
    // if not in keymap or already on stack, dont do anything. else, add that note.
    if (tStack_contains(poly->stack, note) >= 0) return;
    else
    {
        tMPoly_orderedAddToStack(poly, note);
        tStack_add(poly->stack, note);
        
        oBool found = OFALSE;
        for (int i = 0; i < poly->numVoices; i++)
        {
            if (poly->voices[i][0] < 0)    // if inactive voice, give this note to voice
            {
                found = OTRUE;
                
                poly->voices[i][0] = note;
                poly->voices[i][1] = vel;
                poly->lastVoiceToChange = i;
                poly->notes[note][0] = vel;
                poly->notes[note][1] = 1;
                break;
            }
        }
        
        if (!found) //steal
        {
            int whichVoice = poly->lastVoiceToChange;
            int oldNote = poly->voices[whichVoice][0];
            poly->voices[whichVoice][0] = note;
            poly->voices[whichVoice][1] = vel;
            poly->notes[oldNote][1] = 0; //mark the stolen voice as inactive (in the second dimension of the notes array)
            
            poly->notes[note][0] = vel;
            poly->notes[note][1] = OTRUE;
        }
        
    }
}


int16_t noteToTest = -1;

void tMPoly_noteOff(tMPoly* poly, uint8_t note)
{
    tStack_remove(poly->stack, note);
    tStack_remove(poly->orderStack, note);
    poly->notes[note][0] = 0;
    poly->notes[note][1] = 0;
    
    int deactivatedVoice = -1;
    for (int i = 0; i < poly->numVoices; i++)
    {
        if (poly->voices[i][0] == note)
        {
            poly->voices[i][0] = -1;
            poly->voices[i][1] = 0;
            poly->lastVoiceToChange = i;
            deactivatedVoice = i;
            break;
        }
    }
    //monophonic handling
    if ((poly->numVoices == 1) && (tStack_getSize(poly->stack) > 0))
    {
        int oldNote = tStack_first(poly->stack);
        poly->voices[0][0] = oldNote;
        poly->voices[0][1] = poly->notes[oldNote][0];
        poly->lastVoiceToChange = 0;
    }
    
    //grab old notes off the stack if there are notes waiting to replace the free voice
    else if (deactivatedVoice != -1)
    {
        int i = 0;
        
        while (1)
        {
            noteToTest = tStack_get(poly->stack, i++);
            if (noteToTest < 0 ) break;
            
            if (poly->notes[noteToTest][1] == 0) //if there is a stolen note waiting (marked inactive but on the stack)
            {
                poly->voices[deactivatedVoice][0] = noteToTest; //set the newly free voice to use the old stolen note
                poly->voices[deactivatedVoice][1] = poly->notes[noteToTest][0]; // set the velocity of the voice to be the velocity of that note
                poly->notes[noteToTest][1] = 1; //mark that it is no longer stolen and is now active
                poly->lastVoiceToChange = deactivatedVoice; // mark the voice that was just changed as the last voice to change
                break;
            }
        }
    }
    
    
}

void tMPoly_orderedAddToStack(tMPoly* poly, uint8_t noteVal)
{
    
    uint8_t j;
    int myPitch, thisPitch, nextPitch;
    
    tStack* ns = poly->orderStack;
    
    int whereToInsert = 0;
    
    for (j = 0; j < ns->size; j++)
    {
        myPitch = noteVal;
        thisPitch = ns->data[j];
        nextPitch = ns->data[j+1];
        
        if (myPitch > thisPitch)
        {
            if ((myPitch < nextPitch) || (nextPitch == -1))
            {
                whereToInsert = j+1;
                break;
            }
        }
    }
    
    //first move notes that are already in the stack one position to the right
    for (j = ns->size; j > whereToInsert; j--)
    {
        ns->data[j] = ns->data[(j - 1)];
    }
    
    //then, insert the new note into the front of the stack
    ns->data[whereToInsert] =  noteVal;
    
    ns->size++;
    
}

#endif //N_MPOLY

#if N_SOLAD

/******************************************************************************/
/***************** static function declarations *******************************/
/******************************************************************************/

static void solad_init(tSOLAD *w);
static inline float read_sample(tSOLAD *w, float floatindex);
static void pitchdown(tSOLAD *w, float *out);
static void pitchup(tSOLAD *w, float *out);

/******************************************************************************/
/***************** public access functions ************************************/
/******************************************************************************/

// init
tSOLAD*     tSOLAD_init(void)
{
    tSOLAD *w = &oops.tSOLADRegistry[oops.registryIndex[T_SOLAD]++];

    w->pitchfactor = 1.;
    
    solad_init(w);
    
    return w;
}

// send one block of input samples, receive one block of output samples
void tSOLAD_ioSamples(tSOLAD *w, float* in, float* out, int blocksize)
{
    int i = w->timeindex;
    int n = w->blocksize = blocksize;
    
    if(!i) w->delaybuf[LOOPSIZE] = in[0];   // copy one sample for interpolation
    while(n--) w->delaybuf[i++] = *in++;    // copy one input block to delay buffer
    
    if(w->pitchfactor > 1) pitchup(w, out);
    else pitchdown(w, out);
    
    w->timeindex += blocksize;
    w->timeindex &= LOOPMASK;
}

// set periodicity analysis data
void tSOLAD_setPeriod(tSOLAD *w, float period)
{
    if(period > MAXPERIOD) period = MAXPERIOD;
    if(period > MINPERIOD) w->period = period;  // ignore period when too small
}

// set pitch factor between 0.25 and 4
void tSOLAD_setPitchFactor(tSOLAD *w, float pitchfactor)
{
    if(pitchfactor < 0.25) pitchfactor = 0.25;
    else if(pitchfactor > 4.) pitchfactor = 4.;
    w->pitchfactor = pitchfactor;
}

// force readpointer lag
void tSOLAD_setReadLag(tSOLAD *w, float readlag)
{
    if(readlag < 0) readlag = 0;
    if(readlag < w->readlag)               // do not jump backward, only forward
    {
        w->jump = w->readlag - readlag;
        w->readlag = readlag;
        w->xfadelength = readlag;
        w->xfadevalue = 1;
    }
}

// reset state variables
void tSOLAD_resetState(tSOLAD *w)
{
    int n = LOOPSIZE + 1;
    float *buf = w->delaybuf;
    
    while(n--) *buf++ = 0;
    solad_init(w);
}

/******************************************************************************/
/******************** private procedures **************************************/
/******************************************************************************/

/*
 Function pitchdown() is called to read samples from the delay buffer when pitch
 factor is between 0.25 and 1. The read pointer lags behind because of the slowed
 down speed, and it must jump forward towards the write pointer soon as there is
 sufficient space to jump. That is, if there is at least one period of the input
 signal between read pointer and write pointer.  When short periods follow up on
 long periods, the read pointer may have space to jump over more than one period
 lenghts. Jump length must be [periodlength ^ 2] in any case.
 
 A linear crossfade function joins the jump-from point with the jump-to point.
 The crossfade must be completed before another read pointer jump is allowed.
 Length of the crossfade function is stored as a number of samples in terms of
 the input sample rate. This length is dynamically translated
 to a crossfade length expressed in output reading rate, according to pitch
 factor which can change before the crossfade is completed. Crossfade length does
 not cover an invariable length in periods for all pitch transposition factors.
 For pitch factors from 0.5 till 1, crossfade length is stretched in the
 output just as much as the signal itself, as crossfade speed is set to equal
 pitch factor. For pitch factors below 0.5, the read pointer wants to jump
 forward before one period is read, therefore the crossfade length as expressed
 in output periods must be shorter. Crossfade speed is set to [1 - pitchfactor]
 for those cases. Pitch factor 0.5 is the natural switch point between crossfade
 speeds [pitchfactor] and [1 - pitchfactor] because 0.5 == 1 - 0.5. The crossfade
 speed modification for pitch factors below 0.5 also means that much of the
 original signal content will be skipped.
 */


static void pitchdown(tSOLAD *w, float *out)
{
    int n = w->blocksize;
    float refindex = (float)(w->timeindex + LOOPSIZE); // no negative values!
    float pitchfactor = w->pitchfactor;
    float period = w->period;
    float readlag = w->readlag;
    float readlagstep = 1 - pitchfactor;
    float jump = w->jump;
    float xfadevalue = w->xfadevalue;
    float xfadelength = w->xfadelength;
    float xfadespeed, xfadestep, readindex, outputsample;
    
    if(pitchfactor > 0.5) xfadespeed = pitchfactor;
    else xfadespeed = 1 - pitchfactor;
    xfadestep = xfadespeed / xfadelength;
    
    while(n--)
    {
        if(readlag > period)        // check if read pointer may jump forward...
        {
            if(xfadevalue <= 0)      // ...but do not interrupt crossfade
            {
                jump = period;                           // jump forward
                while((jump * 2) < readlag) jump *= 2;   // use available space
                readlag -= jump;                         // reduce read pointer lag
                xfadevalue = 1;                          // start crossfade
                xfadelength = period - 1;
                xfadestep = xfadespeed / xfadelength;
            }
        }
        
        readindex = refindex - readlag;
        outputsample = read_sample(w, readindex);
        
        if(xfadevalue > 0)
        {
            outputsample *= (1 - xfadevalue);                               // fadein
            outputsample += read_sample(w, readindex - jump) * xfadevalue;  // fadeout
            xfadevalue -= xfadestep;
        }
        
        *out++ = outputsample;
        refindex += 1;
        readlag += readlagstep;
    }
    
    w->jump = jump;                 // state variables
    w->readlag = readlag;
    w->xfadevalue = xfadevalue;
    w->xfadelength = xfadelength;
}


/*
 Function pitchup() for pitch factors above 1 is more complicated than
 pitchdown(). The read pointer increments faster than the write pointer and a
 backward jump must happen in time, reckoning with the crossfade region. The read
 pointer backward jump length is always one period. In order to minimize the area
 of signal duplicates, crossfade length is aimed at [period / pitchfactor].
 This leads to a crossfade speed of [pitchfactor * pitchfactor].
 
 Some samples for the fade out (but not all of them) must already be in the
 buffer, otherwise we will run out of input samples before the crossfade is
 completed. The ratio of past samples and future samples for a crossfade of any
 length is as follows:
 
 past samples: xfadelength * (1 - 1 / pitchfactor)
 future samples: xfadelength * (1 / pitchfactor)
 
 For example in the case of pitch factor 1.5 this would be:
 
 past samples: xfadelength * (1 - 1 / 1.5) = xfadelength * 1 / 3
 future samples: xfadelength * (1 / 1.5) = xfadelength * 2 / 3
 
 In the case of pitch factor 4 this would be:
 
 past samples: xfadelength * (1 - 1 / 4) = xfadelength * 3 / 4
 future samples: xfadelength * (1 / 4) = xfadelength * 1 / 4
 
 The read pointer lag must therefore preserve a minimum dependent on pitch
 factor. The minimum is called 'limit' here:
 
 limit = period * (pitchfactor - 1) / pitchfactor * pitchfactor
 
 Components of this expression are combined to reuse them in operations, while
 (pitchfactor - 1) is changed to (pitchfactor - 0.99) to avoid numerical
 resolution issues for pitch factors slightly above 1:
 
 xfadespeed = pitchfactor * pitchfactor
 limitfactor = (pitchfactor - 0.99) / xfadespeed
 limit = period * limitfactor
 
 When read lag is smaller than this limit, the read pointer must preferably
 jump backward, unless a previous crossfade is not yet completed. Crossfades must
 preferably be completed, unless the read pointer lag becomes smaller than zero.
 With fluctuating period lengths and pitch factors, the readpointer lag limit may
 change from one input block to the next in such a way that the actual lag is
 suddenly much smaller than the limit, and the intended crossfade length can not
 be applied. Therefore the crossfade length is simply calculated from the
 available amount of samples for all cases, like so:
 
 xfadelength = readlag / limitfactor
 
 For most occurrences, this will amount to a crossfade length reduced to
 [period / pitchfactor] in the output for pitch factors above 1, while in some
 cases it will be considerably shorter. Fortunately, an incidental aberration of
 the intended crossfade length hardly ever creates an audible artifact. The
 reason to specify preferred crossfade length according to pitch factor is to
 minimize the impression of echoes without sacrificing too much of the signal
 content. The readpointer jump length remains one period in any case.
 
 Sometimes, the input signal periodicity may decrease substantially between one
 signal block and the next. In such cases it may be possible for the read pointer
 to jump forward and reduce latency. For every signal block, a check on this
 possibility is done. A previous crossfade must be completed before a forward
 jump is allowed.
 */
static void pitchup(tSOLAD *w, float *out)
{
    int n = w->blocksize;
    float refindex = (float)(w->timeindex + LOOPSIZE); // no negative values
    float pitchfactor = w->pitchfactor;
    float period = w->period;
    float readlag = w->readlag;
    float jump = w->jump;
    float xfadevalue = w->xfadevalue;
    float xfadelength = w->xfadelength;
    
    float readlagstep = pitchfactor - 1;
    float xfadespeed = pitchfactor * pitchfactor;
    float xfadestep = xfadespeed / xfadelength;
    float limitfactor = (pitchfactor - (float)0.99) / xfadespeed;
    float limit = period * limitfactor;
    float readindex, outputsample;
    
    if((readlag > (period + 2 * limit)) & (xfadevalue < 0))
    {
        jump = period;                                        // jump forward
        while((jump * 2) < (readlag - 2 * limit)) jump *= 2;  // use available space
        readlag -= jump;                                      // reduce read pointer lag
        xfadevalue = 1;                                       // start crossfade
        xfadelength = period - 1;
        xfadestep = xfadespeed / xfadelength;
    }
    
    while(n--)
    {
        if(readlag < limit)  // check if read pointer should jump backward...
        {
            if((xfadevalue < 0) | (readlag < 0)) // ...but try not to interrupt crossfade
            {
                xfadelength = readlag / limitfactor;
                if(xfadelength < 1) xfadelength = 1;
                xfadestep = xfadespeed / xfadelength;
                
                jump = -period;         // jump backward
                readlag += period;      // increase read pointer lag
                xfadevalue = 1;         // start crossfade
            }
        }
        
        readindex = refindex - readlag;
        outputsample = read_sample(w, readindex);
        
        if(xfadevalue > 0)
        {
            outputsample *= (1 - xfadevalue);
            outputsample += read_sample(w, readindex - jump) * xfadevalue;
            xfadevalue -= xfadestep;
        }
        
        *out++ = outputsample;
        refindex += 1;
        readlag -= readlagstep;
    }
    
    w->readlag = readlag;               // state variables
    w->jump = jump;
    w->xfadelength = xfadelength;
    w->xfadevalue = xfadevalue;
}

// read one sample from delay buffer, with linear interpolation
static inline float read_sample(tSOLAD *w, float floatindex)
{
    int index = (int)floatindex;
    float fraction = floatindex - (float)index;
    float *buf = w->delaybuf;
    index &= LOOPMASK;
    
    return (buf[index] + (fraction * (buf[index+1] - buf[index])));
}

static void solad_init(tSOLAD *w)
{
    w->timeindex = 0;
    w->xfadevalue = -1;
    w->period = INITPERIOD;
    w->readlag = INITPERIOD;
    w->blocksize = INITPERIOD;
}


#endif // N_SOLAD

#if N_SNAC
/******************************************************************************/
/***************************** private procedures *****************************/
/******************************************************************************/

#define REALFFT mayer_realfft
#define REALIFFT mayer_realifft

static void snac_analyzeframe(tSNAC *s);
static void snac_autocorrelation(tSNAC *s);
static void snac_normalize(tSNAC *s);
static void snac_pickpeak(tSNAC *s);
static void snac_periodandfidelity(tSNAC *s);
static void snac_biasbuf(tSNAC *s);
static float snac_spectralpeak(tSNAC *s, float periodlength);


/******************************************************************************/
/******************************** constructor, destructor *********************/
/******************************************************************************/


tSNAC* tSNAC_init(int framearg, int overlaparg)
{
    
    tSNAC *s = &oops.tSNACRegistry[oops.registryIndex[T_SNAC]++];

    s->inputbuf = NULL;
    s->processbuf = NULL;
    s->spectrumbuf = NULL;
    s->biasbuf = NULL;
    s->biasfactor = DEFBIAS;
    s->timeindex = 0;
    s->periodindex = 0;
    s->periodlength = 0.;
    s->fidelity = 0.;
    s->minrms = DEFMINRMS;
    
    tSNAC_setFramesize(s, framearg);
    tSNAC_setOverlap(s, overlaparg);
    
    return s;
}
/******************************************************************************/
/************************** public access functions****************************/
/******************************************************************************/


void tSNAC_ioSamples(tSNAC *s, float *in, float *out, int size)
{
    int timeindex = s->timeindex;
    int mask = s->framesize - 1;
    int outindex = 0;
    float *inputbuf = s->inputbuf;
    float *processbuf = s->processbuf;
    
    // call analysis function when it is time
    if(!(timeindex & (s->framesize / s->overlap - 1))) snac_analyzeframe(s);
    
    while(size--)
    {
        inputbuf[timeindex] = *in++;
        out[outindex++] = processbuf[timeindex++];
        timeindex &= mask;
    }
    s->timeindex = timeindex;
    return;
}


// set framesize and (re)allocate buffers accordingly
void tSNAC_setFramesize(tSNAC *s, int frame)
{
    int n;
    
    if(!((frame==128)|(frame==256)|(frame==512)|(frame==1024)|(frame==2048)))
        frame = DEFFRAMESIZE;
    s->framesize = n = frame;
    
    s->inputbuf = (float*)realloc(s->inputbuf, s->framesize * sizeof(float));
    float *inputbuf = s->inputbuf;
    while(n--) *inputbuf++ = 0;
    
    s->processbuf = (float*)realloc(s->processbuf,
                                      s->framesize * 2 * sizeof(float));
    
    s->spectrumbuf = (float*)realloc(s->spectrumbuf,
                                       s->framesize * 0.5 * sizeof(float));
    
    s->biasbuf = (float*)realloc(s->biasbuf, s->framesize * sizeof(float));
    snac_biasbuf(s);
    
    s->timeindex = 0;
    return;
}


void tSNAC_setOverlap(tSNAC *s, int lap)
{
    if(!((lap==1)|(lap==2)|(lap==4)|(lap==8))) lap = DEFOVERLAP;
    s->overlap = lap;
}


void tSNAC_setBias(tSNAC *s, float bias)
{
    if(bias > 1.) bias = 1.;
    if(bias < 0.) bias = 0.;
    s->biasfactor = bias;
    snac_biasbuf(s);
    return;
}


void tSNAC_setMinRMS(tSNAC *s, float rms)
{
    if(rms > 1.) rms = 1.;
    if(rms < 0.) rms = 0.;
    s->minrms = rms;
    return;
}


float tSNAC_getPeriod(tSNAC *s)
{
    return(s->periodlength);
}


float tSNAC_getFidelity(tSNAC *s)
{
    return(s->fidelity);
}


/******************************************************************************/
/***************************** private procedures *****************************/
/******************************************************************************/


// main analysis function
static void snac_analyzeframe(tSNAC *s)
{
    int n, tindex = s->timeindex;
    int framesize = s->framesize;
    int mask = framesize - 1;
    float norm = 1. / sqrt((float)(framesize * 2));
    
    float *inputbuf = s->inputbuf;
    float *processbuf = s->processbuf;
    
    // copy input to processing buffers
    for(n=0; n<framesize; n++)
    {
        processbuf[n] = inputbuf[tindex] * norm;
        tindex++;
        tindex &= mask;
    }
    
    // zeropadding
    for(n=framesize; n<(framesize<<1); n++) processbuf[n] = 0.;
    
    // call analysis procedures
    snac_autocorrelation(s);
    snac_normalize(s);
    snac_pickpeak(s);
    snac_periodandfidelity(s);
}


static void snac_autocorrelation(tSNAC *s)
{
    int n, m;
    int framesize = s->framesize;
    int fftsize = framesize * 2;
    float *processbuf = s->processbuf;
    float *spectrumbuf = s->spectrumbuf;
    
    REALFFT(fftsize, processbuf);
    
    // compute power spectrum
    processbuf[0] *= processbuf[0];                      // DC
    processbuf[framesize] *= processbuf[framesize];      // Nyquist
    
    for(n=1; n<framesize; n++)
    {
        processbuf[n] = processbuf[n] * processbuf[n]
        + processbuf[fftsize-n] * processbuf[fftsize-n]; // imag coefficients appear reversed
        processbuf[fftsize-n] = 0.;
    }
    
    // store power spectrum up to SR/4 for possible later use
    for(m=0; m<(framesize>>1); m++)
    {
        spectrumbuf[m] = processbuf[m];
    }
    
    // transform power spectrum to autocorrelation function
    REALIFFT(fftsize, processbuf);
    return;
}


static void snac_normalize(tSNAC *s)
{
    int framesize = s->framesize;
    int framesizeplustimeindex = s->framesize + s->timeindex;
    int timeindexminusone = s->timeindex - 1;
    int n, m;
    int mask = framesize - 1;
    int seek = framesize * SEEK;
    float *inputbuf = s->inputbuf;
    float *processbuf= s->processbuf;
    float signal1, signal2;
    
    // minimum RMS implemented as minimum autocorrelation at index 0
    // functionally equivalent to white noise floor
    float rms = s->minrms / sqrt(1.0f / (float)framesize);
    float minrzero = rms * rms;
    float rzero = processbuf[0];
    if(rzero < minrzero) rzero = minrzero;
    double normintegral = (double)rzero * 2.;
    
    // normalize biased autocorrelation function
    // inputbuf is circular buffer: timeindex may be non-zero when overlap > 1
    processbuf[0] = 1;
    for(n=1, m=s->timeindex+1; n<seek; n++, m++)
    {
        signal1 = inputbuf[(n + timeindexminusone)&mask];
        signal2 = inputbuf[(framesizeplustimeindex - n)&mask];
        normintegral -= (double)(signal1 * signal1 + signal2 * signal2);
        processbuf[n] /= (float)normintegral * 0.5f;
    }
    
    // flush instable function tail
    for(n = seek; n<framesize; n++) processbuf[n] = 0.;
    return;
}


static void snac_periodandfidelity(tSNAC *s)
{
    float periodlength;
    
    if(s->periodindex)
    {
        periodlength = (float)s->periodindex +
        interpolate3phase(s->processbuf, s->periodindex);
        if(periodlength < 8) periodlength = snac_spectralpeak(s, periodlength);
        s->periodlength = periodlength;
        s->fidelity = interpolate3max(s->processbuf, s->periodindex);
    }
    return;
}

// select the peak which most probably represents period length
static void snac_pickpeak(tSNAC *s)
{
    int n, peakindex=0;
    int seek = s->framesize * SEEK;
    float *processbuf= s->processbuf;
    float maxvalue = 0.;
    float biasedpeak;
    float *biasbuf = s->biasbuf;
    
    // skip main lobe
    for(n=1; n<seek; n++)
    {
        if(processbuf[n] < 0.) break;
    }
    
    // find interpolated / biased maximum in SNAC function
    // interpolation finds the 'real maximum'
    // biasing favours the first candidate
    for(; n<seek-1; n++)
    {
        if(processbuf[n] >= processbuf[n-1])
        {
            if(processbuf[n] > processbuf[n+1])     // we have a local peak
            {
                biasedpeak = interpolate3max(processbuf, n) * biasbuf[n];
                
                if(biasedpeak > maxvalue)
                {
                    maxvalue = biasedpeak;
                    peakindex = n;
                }
            }
        }
    }
    s->periodindex = peakindex;
    return;
}


// verify period length via frequency domain (up till SR/4)
// frequency domain is more precise than lag domain for period lengths < 8
// argument 'periodlength' is initial estimation from autocorrelation
static float snac_spectralpeak(tSNAC *s, float periodlength)
{
    if(periodlength < 4.) return periodlength;
    
    float max = 0.;
    int n, startbin, stopbin, peakbin = 0;
    int spectrumsize = s->framesize>>1;
    float *spectrumbuf = s->spectrumbuf;
    float peaklocation = (float)(s->framesize * 2.) / periodlength;
    
    startbin = (int)(peaklocation * 0.8 + 0.5);
    if(startbin < 1) startbin = 1;
    stopbin = (int)(peaklocation * 1.25 + 0.5);
    if(stopbin >= spectrumsize - 1) stopbin = spectrumsize - 1;
    
    for(n=startbin; n<stopbin; n++)
    {
        if(spectrumbuf[n] >= spectrumbuf[n-1])
        {
            if(spectrumbuf[n] > spectrumbuf[n+1])
            {
                if(spectrumbuf[n] > max)
                {
                    max = spectrumbuf[n];
                    peakbin = n;
                }
            }
        }
    }
    
    // calculate amplitudes in peak region
    for(n=(peakbin-1); n<(peakbin+2); n++)
    {
        spectrumbuf[n] = sqrt(spectrumbuf[n]);
    }
    
    peaklocation = (float)peakbin + interpolate3phase(spectrumbuf, peakbin);
    periodlength = (float)(s->framesize * 2.0f) / peaklocation;
    
    return periodlength;
}


// modified logarithmic bias function
static void snac_biasbuf(tSNAC *s)
{
    int n;
    int maxperiod = (int)(s->framesize * (float)SEEK);
    float bias = s->biasfactor / log((float)(maxperiod - 4));
    float *biasbuf = s->biasbuf;
    
    for(n=0; n<5; n++)    // periods < 5 samples can't be tracked
    {
        biasbuf[n] = 0.;
    }
    
    for(n=5; n<maxperiod; n++)
    {
        biasbuf[n] = 1.0f - (float)log(n - 4) * bias;
    }
}
#endif // N_SNAC


#if N_ATKDTK
/********Private function prototypes**********/
static void atkdtk_init(tAtkDtk *a, int blocksize, int atk, int rel);
static void atkdtk_envelope(tAtkDtk *a, float *in);

/********Constructor/Destructor***************/

tAtkDtk* tAtkDtk_init(int blocksize)
{
    tAtkDtk *a = &oops.tAtkDtkRegistry[oops.registryIndex[T_ATKDTK]++];
    
    atkdtk_init(a, blocksize, DEFATTACK, DEFRELEASE);
    return a;
    
}

tAtkDtk* tAtkDtk_init_expanded(int blocksize, int atk, int rel)
{
    tAtkDtk *a = &oops.tAtkDtkRegistry[oops.registryIndex[T_ATKDTK]++];
    
    atkdtk_init(a, blocksize, atk, rel);
    return (a);
    
}

void tAtkDtk_free(tAtkDtk *a)
{
    free(a);
}

/*******Public Functions***********/


void tAtkDtk_setBlocksize(tAtkDtk *a, int size)
{
    
    if(!((size==64)|(size==128)|(size==256)|(size==512)|(size==1024)|(size==2048)))
        size = DEFBLOCKSIZE;
    a->blocksize = size;
    
    return;
    
}

void tAtkDtk_setSamplerate(tAtkDtk *a, int inRate)
{
    a->samplerate = inRate;
    
    //Reset atk and rel to recalculate coeff
    tAtkDtk_setAtk(a, a->atk);
    tAtkDtk_setRel(a, a->rel);
    
    return;
}

void tAtkDtk_setThreshold(tAtkDtk *a, float thres)
{
    a->threshold = thres;
    return;
}

void tAtkDtk_setAtk(tAtkDtk *a, int inAtk)
{
    a->atk = inAtk;
    a->atk_coeff = pow(0.01, 1.0/(a->atk * a->samplerate * 0.001));
    
    return;
}

void tAtkDtk_setRel(tAtkDtk *a, int inRel)
{
    a->rel = inRel;
    a->rel_coeff = pow(0.01, 1.0/(a->rel * a->samplerate * 0.001));
    
    return;
}


int tAtkDtk_detect(tAtkDtk *a, float *in)
{
    int result;
    
    atkdtk_envelope(a, in);
    
    if(a->env >= a->prevAmp*2) //2 times greater = 6dB increase
        result = 1;
    else
        result = 0;
    
    a->prevAmp = a->env;
    
    return result;
    
}

/*******Private Functions**********/

static void atkdtk_init(tAtkDtk *a, int blocksize, int atk, int rel)
{
    a->env = 0;
    a->blocksize = blocksize;
    a->threshold = DEFTHRESHOLD;
    a->samplerate = DEFSAMPLERATE;
    a->prevAmp = 0;
    
    a->env = 0;
    
    tAtkDtk_setAtk(a, atk);
    tAtkDtk_setRel(a, rel);
}

static void atkdtk_envelope(tAtkDtk *a, float *in)
{
    int i = 0;
    float tmp;
    for(i = 0; i < a->blocksize; ++i){
        tmp = fastabs(in[i]);
        
        if(tmp > a->env)
            a->env = a->atk_coeff * (a->env - tmp) + tmp;
        else
            a->env = a->rel_coeff * (a->env - tmp) + tmp;
    }
    
}
#endif //N_ATKDTK

#if N_LOCKHARTWAVEFOLDER

tLockhartWavefolder* tLockhartWavefolderInit(void)
{
    tLockhartWavefolder *w = &oops.tLockhartWavefolderRegistry[oops.registryIndex[T_LOCKHARTWAVEFOLDER]++];
    
    w->Ln1 = 0.0;
    w->Fn1 = 0.0;
    w->xn1 = 0.0;
    
    return w;
}

double tLockhartWavefolderLambert(double x, double ln)
{
    double w = ln;
    double expw, p, r, s, err, q;
    
    for (int i = 0; i < 1000; i++)
    {
        //err = x * (logf(x) - logf(w));
        
        /*
        r = logf(x / w) - w;
        q = 2.0f * (1.0f+w) * (1.0f + w + 0.666666666f*r);
        err = (r / (1.0f+w)) * ((q - r) / (q - 2*r));
        
        
        */
        expw = exp(w);
        
        p = (w * expw) - x;
        r = (w + 1.0) * expw;
        s = (w + 2.0) / (2.0 * (w + 1.0));
        err = (p/(r-(p*s)));
        
        if (fabs(err) < THRESH) break;
        
        w -= err;
        //w = w * (1 + err);
        //w = w - err;
    }
    
    return w;
}

float tLockhartWavefolderTick(tLockhartWavefolder* const w, float samp)
{
    int l;
    double u, Ln, Fn, xn;
    float o;
    samp=(double)samp;
    // Compute Antiderivative
    if (samp > 0) l = 1;
    else if (samp < 0) l = -1;
    else l = 0;
    
    u = LOCKHART_D * exp(l*LOCKHART_B*samp);
    Ln = tLockhartWavefolderLambert(u, w->Ln1);
    Fn = (0.5 * VT_DIV_B) * (Ln*(Ln + 2.0)) - 0.5*LOCKHART_A*samp*samp;
    
    // Check for ill-conditioning
    if (fabs(samp - w->xn1) < ILL_THRESH)
    {
        // Compute Averaged Wavefolder Output
        xn = 0.5*(samp + w->xn1);
        u = LOCKHART_D*exp(l*LOCKHART_B*xn);
        Ln = tLockhartWavefolderLambert(u, w->Ln1);
        o = (float)(l*LOCKHART_VT*Ln) - (LOCKHART_A*xn);
    }
    else
    {
        // Apply AA Form
        o = (float)(Fn - w->Fn1)/(samp - w->xn1);
    }
    
    // Update States
    w->Ln1 = Ln;
    w->Fn1 = Fn;
    w->xn1 = samp;
    
    return o;
}

#endif
