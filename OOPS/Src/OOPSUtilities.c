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

#else

#include "../Inc/OOPSUtilities.h"
#include "../Inc/OOPSWavetables.h"
#include "../Inc/OOPS.h"

#endif


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
    tRamp* ramp = &oops.tRampRegistry[oops.registryIndex[T_RAMP]++];
    
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
