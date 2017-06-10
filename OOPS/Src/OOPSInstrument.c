/*
 ==============================================================================
 
 OOPSInstrument.c
 Created: 20 Jan 2017 12:01:54pm
 Author:  Michael R Mulshine
 
 ==============================================================================
 */

#include "OOPSUtilities.h"
#include "OOPSInstrument.h"
#include "OOPS.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if N_PLUCK
/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ tPluck ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
tPluck*    tPluckInit         (float lowestFrequency,  float delayBuff[DELAY_LENGTH])
{
    tPluck* p = &oops.tPluckRegistry[oops.registryIndex[T_PLUCK]++];
    
    if ( lowestFrequency <= 0.0f )  lowestFrequency = 10.0f;
    
    p->noise = tNoiseInit(WhiteNoise);
    p->pickFilter = tOnePoleInit(0.0f);
    p->loopFilter = tOneZeroInit(0.0f);
    
    
    p->delayLine = tDelayAInit(0.0f);
    
    tPluckSetFrequency(p, 220.0f);
    
    p->sampleRateChanged = &tPluckSampleRateChanged;
    
    return p;
    
}

float   tPluckGetLastOut    (tPluck *p)
{
    return p->lastOut;
}

float   tPluckTick          (tPluck *p)
{
    return (p->lastOut = 3.0f * tDelayATick(p->delayLine, tOneZeroTick(p->loopFilter, tDelayAGetLastOut(p->delayLine) * p->loopGain ) ));
}

void    tPluckPluck         (tPluck* const p, float amplitude)
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    tOnePoleSetPole(p->pickFilter, 0.999f - (amplitude * 0.15f));
    tOnePoleSetGain(p->pickFilter, amplitude * 0.5f );
    
    // Fill delay with noise additively with current contents.
    for ( uint32_t i = 0; i < (uint32_t)tDelayAGetDelay(p->delayLine); i++ )
        tDelayATick(p->delayLine, 0.6f * tDelayAGetLastOut(p->delayLine) + tOnePoleTick(p->pickFilter, tNoiseTick(p->noise) ) );
}

// Start a note with the given frequency and amplitude.;
void    tPluckNoteOn        (tPluck* const p, float frequency, float amplitude )
{
    p->lastFreq = frequency;
    tPluckSetFrequency( p, frequency );
    tPluckPluck( p, amplitude );
}

// Stop a note with the given amplitude (speed of decay).
void    tPluckNoteOff       (tPluck* const p, float amplitude )
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->loopGain = 1.0f - amplitude;
}

// Set instrument parameters for a particular frequency.
void    tPluckSetFrequency  (tPluck* const p, float frequency )
{
    if ( frequency <= 0.0f )   frequency = 0.001f;
    
    // Delay = length - filter delay.
    float delay = ( oops.sampleRate / frequency ) - tOneZeroGetPhaseDelay(p->loopFilter, frequency );
    
    tDelayASetDelay(p->delayLine, delay );
    
    p->loopGain = 0.99f + (frequency * 0.000005f);
    
    if ( p->loopGain >= 0.999f ) p->loopGain = 0.999f;
    
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tPluckControlChange (tPluck* const p, int number, float value)
{
    return;
}

void tPluckSampleRateChanged(tPluck* const p)
{
    //tPluckSetFrequency(p, p->lastFreq);
}

#endif
/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */

#if N_STIFKARP
/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ tStifKarp ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
tStifKarp*    tStifKarpInit          (float lowestFrequency)
{
    tStifKarp* p = &oops.tStifKarpRegistry[oops.registryIndex[T_STIFKARP]++];
    
    if ( lowestFrequency <= 0.0f )  lowestFrequency = 8.0f;
    
    p->delayLine = tDelayAInit(0.0f);
    p->combDelay = tDelayLInit(0.0f);
    
    p->filter = tOneZeroInit(0.0f);
    
    p->noise = tNoiseInit(WhiteNoise);
    
    for (int i = 0; i < 4; i++)
        p->biquad[i] = tBiQuadInit();
    
    p->pluckAmplitude = 0.3f;
    p->pickupPosition = 0.4f;
    
    p->stretching = 0.9999f;
    p->baseLoopGain = 0.995f;
    p->loopGain = 0.999f;
    
    tStifKarpSetFrequency( p, 220.0f );
    
    p->sampleRateChanged = &tStifKarpSampleRateChanged;
    
    return p;
    
}

float   tStifKarpGetLastOut    (tStifKarp* const p)
{
    return p->lastOut;
}

float   tStifKarpTick          (tStifKarp* const p)
{
    float temp = tDelayAGetLastOut(p->delayLine) * p->loopGain;
    
    // Calculate allpass stretching.
    for (int i=0; i<4; i++)     temp = tBiQuadTick(p->biquad[i],temp);
    
    // Moving average filter.
    temp = tOneZeroTick(p->filter, temp);
    
    float out = tDelayATick(p->delayLine, temp);
    out = out - tDelayLTick(p->combDelay, out);
    p->lastOut = out;
    
    return p->lastOut;
}

void    tStifKarpPluck         (tStifKarp* const p, float amplitude)
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->pluckAmplitude = amplitude;
    
    for ( uint32_t i=0; i < (uint32_t)tDelayAGetDelay(p->delayLine); i++ )
    {
        // Fill delay with noise additively with current contents.
        tDelayATick(p->delayLine, (tDelayAGetLastOut(p->delayLine) * 0.6f) + 0.4f * tNoiseTick(p->noise) * p->pluckAmplitude );
        //delayLine_.tick( combDelay_.tick((delayLine_.lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude_) );
    }
}

// Start a note with the given frequency and amplitude.;
void    tStifKarpNoteOn        (tStifKarp* const p, float frequency, float amplitude )
{
    tStifKarpSetFrequency( p, frequency );
    tStifKarpPluck( p, amplitude );
}

// Stop a note with the given amplitude (speed of decay).
void    tStifKarpNoteOff       (tStifKarp* const p, float amplitude )
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->loopGain = 1.0f - amplitude;
}

// Set instrument parameters for a particular frequency.
void    tStifKarpSetFrequency  (tStifKarp* const p, float frequency )
{
    if ( frequency <= 0.0f )   frequency = 0.001f;
    
    p->lastFrequency = frequency;
    p->lastLength = oops.sampleRate / p->lastFrequency;
    float delay = p->lastLength - 0.5f;
    tDelayASetDelay(p->delayLine, delay );
    
    // MAYBE MODIFY LOOP GAINS
    p->loopGain = p->baseLoopGain + (frequency * 0.000005f);
    if (p->loopGain >= 1.0f) p->loopGain = 0.99999f;
    
    tStifKarpSetStretch(p, p->stretching);
    
    tDelayLSetDelay(p->combDelay, 0.5f * p->pickupPosition * p->lastLength );
    
}

// Set the stretch "factor" of the string (0.0 - 1.0).
void    tStifKarpSetStretch         (tStifKarp* const p, float stretch )
{
    p->stretching = stretch;
    float coefficient;
    float freq = p->lastFrequency * 2.0f;
    float dFreq = ( (0.5f * oops.sampleRate) - freq ) * 0.25f;
    float temp = 0.5f + (stretch * 0.5f);
    if ( temp > 0.9999f ) temp = 0.9999f;
    
    for ( int i=0; i<4; i++ )
    {
        coefficient = temp * temp;
        tBiQuadSetA2(p->biquad[i], coefficient);
        tBiQuadSetB0(p->biquad[i], coefficient);
        tBiQuadSetB2(p->biquad[i], 1.0f);
        
        coefficient = -2.0f * temp * cos(TWO_PI * freq / oops.sampleRate);
        tBiQuadSetA1(p->biquad[i], coefficient);
        tBiQuadSetB1(p->biquad[i], coefficient);
        
        freq += dFreq;
    }
}

// Set the pluck or "excitation" position along the string (0.0 - 1.0).
void    tStifKarpSetPickupPosition  (tStifKarp* const p, float position )
{
    if (position < 0.0f)        p->pickupPosition = 0.0f;
    else if (position <= 1.0f)  p->pickupPosition = position;
    else                        p->pickupPosition = 1.0f;
    
    tDelayLSetDelay(p->combDelay, 0.5f * p->pickupPosition * p->lastLength);
}

// Set the base loop gain.
void    tStifKarpSetBaseLoopGain    (tStifKarp* const p, float aGain )
{
    p->baseLoopGain = aGain;
    p->loopGain = p->baseLoopGain + (p->lastFrequency * 0.000005f);
    if ( p->loopGain > 0.99999f ) p->loopGain = 0.99999f;
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tStifKarpControlChange (tStifKarp* const p, SKControlType type, float value)
{
    if ( value < 0.0f )         value = 0.0f;
    else if ( value > 128.0f )   value = 128.0f;
    
    float normalizedValue = value * INV_128;
    
    if (type == SKPickPosition) // 4
        tStifKarpSetPickupPosition( p, normalizedValue );
    else if (type == SKStringDamping) // 11
        tStifKarpSetBaseLoopGain( p, 0.97f + (normalizedValue * 0.03f) );
    else if (type == SKDetune) // 1
        tStifKarpSetStretch( p, 0.91f + (0.09f * (1.0f - normalizedValue)) );
    
}

void    tStifKarpSampleRateChanged (tStifKarp* const c)
{
    tStifKarpSetFrequency(c, c->lastFrequency);
    tStifKarpSetStretch(c, c->stretching);
}

#endif //N_STIFKARP

#define USE_STICK 0

#if N_808COWBELL

void t808CowbellOn(t808Cowbell* const cowbell, float vel)
{
    tEnvelopeOn(cowbell->envGain, vel);
    
#if USE_STICK
    tEnvelopeOn(cowbell->envStick,vel);
#endif
    
}

float t808CowbellTick(t808Cowbell* const cowbell) {
    
    float sample = 0.0f;
    
    // Mix oscillators.
    sample = (cowbell->oscMix * tSquareTick(cowbell->p[0])) + ((1.0f-cowbell->oscMix) * tSquareTick(cowbell->p[1]));
    
    // Filter dive and filter.
    tSVFESetFreq(cowbell->bandpassOsc, cowbell->filterCutoff + 1000.0f * tEnvelopeTick(cowbell->envFilter));
    sample = tSVFETick(cowbell->bandpassOsc,sample);
    
    sample *= (0.9f * tEnvelopeTick(cowbell->envGain));
    
#if USE_STICK
    sample += (0.1f * tEnvelopeTick(cowbell->envStick) * tSVFETick(cowbell->bandpassStick, tNoiseTick(cowbell->stick)));
#endif
    
    sample = tHighpassTick(cowbell->highpass, sample);
    
    return sample;
}

void t808CowbellSetDecay(t808Cowbell* const cowbell, float decay)
{
    tEnvelopeSetDecay(cowbell->envGain,decay);
}

void t808CowbellSetHighpassFreq(t808Cowbell *cowbell, float freq)
{
    tHighpassSetFreq(cowbell->highpass,freq);
}

void t808CowbellSetBandpassFreq(t808Cowbell* const cowbell, float freq)
{
    cowbell->filterCutoff = freq;
}

void t808CowbellSetFreq(t808Cowbell* const cowbell, float freq)
{
    
    tSquareSetFreq(cowbell->p[0],freq);
    tSquareSetFreq(cowbell->p[1],1.48148f*freq);
}

void t808CowbellSetOscMix(t808Cowbell* const cowbell, float oscMix)
{
    cowbell->oscMix = oscMix;
}

t808Cowbell* t808CowbellInit(void) {
    
    t808Cowbell* cowbell = &oops.t808CowbellRegistry[oops.registryIndex[T_808COWBELL]++];
    
    cowbell->p[0] = tSquareInit();
    cowbell->p[1] = tSquareInit();
    
    tSquareSetFreq(cowbell->p[0], 540.0f);
    tSquareSetFreq(cowbell->p[1], 1.48148f * 540.0f);
    
    cowbell->oscMix = 0.5f;
    
    cowbell->bandpassOsc = tSVFEInit(SVFTypeBandpass, 2500, 1.0f);
    
    cowbell->bandpassStick = tSVFEInit(SVFTypeBandpass, 1800, 1.0f);
    
    cowbell->envGain = tEnvelopeInit(5.0f, 100.0f, OFALSE);
    
    cowbell->envFilter = tEnvelopeInit(5.0, 100.0f, OFALSE);
    
    cowbell->highpass = tHighpassInit(1000.0f);
    
#if USE_STICK
    cowbell->stick = tNoiseInit(NoiseTypeWhite);
    cowbell->envStick = tEnvelopeInit(5.0f, 5.0f, 0);
#endif
    
    return cowbell;
}

#endif

#if N_808HIHAT

void t808HihatOn(t808Hihat* const hihat, float vel) {
    
    tEnvelopeOn(hihat->envGain, vel);
    tEnvelopeOn(hihat->envStick, vel);
    
}

void t808HihatSetOscNoiseMix(t808Hihat* const hihat, float oscNoiseMix) {
    
    hihat->oscNoiseMix = oscNoiseMix;
    
}

float t808HihatTick(t808Hihat* const hihat) {
    
    float sample = 0.0f;
    float gainScale = 0.1666f;
    
    for (int i = 0; i < 6; i++)
    {
        sample += tSquareTick(hihat->p[i]);
    }
    
    sample *= gainScale;
    
    sample = (hihat->oscNoiseMix * sample) + ((1.0f-hihat->oscNoiseMix) * (0.8f * tNoiseTick(hihat->n)));
    
    sample = tSVFETick(hihat->bandpassOsc, sample);
    
    sample *= tEnvelopeTick(hihat->envGain);
    
    sample = 0.85f * OOPS_clip(0.0f, tHighpassTick(hihat->highpass, sample), 1.0f);
    
    sample += 0.15f * tEnvelopeTick(hihat->envStick) * tSVFETick(hihat->bandpassStick, tNoiseTick(hihat->stick));
    
    return sample;
}

void t808HihatSetDecay(t808Hihat* const hihat, float decay)
{
    tEnvelopeSetDecay(hihat->envGain,decay);
}

void t808HihatSetHighpassFreq(t808Hihat* const hihat, float freq)
{
    tHighpassSetFreq(hihat->highpass,freq);
}

void t808HihatSetOscBandpassFreq(t808Hihat* const hihat, float freq)
{
    tSVFESetFreq(hihat->bandpassOsc,freq);
}


void t808HihatSetOscFreq(t808Hihat* const hihat, float freq)
{
    tSquareSetFreq(hihat->p[0], 2.0f * freq);
    tSquareSetFreq(hihat->p[1], 3.00f * freq);
    tSquareSetFreq(hihat->p[2], 4.16f * freq);
    tSquareSetFreq(hihat->p[3], 5.43f * freq);
    tSquareSetFreq(hihat->p[4], 6.79f * freq);
    tSquareSetFreq(hihat->p[5], 8.21f * freq);
    
}

t808Hihat* t808HihatInit(void)
{
    t808Hihat* hihat = &oops.t808HihatRegistry[oops.registryIndex[T_808HIHAT]++];
    
    for (int i = 0; i < 6; i++)
    {
        hihat->p[i] = tSquareInit();
    }
    
    hihat->stick = tNoiseInit(WhiteNoise);
    hihat->n = tNoiseInit(WhiteNoise);
    
    // need to fix SVF to be generic
    hihat->bandpassStick = tSVFEInit(SVFTypeBandpass,2500.0,1.5f);
    hihat->bandpassOsc = tSVFEInit(SVFTypeBandpass,3500,0.5f);
    
    hihat->envGain = tEnvelopeInit(5.0f, 50.0f, OFALSE);
    hihat->envStick = tEnvelopeInit(5.0f, 15.0f, OFALSE);
    
    hihat->highpass = tHighpassInit(7000.0f);
    
    float freq = 40.0f;
    
    tSquareSetFreq(hihat->p[0], 2.0f * freq);
    tSquareSetFreq(hihat->p[1], 3.00f * freq);
    tSquareSetFreq(hihat->p[2], 4.16f * freq);
    tSquareSetFreq(hihat->p[3], 5.43f * freq);
    tSquareSetFreq(hihat->p[4], 6.79f * freq);
    tSquareSetFreq(hihat->p[5], 8.21f * freq);
    
    return hihat;
}
#endif

#if N_808SNARE
void t808SnareOn(t808Snare* const snare, float vel)
{
    for (int i = 0; i < 2; i++)
    {
        tEnvelopeOn(snare->toneEnvOsc[i], vel);
        tEnvelopeOn(snare->toneEnvGain[i], vel);
        tEnvelopeOn(snare->toneEnvFilter[i], vel);
    }
    
    tEnvelopeOn(snare->noiseEnvGain, vel);
    tEnvelopeOn(snare->noiseEnvFilter, vel);
}

void t808SnareSetTone1Freq(t808Snare* const snare, float freq)
{
    snare->tone1Freq = freq;
    tTriangleSetFreq(snare->tone[0], freq);
    
}

void t808SnareSetTone2Freq(t808Snare* const snare, float freq)
{
    snare->tone2Freq = freq;
    tTriangleSetFreq(snare->tone[1],freq);
}

void t808SnareSetTone1Decay(t808Snare* const snare, float decay)
{
    tEnvelopeSetDecay(snare->toneEnvGain[0],decay);
}

void t808SnareSetTone2Decay(t808Snare* const snare, float decay)
{
    tEnvelopeSetDecay(snare->toneEnvGain[1],decay);
}

void t808SnareSetNoiseDecay(t808Snare* const snare, float decay)
{
    tEnvelopeSetDecay(snare->noiseEnvGain,decay);
}

void t808SnareSetToneNoiseMix(t808Snare* const snare, float toneNoiseMix)
{
    snare->toneNoiseMix = toneNoiseMix;
}

void t808SnareSetNoiseFilterFreq(t808Snare* const snare, float noiseFilterFreq)
{
    snare->noiseFilterFreq = noiseFilterFreq;
}

void t808SnareSetNoiseFilterQ(t808Snare* const snare, float noiseFilterQ)
{
    tSVFESetQ(snare->noiseLowpass, noiseFilterQ);
}


static float tone[2];

float t808SnareTick(t808Snare* const snare)
{
    for (int i = 0; i < 2; i++)
    {
        tTriangleSetFreq(snare->tone[i], snare->tone1Freq + (50.0f * tEnvelopeTick(snare->toneEnvOsc[i])));
        tone[i] = tTriangleTick(snare->tone[i]);
        
        tSVFESetFreq(snare->toneLowpass[i], 2000 + (500 * tEnvelopeTick(snare->toneEnvFilter[i])));
        tone[i] = tSVFETick(snare->toneLowpass[i], tone[i]) * tEnvelopeTick(snare->toneEnvGain[i]);
    }
    
    float noise = tNoiseTick(snare->noiseOsc);
    tSVFESetFreq(snare->noiseLowpass, snare->noiseFilterFreq +(500 * tEnvelopeTick(snare->noiseEnvFilter)));
    noise = tSVFETick(snare->noiseLowpass, noise) * tEnvelopeTick(snare->noiseEnvGain);
    
    float sample = (snare->toneNoiseMix)*(tone[0] * snare->toneGain[0] + tone[1] * snare->toneGain[1]) + (1.0f-snare->toneNoiseMix) * (noise * snare->noiseGain);
    
    return sample;
}

t808Snare* t808SnareInit(void)
{
    t808Snare* snare = &oops.t808SnareRegistry[oops.registryIndex[T_808SNARE]++];
    
    for (int i = 0; i < 2; i++)
    {
        snare->tone[i] = tTriangleInit();
        snare->toneLowpass[i] = tSVFEInit(SVFTypeLowpass, 2000, 1.0f);
        snare->toneEnvOsc[i] = tEnvelopeInit(3.0f, 20.0f, OFALSE);
        snare->toneEnvGain[i] = tEnvelopeInit(10.0f, 200.0f, OFALSE);
        snare->toneEnvFilter[i] = tEnvelopeInit(3.0f, 200.0f, OFALSE);
        snare->toneGain[i] = 0.5f;
    }
    
    
    snare->noiseOsc = tNoiseInit(WhiteNoise);
    snare->noiseLowpass = tSVFEInit(SVFTypeLowpass, 2000, 3.0f);
    snare->noiseEnvGain = tEnvelopeInit(10.0f, 125.0f, OFALSE);
    snare->noiseEnvFilter = tEnvelopeInit(3.0f, 100.0f, OFALSE);
    snare->noiseGain = 0.25f;
    
    
    return snare;
}

#endif


