/*
  ==============================================================================

    OOPSFilter.c
    Created: 20 Jan 2017 12:01:10pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#if _WIN32 || _WIN64

#include "..\Inc\OOPSFilter.h"
#include "..\Inc\OOPSWavetables.h"
#include "..\Inc\OOPS.h"

#else

#include "../Inc/OOPSFilter.h"
#include "../Inc/OOPSWavetables.h"
#include "../Inc/OOPS.h"

#endif

void tButterworth_init(tButterworth* const f, int N, float f1, float f2)
{
	f->f1 = f1;
	f->f2 = f2;
	f->gain = 1.0f;
    
    f->N = N;
    
    if (f->N > NUM_SVF_BW) f->N = NUM_SVF_BW;

	for(int i = 0; i < N/2; ++i)
	{
        tSVF_init(&f->low[i], SVFTypeHighpass, f1, 0.5f/cosf((1.0f+2.0f*i)*PI/(2*N)));
        tSVF_init(&f->high[i], SVFTypeLowpass, f2, 0.5f/cosf((1.0f+2.0f*i)*PI/(2*N)));
	}
}

void tButterworth_free(tButterworth* const f)
{
    for(int i = 0; i < f->N/2; ++i)
    {
        tSVF_free(&f->low[i]);
        tSVF_free(&f->high[i]);
    }
    
    oops_free(f);
}

float tButterworth_tick(tButterworth* const f, float samp)
{
	for(int i = 0; i < ((f->N)/2); ++i)
	{
		samp = tSVF_tick(&f->low[i],samp);
		samp = tSVF_tick(&f->high[i],samp);
	}
	return samp;
}

void tButterworth_setF1(tButterworth* const f, float f1)
{
	f->f1 = f1;
	for(int i = 0; i < ((f->N)/2); ++i)		tSVF_setFreq(&f->low[i], f1);
}

void tButterworth_setF2(tButterworth* const f, float f2)
{
	f->f2 = f2;
	for(int i = 0; i < ((f->N)/2); ++i)		tSVF_setFreq(&f->high[i], f2);
}

void tButterworth_setFreqs(tButterworth* const f, float f1, float f2)
{
	f->f1 = f1;
	f->f2 = f2;
	for(int i = 0; i < ((f->N)/2); ++i)
	{
		tSVF_setFreq(&f->low[i], f1);
		tSVF_setFreq(&f->high[i], f2);
	}
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ OneZero Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tOneZero_init(tOneZero* const f, float theZero)
{
    f->gain = 1.0f;
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
    tOneZero_setZero(f, theZero);
}

void    tOneZero_free(tOneZero* const f)
{
    oops_free(f);
}

float   tOneZero_tick(tOneZero* const f, float input)
{
    float in = input * f->gain;
    float out = f->b1 * f->lastIn + f->b0 * in;
    
    f->lastIn = in;
    
    return out;
}

void    tOneZero_setZero(tOneZero* const f, float theZero)
{
    if (theZero > 0.0f) f->b0 = 1.0f / (1.0f + theZero);
    else                f->b0 = 1.0f / (1.0f - theZero);
    
    f->b1 = -theZero * f->b0;
    
}

void    tOneZero_setB0(tOneZero* const f, float b0)
{
    f->b0 = b0;
}

void    tOneZero_setB1(tOneZero* const f, float b1)
{
    f->b1 = b1;
}

void    tOneZero_setCoefficients(tOneZero* const f, float b0, float b1)
{
    f->b0 = b0;
    f->b1 = b1;
}

void    tOneZero_setGain(tOneZero *f, float gain)
{
    f->gain = gain;
}

float   tOneZero_getPhaseDelay(tOneZero* const f, float frequency )
{
    if ( frequency <= 0.0f) frequency = 0.05f;
    
    f->frequency = frequency;
    
    float omegaT = 2 * PI * frequency * oops.invSampleRate;
    float real = 0.0, imag = 0.0;
    
    real += f->b0;
    
    real += f->b1 * cosf(omegaT);
    imag -= f->b1 * sinf(omegaT);
    
    real *= f->gain;
    imag *= f->gain;
    
    float phase = atan2f( imag, real );
    
    real = 0.0; imag = 0.0;
    
    phase -= atan2f( imag, real );
    
    phase = fmodf( -phase, 2 * PI );
    
    return phase / omegaT;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TwoZero Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tTwoZero_init(tTwoZero* const f)
{
    f->gain = 1.0f;
    f->lastIn[0] = 0.0f;
    f->lastIn[1] = 0.0f;
}

void    tTwoZero_free(tTwoZero* const f)
{
    oops_free(f);
}

float   tTwoZero_tick(tTwoZero* const f, float input)
{
    float in = input * f->gain;
    float out = f->b2 * f->lastIn[1] + f->b1 * f->lastIn[0] + f->b0 * in;
    
    f->lastIn[1] = f->lastIn[0];
    f->lastIn[0] = in;
    
    return out;
}

void    tTwoZero_setNotch(tTwoZero* const f, float freq, float radius)
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

void    tTwoZero_setB0(tTwoZero* const f, float b0)
{
    f->b0 = b0;
}

void    tTwoZero_setB1(tTwoZero* const f, float b1)
{
    f->b1 = b1;
}

void    tTwoZero_setCoefficients(tTwoZero* const f, float b0, float b1, float b2)
{
    f->b0 = b0;
    f->b1 = b1;
    f->b2 = b2;
}

void    tTwoZero_setGain(tTwoZero* const f, float gain)
{
    f->gain = gain;
}

void tTwoZeroSampleRateChanged(tTwoZero* const f)
{
    tTwoZero_setNotch(f, f->frequency, f->radius);
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ OnePole Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tOnePole_init(tOnePole* const f, float thePole)
{
    f->gain = 1.0f;
    f->a0 = 1.0;
    
    tOnePole_setPole(f, thePole);
    
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
}

void    tOnePole_init(tOnePole* const f)
{
    oops_free(f);
}

void    tOnePole_setB0(tOnePole* const f, float b0)
{
    f->b0 = b0;
}

void    tOnePole_setA1(tOnePole* const f, float a1)
{
    if (a1 >= 1.0f)     a1 = 0.999999f;
    
    f->a1 = a1;
}

void    tOnePole_setPole(tOnePole* const f, float thePole)
{
    if (thePole >= 1.0f)    thePole = 0.999999f;
    
    // Normalize coefficients for peak unity gain.
    if (thePole > 0.0f)     f->b0 = (1.0f - thePole);
    else                    f->b0 = (1.0f + thePole);
    
    f->a1 = -thePole;
}

void    tOnePole_setCoefficients(tOnePole* const f, float b0, float a1)
{
    if (a1 >= 1.0f)     a1 = 0.999999f;
    
    f->b0 = b0;
    f->a1 = a1;
}

void    tOnePole_setGain(tOnePole* const f, float gain)
{
    f->gain = gain;
}

float   tOnePole_tick(tOnePole* const f, float input)
{
    float in = input * f->gain;
    float out = (f->b0 * in) - (f->a1 * f->lastOut);
    
    f->lastIn = in;
    f->lastOut = out;
    
    return out;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TwoPole Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tTwoPole_init(tTwoPole* const f)
{
    f->gain = 1.0f;
    f->a0 = 1.0;
    f->b0 = 1.0;
    
    f->lastOut[0] = 0.0f;
    f->lastOut[1] = 0.0f;
}

void    tTwoPole_free(tTwoPole* const f)
{
    oops_free(f);
}

float   tTwoPole_tick(tTwoPole* const f, float input)
{
    float in = input * f->gain;
    float out = (f->b0 * in) - (f->a1 * f->lastOut[0]) - (f->a2 * f->lastOut[1]);
    
    f->lastOut[1] = f->lastOut[0];
    f->lastOut[0] = out;
    
    return out;
}

void    tTwoPole_setB0(tTwoPole* const f, float b0)
{
    f->b0 = b0;
}

void    tTwoPole_setA1(tTwoPole* const f, float a1)
{
    f->a1 = a1;
}

void    tTwoPole_setA2(tTwoPole* const f, float a2)
{
    f->a2 = a2;
}


void    tTwoPole_setResonance(tTwoPole* const f, float frequency, float radius, oBool normalize)
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

void    tTwoPole_setCoefficients(tTwoPole* const f, float b0, float a1, float a2)
{
    f->b0 = b0;
    f->a1 = a1;
    f->a2 = a2;
}

void    tTwoPole_setGain(tTwoPole* const f, float gain)
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

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ PoleZero Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void   tPoleZero_init(tPoleZero* const f)
{
    f->gain = 1.0f;
    f->b0 = 1.0;
    f->a0 = 1.0;
    
    f->lastIn = 0.0f;
    f->lastOut = 0.0f;
}

void   tPoleZero_free(tPoleZero* const f)
{
    oops_free(f);
}

void    tPoleZero_setB0(tPoleZero* const pzf, float b0)
{
    pzf->b0 = b0;
}

void    tPoleZero_setB1(tPoleZero* const pzf, float b1)
{
    pzf->b1 = b1;
}

void    tPoleZero_setA1(tPoleZero* const pzf, float a1)
{
    if (a1 >= 1.0f) // a1 should be less than 1.0
    {
        a1 = 0.999999f;
    }
    
    pzf->a1 = a1;
}

void    tPoleZero_setCoefficients(tPoleZero* const pzf, float b0, float b1, float a1)
{
    if (a1 >= 1.0f) // a1 should be less than 1.0
    {
        a1 = 0.999999f;
    }
    
    pzf->b0 = b0;
    pzf->b1 = b1;
    pzf->a1 = a1;
}

void    tPoleZero_setAllpass(tPoleZero* const pzf, float coeff)
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

void    tPoleZero_setBlockZero(tPoleZero* const pzf, float thePole)
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

void    tPoleZero_setGain(tPoleZero* const pzf, float gain)
{
    pzf->gain = gain;
}

float   tPoleZero_tick(tPoleZero* const pzf, float input)
{
    float in = input * pzf->gain;
    float out = (pzf->b0 * in) + (pzf->b1 * pzf->lastIn) - (pzf->a1 * pzf->lastOut);
    
    pzf->lastIn = in;
    pzf->lastOut = out;
    
    return out;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ BiQuad Filter ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void    tBiQuad_init(tBiQuad* const f)
{
    f->gain = 1.0f;
    
    f->b0 = 0.0f;
    f->a0 = 0.0f;
    
    f->lastIn[0] = 0.0f;
    f->lastIn[1] = 0.0f;
    f->lastOut[0] = 0.0f;
    f->lastOut[1] = 0.0f;
}

void    tBiQuad_free(tBiQuad* const f)
{
    oops_free(f);
}

float   tBiQuad_tick(tBiQuad* const f, float input)
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

void    tBiQuad_setResonance(tBiQuad* const f, float freq, float radius, oBool normalize)
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

void    tBiQuad_setNotch(tBiQuad* const f, float freq, float radius)
{
    // Should also deal with frequency being > half sample rate / nyquist. See STK
    if (freq < 0.0f)    freq = 0.0f;
    if (radius < 0.0f)  radius = 0.0f;
    
    f->b2 = radius * radius;
    f->b1 = -2.0f * radius * cosf(TWO_PI * freq * oops.invSampleRate); // OPTIMIZE with LOOKUP or APPROXIMATION
    
    // Does not attempt to normalize filter gain.
}

void tBiQuad_setEqualGainZeros(tBiQuad* const f)
{
    f->b0 = 1.0f;
    f->b1 = 0.0f;
    f->b2 = -1.0f;
}

void    tBiQuad_setB0(tBiQuad* const f, float b0)
{
    f->b0 = b0;
}

void    tBiQuad_setB1(tBiQuad* const f, float b1)
{
    f->b1 = b1;
}

void    tBiQuad_setB2(tBiQuad* const f, float b2)
{
    f->b2 = b2;
}

void    tBiQuad_setA1(tBiQuad* const f, float a1)
{
    f->a1 = a1;
}

void    tBiQuad_setA2(tBiQuad* const f, float a2)
{
    f->a2 = a2;
}

void    tBiQuad_setCoefficients(tBiQuad* const f, float b0, float b1, float b2, float a1, float a2)
{
    f->b0 = b0;
    f->b1 = b1;
    f->b2 = b2;
    f->a1 = a1;
    f->a2 = a2;
}

void    tBiQuad_setGain(tBiQuad* const f, float gain)
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

/* Highpass */
void     tHighpass_setFreq(tHighpass* const f, float freq)
{
    f->frequency = freq;
    f->R = (1.0f-((freq * 2.0f * 3.14f) * oops.invSampleRate));
    
}

float     tHighpass_getFreq(tHighpass* const f)
{
    return f->frequency;
}

// From JOS DC Blocker
float   tHighpass_tick(tHighpass* const f, float x)
{
    f->ys = x - f->xs + f->R * f->ys;
    f->xs = x;
    return f->ys;
}

void    tHighpass_init(tHighpass* const f, float freq)
{
    f->R = (1.0f-((freq * 2.0f * 3.14f)* oops.invSampleRate));
    f->ys = 0.0f;
    f->xs = 0.0f;
    
    f->frequency = freq;
}

void    tHighpass_free(tHighpass* const f)
{
    oops_free(f);
}

void tHighpassSampleRateChanged(tHighpass* const f)
{
    f->R = (1.0f-((f->frequency * 2.0f * 3.14f) * oops.invSampleRate));
}

float   tSVF_tick(tSVF* const svf, float v0)
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
void tSVF_init(tSVF* const svf, SVFType type, float freq, float Q)
{
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
}

void tSVF_free(tSVF* const svf)
{
    oops_free(svf);
}

int     tSVF_setFreq(tSVF* const svf, float freq)
{
    svf->g = tanf(PI * freq * oops.invSampleRate);
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
    
    return 0;
}

int     tSVF_setQ(tSVF* const svf, float Q)
{
    svf->k = 1.0f/OOPS_clip(0.01f,Q,10.0f);
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
    
    return 0;
}

// Efficient version of tSVF where frequency is set based on 12-bit integer input for lookup in tanh wavetable.
void   tSVFE_init(tSVFE* const svf, SVFType type, uint16_t input, float Q)
{
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
}

float   tSVFE_tick(tSVFE* const svf, float v0)
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

int     tSVFE_setFreq(tSVFE* const svf, uint16_t input)
{
    svf->g = filtertan[input];
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
    
    return 0;
}

int     tSVFE_setQ(tSVFE* const svf, float Q)
{
    svf->k = 1.0f/Q;
    svf->a1 = 1.0f/(1.0f + svf->g * (svf->g + svf->k));
    svf->a2 = svf->g * svf->a1;
    svf->a3 = svf->g * svf->a2;
    
    return 0;
}

void tFormantShifter_init(tFormantShifter* const fs)
{
    fs->ford = FORD;
    fs->falph = powf(0.001f, 80.0f / (oops.sampleRate));
    fs->flamb = -(0.8517f*sqrt(atanf(0.06583f*oops.sampleRate))-0.1916f);
    fs->fhp = 0.0f;
    fs->flp = 0.0f;
    fs->flpa = powf(0.001f, 10.0f / (oops.sampleRate));
    fs->fmute = 1.0f;
    fs->fmutealph = powf(0.001f, 1.0f / (oops.sampleRate));
    fs->cbi = 0;
}

void tFormantShifter_free(tFormantShifter* const fs)
{
    oops_free(fs);
}


float tFormantShifter_tick(tFormantShifter* fs, float in, float fwarp)
{
	return tFormantShifter_add(fs, tFormantShifter_remove(fs, in), fwarp);
}

float tFormantShifter_remove(tFormantShifter* fs, float in)
{
	float fa, fb, fc, foma, falph, ford, flpa, flamb, tf, fk, tf2, f0resp, f1resp, frlamb;
	int outindex = 0;
	int ti4;
	ford = fs->ford;
	falph = fs->falph;
	foma = (1.0f - falph);
	flpa = fs->flpa;
	flamb = fs->flamb;

	tf = in;
	ti4 = fs->cbi;

	fa = tf - fs->fhp;
	fs->fhp = tf;
	fb = fa;
	for(int i = 0; i < ford; i++)
	{
		fs->fsig[i] = fa*fa*foma + fs->fsig[i]*falph;
		fc = (fb - fs->fc[i])*flamb + fs->fb[i];
		fs->fc[i] = fc;
		fs->fb[i] = fb;
		fk = fa*fc*foma + fs->fk[i]*falph;
		fs->fk[i] = fk;
		tf = fk/(fs->fsig[i] + 0.000001f);
		tf = tf*foma + fs->fsmooth[i]*falph;
		fs->fsmooth[i] = tf;
		fs->fbuff[i][ti4] = tf;
		fb = fc - tf*fa;
		fa = fa - tf*fc;
	}
	fs->cbi++;
	if(fs->cbi >= FORMANT_BUFFER_SIZE)
	{
		fs->cbi = 0;
	}

	return fa;
}

float tFormantShifter_add(tFormantShifter* fs, float in, float fwarp)
{
	float fa, fb, fc, foma, falph, ford, flpa, flamb, tf, fk, tf2, f0resp, f1resp, frlamb;
	int outindex = 0;
	int ti4;
	ford = fs->ford;
	falph = fs->falph;
	foma = (1.0f - falph);
	flpa = fs->flpa;
	flamb = fs->flamb;
	tf = exp2(fwarp/2.0f) * (1+flamb)/(1-flamb);
	frlamb = (tf-1)/(tf+1);
	ti4 = fs->cbi;

	tf2 = in;
	fa = 0;
	fb = fa;
	for (int i=0; i<ford; i++)
	{
		fc = (fb-fs->frc[i])*frlamb + fs->frb[i];
		tf = fs->fbuff[i][ti4];
		fb = fc - tf*fa;
		fs->ftvec[i] = tf*fc;
		fa = fa - fs->ftvec[i];
	}
	tf = -fa;
	for (int i=ford-1; i>=0; i--)
	{
		tf = tf + fs->ftvec[i];
	}
	f0resp = tf;

	//  second time: compute 1-response
	fa = 1;
	fb = fa;
	for (int i=0; i<ford; i++)
	{
		fc = (fb-fs->frc[i])*frlamb + fs->frb[i];
		tf = fs->fbuff[i][ti4];
		fb = fc - tf*fa;
		fs->ftvec[i] = tf*fc;
		fa = fa - fs->ftvec[i];
	}
	tf = -fa;
	for (int i=ford-1; i>=0; i--)
	{
		tf = tf + fs->ftvec[i];
	}
	f1resp = tf;

	//  now solve equations for output, based on 0-response and 1-response
	tf = (float)2*tf2;
	tf2 = tf;
	tf = ((float)1 - f1resp + f0resp);
	if (tf!=0)
	{
		tf2 = (tf2 + f0resp) / tf;
	}
	else
	{
		tf2 = 0;
	}

	//  third time: update delay registers
	fa = tf2;
	fb = fa;
	for (int i=0; i<ford; i++)
	{
		fc = (fb-fs->frc[i])*frlamb + fs->frb[i];
		fs->frc[i] = fc;
		fs->frb[i] = fb;
		tf = fs->fbuff[i][ti4];
		fb = fc - tf*fa;
		fa = fa - tf*fc;
	}
	tf = tf2;
	tf = tf + flpa * fs->flp;  // lowpass post-emphasis filter
	fs->flp = tf;

	// Bring up the gain slowly when formant correction goes from disabled
	// to enabled, while things stabilize.
	if (fs->fmute>0.5)
	{
		tf = tf*(fs->fmute - 0.5)*2;
	}
	else
	{
		tf = 0;
	}
	tf2 = fs->fmutealph;
	fs->fmute = (1-tf2) + tf2*fs->fmute;
	// now tf is signal output
	// ...and we're done messing with formants

	return tf;
}


static int pitchshifter_attackdetect(tPitchShifter* ps);

void tPitchShifter_init(tPitchShifter* const ps, float* in, float* out, int bufSize, int frameSize)
{
    ps->inBuffer = in;
    ps->outBuffer = out;
    ps->bufSize = bufSize;
    ps->frameSize = frameSize;
    ps->framesPerBuffer = ps->bufSize / ps->frameSize;
    ps->curBlock = 1;
    ps->lastBlock = 0;
    ps->index = 0;
    
    ps->hopSize = DEFHOPSIZE;
    ps->windowSize = DEFWINDOWSIZE;
    ps->fba = FBA;
    
    ps->env = (tEnv*) oops_alloc(sizeof(tEnv));
    tEnv_init(ps->env, ps->windowSize, ps->hopSize, ps->frameSize);
    
    ps->snac = (tSNAC*) oops_alloc(sizeof(tSNAC));
    tSNAC_init(ps->snac, DEFOVERLAP);
    
    ps->sola = (tSOLAD*) oops_alloc(sizeof(tSOLAD));
    tSOLAD_init(ps->sola);
    
    ps->hp = (tHighpass*) oops_alloc(sizeof(tHighpass));
    tHighpass_init(ps->hp, HPFREQ);
    
    tSOLAD_setPitchFactor(ps->sola, DEFPITCHRATIO);
    
    tPitchShifter_setTimeConstant(ps, DEFTIMECONSTANT);
}

void tPitchShifter_free(tPitchShifter* const ps)
{
    tEnv_free(ps->env);
    tSNAC_free(ps->snac);
    tSOLAD_free(ps->sola);
    tHighpass_free(ps->hp);
    
    oops_free(ps);
}

float tPitchShifter_tick(tPitchShifter* ps, float sample)
{
    float period, out;
    int i, iLast;

    i = (ps->curBlock*ps->frameSize);
    iLast = (ps->lastBlock*ps->frameSize)+ps->index;

    out = tHighpass_tick(ps->hp, ps->outBuffer[iLast]);
    ps->inBuffer[i+ps->index] = sample;

    ps->index++;
    if (ps->index >= ps->frameSize)
    {
        ps->index = 0;

        tEnv_processBlock(ps->env, &(ps->inBuffer[i]));

        if(pitchshifter_attackdetect(ps) == 1)
        {
            ps->fba = 5;
            tSOLAD_setReadLag(ps->sola, ps->windowSize);
        }

        tSNAC_ioSamples(ps->snac, &(ps->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
        period = tSNAC_getPeriod(ps->snac);

        ps->curBlock++;
		if (ps->curBlock >= ps->framesPerBuffer) ps->curBlock = 0;
		ps->lastBlock++;
		if (ps->lastBlock >= ps->framesPerBuffer) ps->lastBlock = 0;

		//separate here

        tSOLAD_setPeriod(ps->sola, period);

        tSOLAD_setPitchFactor(ps->sola, ps->pitchFactor);
        tSOLAD_ioSamples(ps->sola, &(ps->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);


    }

    return out;
}

float tPitchShifterToFreq_tick(tPitchShifter* ps, float sample, float freq)
{
    float period, out;
    int i, iLast;
    
    i = (ps->curBlock*ps->frameSize);
    iLast = (ps->lastBlock*ps->frameSize)+ps->index;
    
    out = tHighpass_tick(ps->hp, ps->outBuffer[iLast]);
    ps->inBuffer[i+ps->index] = sample;
    
    ps->index++;
    if (ps->index >= ps->frameSize)
    {
        ps->index = 0;
        
        tEnv_processBlock(ps->env, &(ps->inBuffer[i]));
        
        if(pitchshifter_attackdetect(ps) == 1)
        {
            ps->fba = 5;
            tSOLAD_setReadLag(ps->sola, ps->windowSize);
        }
        
        tSNAC_ioSamples(ps->snac, &(ps->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
        period = tSNAC_getPeriod(ps->snac);
        
        tSOLAD_setPeriod(ps->sola, period);
        
        ps->pitchFactor = period*freq*oops.invSampleRate;
        tSOLAD_setPitchFactor(ps->sola, ps->pitchFactor);
        tSOLAD_ioSamples(ps->sola, &(ps->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
        
        ps->curBlock++;
        if (ps->curBlock >= ps->framesPerBuffer) ps->curBlock = 0;
        ps->lastBlock++;
        if (ps->lastBlock >= ps->framesPerBuffer) ps->lastBlock = 0;
    }

    return out;
}

float tPitchShifterToFunc_tick(tPitchShifter* ps, float sample, float (*fun)(float))
{
    float period, out;
    int i, iLast;

    i = (ps->curBlock*ps->frameSize);
    iLast = (ps->lastBlock*ps->frameSize)+ps->index;

    out = tHighpass_tick(ps->hp, ps->outBuffer[iLast]);
    ps->inBuffer[i+ps->index] = sample;

    ps->index++;
    if (ps->index >= ps->frameSize)
    {
        ps->index = 0;

        tEnv_processBlock(ps->env, &(ps->inBuffer[i]));

        if(pitchshifter_attackdetect(ps) == 1)
        {
            ps->fba = 5;
            tSOLAD_setReadLag(ps->sola, ps->windowSize);
        }

        tSNAC_ioSamples(ps->snac, &(ps->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
        period = tSNAC_getPeriod(ps->snac);

        tSOLAD_setPeriod(ps->sola, period);

        ps->pitchFactor = period/fun(period);
        tSOLAD_setPitchFactor(ps->sola, ps->pitchFactor);
        tSOLAD_ioSamples(ps->sola, &(ps->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);

        ps->curBlock++;
        if (ps->curBlock >= ps->framesPerBuffer) ps->curBlock = 0;
        ps->lastBlock++;
        if (ps->lastBlock >= ps->framesPerBuffer) ps->lastBlock = 0;
    }

    return out;
}

void tPitchShifter_ioSamples(tPitchShifter* ps, float* in, float* out, int size)
{
    float period;
    
    tEnv_processBlock(ps->env, in);
    
    if(pitchshifter_attackdetect(ps) == 1)
    {
        ps->fba = 5;
        tSOLAD_setReadLag(ps->sola, ps->windowSize);
    }
    
    tSNAC_ioSamples(ps->snac, in, out, size);
    period = tSNAC_getPeriod(ps->snac);
    
    tSOLAD_setPeriod(ps->sola, period);
    
    tSOLAD_setPitchFactor(ps->sola, ps->pitchFactor);
    tSOLAD_ioSamples(ps->sola, in, out, size);
    
    for (int cc = 0; cc < size; ++cc)
    {
        out[cc] = tHighpass_tick(ps->hp, out[cc]);
    }
}

void tPitchShifter_ioSamples_toFreq(tPitchShifter* ps, float* in, float* out, int size, float toFreq)
{
    float period;

    tEnv_processBlock(ps->env, in);

    if(pitchshifter_attackdetect(ps) == 1)
    {
        ps->fba = 5;
        tSOLAD_setReadLag(ps->sola, ps->windowSize);
    }

    tSNAC_ioSamples(ps->snac, in, out, size);
    period = tSNAC_getPeriod(ps->snac);

    tSOLAD_setPeriod(ps->sola, period);
    ps->pitchFactor = period*toFreq;
    tSOLAD_setPitchFactor(ps->sola, ps->pitchFactor);
    tSOLAD_ioSamples(ps->sola, in, out, size);

    for (int cc = 0; cc < size; ++cc)
    {
        out[cc] = tHighpass_tick(ps->hp, out[cc]);
    }
}

void tPitchShifter_ioSamples_toPeriod(tPitchShifter* ps, float* in, float* out, int size, float toPeriod)
{
    float period;

    tEnv_processBlock(ps->env, in);

    if(pitchshifter_attackdetect(ps) == 1)
    {
        ps->fba = 5;
        tSOLAD_setReadLag(ps->sola, ps->windowSize);
    }

    tSNAC_ioSamples(ps->snac, in, out, size);
    period = tSNAC_getPeriod(ps->snac);

    tSOLAD_setPeriod(ps->sola, period);
    ps->pitchFactor = period/toPeriod;
    tSOLAD_setPitchFactor(ps->sola, ps->pitchFactor);
    tSOLAD_ioSamples(ps->sola, in, out, size);

    for (int cc = 0; cc < size; ++cc)
    {
        out[cc] = tHighpass_tick(ps->hp, out[cc]);
    }
}

void tPitchShifter_ioSamples_toFunc(tPitchShifter* ps, float* in, float* out, int size, float (*fun)(float))
{
    float period;

    tEnv_processBlock(ps->env, in);

    if(pitchshifter_attackdetect(ps) == 1)
    {
        ps->fba = 5;
        tSOLAD_setReadLag(ps->sola, ps->windowSize);
    }

    tSNAC_ioSamples(ps->snac, in, out, size);
    period = tSNAC_getPeriod(ps->snac);

    tSOLAD_setPeriod(ps->sola, period);
    ps->pitchFactor = period/fun(period);
    tSOLAD_setPitchFactor(ps->sola, ps->pitchFactor);
    tSOLAD_ioSamples(ps->sola, in, out, size);

    for (int cc = 0; cc < size; ++cc)
    {
        out[cc] = tHighpass_tick(ps->hp, out[cc]);
    }
}

void tPitchShifter_setPitchFactor(tPitchShifter* ps, float pf)
{
    ps->pitchFactor = pf;
}

void tPitchShifter_setTimeConstant(tPitchShifter* ps, float tc)
{
    ps->timeConstant = tc;
    ps->radius = expf(-1000.0f * ps->hopSize * oops.invSampleRate / ps->timeConstant);
}

void tPitchShifter_setHopSize(tPitchShifter* ps, int hs)
{
    ps->hopSize = hs;
}

void tPitchShifter_setWindowSize(tPitchShifter* ps, int ws)
{
    ps->windowSize = ws;
}

float tPitchShifter_getPeriod(tPitchShifter* ps)
{
    return tSNAC_getPeriod(ps->snac);
}

static int pitchshifter_attackdetect(tPitchShifter* ps)
{
    float envout;
    
    envout = tEnv_tick(ps->env);
    
    if (envout >= 1.0f)
    {
        ps->lastmax = ps->max;
        if (envout > ps->max)
        {
            ps->max = envout;
        }
        else
        {
            ps->deltamax = envout - ps->max;
            ps->max = ps->max * ps->radius;
        }
        ps->deltamax = ps->max - ps->lastmax;
    }
    
    ps->fba = ps->fba ? (ps->fba - 1) : 0;
    
    return (ps->fba == 0 && (ps->max > 60 && ps->deltamax > 6)) ? 1 : 0;
}


void 	tPeriod_init	(tPeriod* const p, float* in, float* out, int bufSize, int frameSize)
{
	p->inBuffer = in;
	p->outBuffer = out;
	p->bufSize = bufSize;
	p->frameSize = frameSize;
	p->framesPerBuffer = p->bufSize / p->frameSize;
	p->curBlock = 1;
	p->lastBlock = 0;
	p->index = 0;

	p->hopSize = DEFHOPSIZE;
	p->windowSize = DEFWINDOWSIZE;
	p->fba = FBA;

    p->env = (tEnv*) oops_alloc(sizeof(tEnv));
	tEnv_init(p->env, p->windowSize, p->hopSize, p->frameSize);
    
    p->snac = (tSNAC*) oops_alloc(sizeof(tSNAC));
	tSNAC_init(p->snac, DEFOVERLAP);

	p->timeConstant = DEFTIMECONSTANT;
	p->radius = expf(-1000.0f * p->hopSize * oops.invSampleRate / p->timeConstant);
}

void tPeriod_free (tPeriod* const p)
{
    tEnv_free(p->env);
    tSNAC_free(p->snac);
    
    oops_free(p);
}

float tPeriod_findPeriod (tPeriod* p, float sample)
{
    float period;
    int i, iLast;

    i = (p->curBlock*p->frameSize);
    iLast = (p->lastBlock*p->frameSize)+p->index;

    p->i = i;
    p->iLast = iLast;

    p->inBuffer[i+p->index] = sample;

    p->index++;
    p->indexstore = p->index;
    if (p->index >= p->frameSize)
    {
        p->index = 0;

        tEnv_processBlock(p->env, &(p->inBuffer[i]));

        tSNAC_ioSamples(p->snac, &(p->inBuffer[i]), &(p->outBuffer[i]), p->frameSize);
        p->period = tSNAC_getPeriod(p->snac);

        p->curBlock++;
		if (p->curBlock >= p->framesPerBuffer) p->curBlock = 0;
		p->lastBlock++;
		if (p->lastBlock >= p->framesPerBuffer) p->lastBlock = 0;
    }

    // changed from period to p->period
    return p->period;
}

void tPeriod_setHopSize(tPeriod* p, int hs)
{
    p->hopSize = hs;
}

void tPeriod_setWindowSize(tPeriod* p, int ws)
{
    p->windowSize = ws;
}

void tPitchShift_setPitchFactor(tPitchShift* ps, float pf)
{
	ps->pitchFactor = pf;
}

static int pitchshift_attackdetect(tPitchShift* ps)
{
    float envout;

    envout = tEnv_tick(ps->p->env);

    if (envout >= 1.0f)
    {
        ps->p->lastmax = ps->p->max;
        if (envout > ps->p->max)
        {
            ps->p->max = envout;
        }
        else
        {
            ps->p->deltamax = envout - ps->p->max;
            ps->p->max = ps->p->max * ps->radius;
        }
        ps->p->deltamax = ps->p->max - ps->p->lastmax;
    }

    ps->p->fba = ps->p->fba ? (ps->p->fba - 1) : 0;

    return (ps->p->fba == 0 && (ps->p->max > 60 && ps->p->deltamax > 6)) ? 1 : 0;
}

void tPitchShift_init (tPitchShift* const ps, tPeriod* p, float* out, int bufSize)
{
	ps->p = p;

	ps->outBuffer = out;
	ps->bufSize = bufSize;
	ps->frameSize = p->frameSize;
	ps->framesPerBuffer = ps->bufSize / ps->frameSize;
	ps->curBlock = 1;
	ps->lastBlock = 0;
	ps->index = 0;
	ps->pitchFactor = 1.0f;

    ps->sola = (tSOLAD*) oops_alloc(sizeof(tSOLAD));
	tSOLAD_init(ps->sola);
    
    ps->hp = (tHighpass*) oops_alloc(sizeof(tHighpass));
    tHighpass_init(ps->hp, HPFREQ);

	tSOLAD_setPitchFactor(ps->sola, DEFPITCHRATIO);
}

void tPitchShift_free(tPitchShift* const ps)
{
    tSOLAD_free(ps->sola);
    tHighpass_free(ps->hp);
    
    oops_free(ps);
}

float tPitchShift_shift (tPitchShift* ps)
{
    float period, out;
    int i, iLast;

    i = ps->p->i;
    iLast = ps->p->iLast;

    out = tHighpass_tick(ps->hp, ps->outBuffer[iLast]);

    if (ps->p->indexstore >= ps->frameSize)
    {
    	period = ps->p->period;

    	if(pitchshift_attackdetect(ps) == 1)
		{
			ps->p->fba = 5;
			tSOLAD_setReadLag(ps->sola, ps->p->windowSize);
		}

        tSOLAD_setPeriod(ps->sola, period);
        tSOLAD_setPitchFactor(ps->sola, ps->pitchFactor);

        tSOLAD_ioSamples(ps->sola, &(ps->p->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
    }

    return out;
}

float tPitchShift_shiftToFreq (tPitchShift* ps, float freq)
{
    float period, out;
    int i, iLast;

    i = ps->p->i;
	iLast = ps->p->iLast;

    out = tHighpass_tick(ps->hp, ps->outBuffer[iLast]);

    if (ps->p->indexstore >= ps->frameSize)
    {
    	period = ps->p->period;

    	if(pitchshift_attackdetect(ps) == 1)
		{
			ps->p->fba = 5;
			tSOLAD_setReadLag(ps->sola, ps->p->windowSize);
		}

        tSOLAD_setPeriod(ps->sola, period);

        ps->pitchFactor = period*freq*oops.invSampleRate;
        tSOLAD_setPitchFactor(ps->sola, ps->pitchFactor);

        tSOLAD_ioSamples(ps->sola, &(ps->p->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);
    }
    return out;
}

float tPitchShift_shiftToFunc (tPitchShift* ps, float (*fun)(float))
{
    float period, out;
    int i, iLast;

    i = ps->p->i;
  	iLast = ps->p->iLast;

    out = tHighpass_tick(ps->hp, ps->outBuffer[iLast]);

    if (ps->p->indexstore >= ps->frameSize)
    {
    	period = ps->p->period;

    	if(pitchshift_attackdetect(ps) == 1)
		{
			ps->p->fba = 5;
			tSOLAD_setReadLag(ps->sola, ps->p->windowSize);
		}

        tSOLAD_setPeriod(ps->sola, period);

        ps->pitchFactor = period/fun(period);
        tSOLAD_setPitchFactor(ps->sola, ps->pitchFactor);

        tSOLAD_ioSamples(ps->sola, &(ps->p->inBuffer[i]), &(ps->outBuffer[i]), ps->frameSize);

        ps->curBlock++;
		if (ps->curBlock >= ps->p->framesPerBuffer) ps->curBlock = 0;
		ps->lastBlock++;
		if (ps->lastBlock >= ps->framesPerBuffer) ps->lastBlock = 0;
    }

    return out;
}
