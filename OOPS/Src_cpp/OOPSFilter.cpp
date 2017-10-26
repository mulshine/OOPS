/*
  ==============================================================================

    OOPSFilter.c
    Created: 20 Jan 2017 12:01:10pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../Inc/OOPSFilter.h"
#include "../Inc/OOPSWavetables.h"
#include "../Inc/OOPS.h"

#if N_ONEZERO
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ OneZero Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
tOneZero*    tOneZeroInit(float theZero)
{
    tOneZero* f = &oops.tOneZeroRegistry[oops.registryIndex[T_ONEZERO]++];
    
    f->gain = 1.0f;
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
    tOneZeroSetZero(f, theZero);
    
    return f;
}

float   tOneZeroTick(tOneZero* const f, float input)
{
    float in = input * f->gain;
    float out = f->b1 * f->lastIn + f->b0 * in;
    
    f->lastIn = in;
    
    return out;
}

void    tOneZeroSetZero(tOneZero* const f, float theZero)
{
    if (theZero > 0.0f) f->b0 = 1.0f / (1.0f + theZero);
    else                f->b0 = 1.0f / (1.0f - theZero);
    
    f->b1 = -theZero * f->b0;
    
}

void    tOneZeroSetB0(tOneZero* const f, float b0)
{
    f->b0 = b0;
}

void    tOneZeroSetB1(tOneZero* const f, float b1)
{
    f->b1 = b1;
}

void    tOneZeroSetCoefficients(tOneZero* const f, float b0, float b1)
{
    f->b0 = b0;
    f->b1 = b1;
}

void    tOneZeroSetGain(tOneZero *f, float gain)
{
    f->gain = gain;
}

float   tOneZeroGetPhaseDelay(tOneZero* const f, float frequency )
{
    if ( frequency <= 0.0) frequency = 0.05f;
    
    f->frequency = frequency;
    
    float omegaT = 2 * PI * frequency * oops.invSampleRate;
    float real = 0.0f, imag = 0.0f;
    
    real += f->b0;
    
    real += f->b1 * cosf(omegaT);
    imag -= f->b1 * sinf(omegaT);
    
    real *= f->gain;
    imag *= f->gain;
    
    float phase = atan2f( imag, real );
    
    real = 0.0f, imag = 0.0f;
    
    phase -= atan2f( imag, real );
    
    phase = fmodf( -phase, 2 * PI );
    
    return phase / omegaT;
}
#endif

#if N_TWOZERO
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TwoZero Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
tTwoZero*    tTwoZeroInit(void)
{
    tTwoZero* f = &oops.tTwoZeroRegistry[oops.registryIndex[T_TWOZERO]++];
    
    f->gain = 1.0f;
    f->lastIn[0] = 0.0f;
    f->lastIn[1] = 0.0f;
    
    return f;
    
}

float   tTwoZeroTick(tTwoZero* const f, float input)
{
    float in = input * f->gain;
    float out = f->b2 * f->lastIn[1] + f->b1 * f->lastIn[0] + f->b0 * in;
    
    f->lastIn[1] = f->lastIn[0];
    f->lastIn[0] = in;
    
    return out;
}

void    tTwoZeroSetNotch(tTwoZero* const f, float freq, float radius)
{
    // Should also deal with frequency being > half sample rate / nyquist. See STK
    if (freq < 0.0f)    freq = 0.0f;
    if (radius < 0.0f)  radius = 0.0f;
    
    f->frequency = freq;
    f->radius = radius;
    
    f->b2 = radius * radius;
    f->b1 = -2.0f * radius * cosf(TWO_PI * freq * oops.invSampleRate); // OPTIMIZE with LOOKUP or APPROXIMATION
    
    // Normalize the filter gain. From STK.
    if ( f->b1 > 0.0f ) // Maximum at z = 0.
        f->b0 = 1.0f / ( 1.0f + f->b1 + f->b2 );
    else            // Maximum at z = -1.
        f->b0 = 1.0f / ( 1.0f - f->b1 + f->b2 );
    f->b1 *= f->b0;
    f->b2 *= f->b0;
    
}

void    tTwoZeroSetB0(tTwoZero* const f, float b0)
{
    f->b0 = b0;
}

void    tTwoZeroSetB1(tTwoZero* const f, float b1)
{
    f->b1 = b1;
}

void    tTwoZeroSetCoefficients(tTwoZero* const f, float b0, float b1, float b2)
{
    f->b0 = b0;
    f->b1 = b1;
    f->b2 = b2;
}

void    tTwoZeroSetGain(tTwoZero* const f, float gain)
{
    f->gain = gain;
}

void tTwoZeroSampleRateChanged(tTwoZero* const f)
{
    tTwoZeroSetNotch(f, f->frequency, f->radius);
}
#endif

#if N_ONEPOLE
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ OnePole Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
tOnePole*    tOnePoleInit(float thePole)
{
    tOnePole* f = &oops.tOnePoleRegistry[oops.registryIndex[T_ONEPOLE]++];
    
    f->gain = 1.0f;
    f->a0 = 1.0;
    
    tOnePoleSetPole(f, thePole);
    
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
    
    return f;
    
}

void    tOnePoleSetB0(tOnePole* const f, float b0)
{
    f->b0 = b0;
}

void    tOnePoleSetA1(tOnePole* const f, float a1)
{
    if (a1 >= 1.0f)     a1 = 0.999999f;
    
    f->a1 = a1;
}

void    tOnePoleSetPole(tOnePole* const f, float thePole)
{
    if (thePole >= 1.0f)    thePole = 0.999999f;
    
    // Normalize coefficients for peak unity gain.
    if (thePole > 0.0f)     f->b0 = (1.0f - thePole);
    else                    f->b0 = (1.0f + thePole);
    
    f->a1 = -thePole;
}

void    tOnePoleSetCoefficients(tOnePole* const f, float b0, float a1)
{
    if (a1 >= 1.0f)     a1 = 0.999999f;
    
    f->b0 = b0;
    f->a1 = a1;
}

void    tOnePoleSetGain(tOnePole* const f, float gain)
{
    f->gain = gain;
}

float   tOnePoleTick(tOnePole* const f, float input)
{
    float in = input * f->gain;
    float out = (f->b0 * in) - (f->a1 * f->lastOut);
    
    f->lastIn = in;
    f->lastOut = out;
    
    return out;
}
#endif

#if N_TWOPOLE
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TwoPole Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
tTwoPole*    tTwoPoleInit(void)
{
    tTwoPole* f = &oops.tTwoPoleRegistry[oops.registryIndex[T_TWOPOLE]++];
    
    f->gain = 1.0f;
    f->a0 = 1.0f;
    f->b0 = 1.0f;
    
    f->lastOut[0] = 0.0f;
    f->lastOut[1] = 0.0f;
    
    f->sampleRateChanged = &tTwoPoleSampleRateChanged;
    
    return f;
    
}

float   tTwoPoleTick(tTwoPole* const f, float input)
{
    float in = input * f->gain;
    float out = (f->b0 * in) - (f->a1 * f->lastOut[0]) - (f->a2 * f->lastOut[1]);
    
    f->lastOut[1] = f->lastOut[0];
    f->lastOut[0] = out;
    
    return out;
}

void    tTwoPoleSetB0(tTwoPole* const f, float b0)
{
    f->b0 = b0;
}

void    tTwoPoleSetA1(tTwoPole* const f, float a1)
{
    f->a1 = a1;
}

void    tTwoPoleSetA2(tTwoPole* const f, float a2)
{
    f->a2 = a2;
}


void    tTwoPoleSetResonance(tTwoPole* const f, float frequency, float radius, oBool normalize)
{
    if (frequency < 0.0f)   frequency = 0.0f; // need to also handle when frequency > nyquist
    if (radius < 0.0f)      radius = 0.0f;
    if (radius >= 1.0f)     radius = 0.999999f;
    
    f->radius = radius;
    f->frequency = frequency;
    f->normalize = normalize;
    
    f->a2 = radius * radius;
    f->a1 =  -2.0f * radius * cos(TWO_PI * frequency * oops.invSampleRate);
    
    if ( normalize )
    {
        // Normalize the filter gain ... not terribly efficient.
        float real = 1 - radius + (f->a2 - radius) * cos(TWO_PI * 2 * frequency * oops.invSampleRate);
        float imag = (f->a2 - radius) * sin(TWO_PI * 2 * frequency * oops.invSampleRate);
        f->b0 = sqrt( pow(real, 2) + pow(imag, 2) );
    }
}

void    tTwoPoleSetCoefficients(tTwoPole* const f, float b0, float a1, float a2)
{
    f->b0 = b0;
    f->a1 = a1;
    f->a2 = a2;
}

void    tTwoPoleSetGain(tTwoPole* const f, float gain)
{
    f->gain = gain;
}

void     tTwoPoleSampleRateChanged (tTwoPole* const f)
{
    f->a2 = f->radius * f->radius;
    f->a1 =  -2.0f * f->radius * cos(TWO_PI * f->frequency * oops.invSampleRate);
    
    if ( f->normalize )
    {
        // Normalize the filter gain ... not terribly efficient.
        float real = 1 - f->radius + (f->a2 - f->radius) * cos(TWO_PI * 2 * f->frequency * oops.invSampleRate);
        float imag = (f->a2 - f->radius) * sin(TWO_PI * 2 * f->frequency * oops.invSampleRate);
        f->b0 = sqrt( pow(real, 2) + pow(imag, 2) );
    }
}
#endif

#if N_POLEZERO
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ PoleZero Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
tPoleZero*    tPoleZeroInit(void)
{
    tPoleZero* f = &oops.tPoleZeroRegistry[oops.registryIndex[T_POLEZERO]++];
    
    f->gain = 1.0f;
    f->b0 = 1.0f;
    f->a0 = 1.0f;
    
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
    
    return f;
}

void    tPoleZeroSetB0(tPoleZero* const pzf, float b0)
{
    pzf->b0 = b0;
}

void    tPoleZeroSetB1(tPoleZero* const pzf, float b1)
{
    pzf->b1 = b1;
}

void    tPoleZeroSetA1(tPoleZero* const pzf, float a1)
{
    if (a1 >= 1.0f) // a1 should be less than 1.0
    {
        a1 = 0.999999f;
    }
    
    pzf->a1 = a1;
}

void    tPoleZeroSetCoefficients(tPoleZero* const pzf, float b0, float b1, float a1)
{
    if (a1 >= 1.0f) // a1 should be less than 1.0
    {
        a1 = 0.999999f;
    }
    
    pzf->b0 = b0;
    pzf->b1 = b1;
    pzf->a1 = a1;
}

void    tPoleZeroSetAllpass(tPoleZero* const pzf, float coeff)
{
    if (coeff >= 1.0f) // allpass coefficient >= 1.0 makes filter unstable
    {
        coeff = 0.999999f;
    }
    
    pzf->b0 = coeff;
    pzf->b1 = 1.0f;
    pzf->a0 = 1.0f;
    pzf->a1 = coeff;
}

void    tPoleZeroSetBlockZero(tPoleZero* const pzf, float thePole)
{
    if (thePole >= 1.0f) // allpass coefficient >= 1.0 makes filter unstable
    {
        thePole = 0.999999f;
    }
    
    pzf->b0 = 1.0f;
    pzf->b1 = -1.0f;
    pzf->a0 = 1.0f;
    pzf->a1 = -thePole;
}

void    tPoleZeroSetGain(tPoleZero* const pzf, float gain)
{
    pzf->gain = gain;
}

float   tPoleZeroTick(tPoleZero* const pzf, float input)
{
    float in = input * pzf->gain;
    float out = (pzf->b0 * in) + (pzf->b1 * pzf->lastIn) - (pzf->a1 * pzf->lastOut);
    
    pzf->lastIn = in;
    pzf->lastOut = out;
    
    return out;
}
#endif

#if N_BIQUAD
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ BiQuad Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
tBiQuad*    tBiQuadInit(void)
{
    tBiQuad* f = &oops.tBiQuadRegistry[oops.registryIndex[T_BIQUAD]++];
    
    f->gain = 1.0f;
    
    f->b0 = 0.0f;
    f->a0 = 0.0f;
    
    f->lastIn[0] = 0.0f;
    f->lastIn[1] = 0.0f;
    f->lastOut[0] = 0.0f;
    f->lastOut[1] = 0.0f;
    f->sampleRateChanged = &tBiQuadSampleRateChanged;
    
    return f;
    
}

float   tBiQuadTick(tBiQuad* const f, float input)
{
    float in = input * f->gain;
    float out = f->b0 * in + f->b1 * f->lastIn[0] + f->b2 * f->lastIn[1];
    out -= f->a2 * f->lastOut[1] + f->a1 * f->lastOut[0];
    
    f->lastIn[1] = f->lastIn[0];
    f->lastIn[0] = in;
    
    f->lastOut[1] = f->lastOut[0];
    f->lastOut[0] = out;
    
    return out;
}

void    tBiQuadSetResonance(tBiQuad* const f, float freq, float radius, oBool normalize)
{
    // Should also deal with frequency being > half sample rate / nyquist. See STK
    if (freq < 0.0f)    freq = 0.0f;
    if (radius < 0.0f)  radius = 0.0f;
    if (radius >= 1.0f)  radius = 1.0f;
    
    f->frequency = freq;
    f->radius = radius;
    f->normalize = normalize;
    
    f->a2 = radius * radius;
    f->a1 = -2.0f * radius * cosf(TWO_PI * freq * oops.invSampleRate);
    
    if (normalize)
    {
        f->b0 = 0.5f - 0.5f * f->a2;
        f->b1 = 0.0f;
        f->b2 = -f->b0;
    }
}

void    tBiQuadSetNotch(tBiQuad* const f, float freq, float radius)
{
    // Should also deal with frequency being > half sample rate / nyquist. See STK
    if (freq < 0.0f)    freq = 0.0f;
    if (radius < 0.0f)  radius = 0.0f;
    
    f->b2 = radius * radius;
    f->b1 = -2.0f * radius * cosf(TWO_PI * freq * oops.invSampleRate); // OPTIMIZE with LOOKUP or APPROXIMATION
    
    // Does not attempt to normalize filter gain.
}

void tBiQuadSetEqualGainZeros(tBiQuad* const f)
{
    f->b0 = 1.0f;
    f->b1 = 0.0f;
    f->b2 = -1.0f;
}

void    tBiQuadSetB0(tBiQuad* const f, float b0)
{
    f->b0 = b0;
}

void    tBiQuadSetB1(tBiQuad* const f, float b1)
{
    f->b1 = b1;
}

void    tBiQuadSetB2(tBiQuad* const f, float b2)
{
    f->b2 = b2;
}

void    tBiQuadSetA1(tBiQuad* const f, float a1)
{
    f->a1 = a1;
}

void    tBiQuadSetA2(tBiQuad* const f, float a2)
{
    f->a2 = a2;
}

void    tBiQuadSetCoefficients(tBiQuad* const f, float b0, float b1, float b2, float a1, float a2)
{
    f->b0 = b0;
    f->b1 = b1;
    f->b2 = b2;
    f->a1 = a1;
    f->a2 = a2;
}

void    tBiQuadSetGain(tBiQuad* const f, float gain)
{
    f->gain = gain;
}

void    tBiQuadSampleRateChanged(tBiQuad* const f)
{
    f->a2 = f->radius * f->radius;
    f->a1 = -2.0f * f->radius * cosf(TWO_PI * f->frequency * oops.invSampleRate);
    
    if (f->normalize)
    {
        f->b0 = 0.5f - 0.5f * f->a2;
        f->b1 = 0.0f;
        f->b2 = -f->b0;
    }
}
#endif

#if N_HIGHPASS
/* Highpass */
void     tHighpassSetFreq(tHighpass* const f, float freq)
{
    f->frequency = freq;
    f->R = (1.0f-((freq * 2.0f * 3.14f) * oops.invSampleRate));
    
}

float     tHighpassGetFreq(tHighpass* const f)
{
    return f->frequency;
}

// From JOS DC Blocker
float   tHighpassTick(tHighpass* const f, float x)
{
    f->ys = x - f->xs + f->R * f->ys;
    f->xs = x;
    return f->ys;
}

tHighpass*    tHighpassInit(float freq)
{
    tHighpass* f = &oops.tHighpassRegistry[oops.registryIndex[T_HIGHPASS]++];
    
    f->R = (1.0f-((freq * 2.0f * 3.14f)* oops.invSampleRate));
    f->ys = 0.0f;
    f->xs = 0.0f;
    
    f->frequency = freq;
    f->sampleRateChanged = &tHighpassSampleRateChanged;
    
    return f;
    
}

void tHighpassSampleRateChanged(tHighpass* const f)
{
    f->R = (1.0f-((f->frequency * 2.0f * 3.14f) * oops.invSampleRate));
}
#endif 

#if N_SVF
float   tSVFTick(tSVF* const svf, float v0)
{
    float v1,v2,v3;
    v3 = v0 - svf->ic2eq;
    v1 = (svf->a1 * svf->ic1eq) + (svf->a2 * v3);
    v2 = svf->ic2eq + (svf->a2 * svf->ic1eq) + (svf->a3 * v3);
    svf->ic1eq = (2.0f * v1) - svf->ic1eq;
    svf->ic2eq = (2.0f * v2) - svf->ic2eq;
    
    if (svf->type == SVFTypeLowpass)        return v2;
    else if (svf->type == SVFTypeBandpass)  return v1;
    else if (svf->type == SVFTypeHighpass)  return v0 - (svf->k * v1) - v2;
    else if (svf->type == SVFTypeNotch)     return v0 - (svf->k * v1);
    else if (svf->type == SVFTypePeak)      return v0 - (svf->k * v1) - (2.0f * v2);
    else                                    return 0.0f;
    
}

// Less efficient, more accurate version of SVF, in which cutoff frequency is taken as floating point Hz value and tanh
// is calculated when frequency changes.
tSVF*    tSVFInit(SVFType type, float freq, float Q)
{
    tSVF* svf = &oops.tSVFRegistry[oops.registryIndex[T_SVF]++];
    
    svf->type = type;
    
    svf->ic1eq = 0;
    svf->ic2eq = 0;
    
    float a1,a2,a3,g,k;
    g = tanf(PI * freq * oops.invSampleRate);
    k = 1.0f/OOPS_clip(0.01f,Q,10.0f);
    a1 = 1.0f/(1.0f+g*(g+k));
    a2 = g*a1;
    a3 = g*a2;
    
    svf->g = g;
    svf->k = k;
    svf->a1 = a1;
    svf->a2 = a2;
    svf->a3 = a3;
    
    return svf;
    
}

int     tSVFSetFreq(tSVF* const svf, float freq)
{
    svf->g = tanf(PI * freq * oops.invSampleRate);
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
    
    return 0;
}

int     tSVFSetQ(tSVF* const svf, float Q)
{
    svf->k = 1.0f/OOPS_clip(0.01f,Q,10.0f);
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
    
    return 0;
}
#endif

#if N_SVFE
// Efficient version of tSVF where frequency is set based on 12-bit integer input for lookup in tanh wavetable.
tSVFE*    tSVFEInit(SVFType type, uint16_t input, float Q)
{
    tSVFE* svf = &oops.tSVFERegistry[oops.registryIndex[T_SVFE]++];
    
    svf->type = type;
    
    svf->ic1eq = 0;
    svf->ic2eq = 0;
    
    float a1,a2,a3,g,k;
    g = filtertan[input];
    k = 1.0f/Q;
    a1 = 1.0f/(1.0f+g*(g+k));
    a2 = g*a1;
    a3 = g*a2;
    
    svf->g = g;
    svf->k = k;
    svf->a1 = a1;
    svf->a2 = a2;
    svf->a3 = a3;
	
	  return svf;
}

float   tSVFETick(tSVFE* const svf, float v0)
{
    float v1,v2,v3;
    v3 = v0 - svf->ic2eq;
    v1 = (svf->a1 * svf->ic1eq) + (svf->a2 * v3);
    v2 = svf->ic2eq + (svf->a2 * svf->ic1eq) + (svf->a3 * v3);
    svf->ic1eq = (2.0f * v1) - svf->ic1eq;
    svf->ic2eq = (2.0f * v2) - svf->ic2eq;
    
    if (svf->type == SVFTypeLowpass)        return v2;
    else if (svf->type == SVFTypeBandpass)  return v1;
    else if (svf->type == SVFTypeHighpass)  return v0 - (svf->k * v1) - v2;
    else if (svf->type == SVFTypeNotch)     return v0 - (svf->k * v1);
    else if (svf->type == SVFTypePeak)      return v0 - (svf->k * v1) - (2.0f * v2);
    else                                    return 0.0f;
    
}

int     tSVFESetFreq(tSVFE* const svf, uint16_t input)
{
    svf->g = filtertan[input];
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
    
    return 0;
}

int     tSVFESetQ(tSVFE* const svf, float Q)
{
    svf->k = 1.0f/Q;
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
    
    return 0;
}
#endif



