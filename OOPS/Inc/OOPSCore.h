/*
  ==============================================================================

    CAudio.h
    Created: 11 Jan 2017 8:32:51pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef CAUDIO_H_INCLUDED
#define CAUDIO_H_INCLUDED

#include "OOPSMemConfig.h"

#include "OOPSMath.h"
typedef struct _tCompressor
{
    int tauAttack, tauRelease;
    float T, R, W, M; // Threshold, compression Ratio, decibel Width of knee transition, decibel Make-up gain
    
    float x_G[2], y_G[2], x_T[2], y_T[2];
	
	  oBool isActive;
    
    void (*sampleRateChanged)(struct _tCompressor *self);
    
}tCompressor;

typedef struct _tPhasor
{
    float phase;
    float inc,freq;
    
    void (*sampleRateChanged)(struct _tPhasor *self);
    
} tPhasor;


// Cycle: Sine waveform
typedef struct _tCycle
{
    // Underlying phasor
    float phase;
    float inc,freq;
    
    void (*sampleRateChanged)(struct _tCycle *self);
    
} tCycle;

// Sawtooth waveform
typedef struct _tSawtooth
{
    // Underlying phasor
    float phase;
    float inc,freq;
    
    void (*sampleRateChanged)(struct _tSawtooth *self);
    
} tSawtooth;

// Triangle waveform
typedef struct _tTriangle
{
    // Underlying phasor
    float phase;
    float inc,freq;
    
    void (*sampleRateChanged)(struct _tTriangle *self);
    
} tTriangle;

// Square waveform
typedef struct _tSquare
{
    // Underlying phasor
    float phase;
    float inc,freq;
    
    void (*sampleRateChanged)(struct _tSquare *self);
    
} tSquare;

// Noise Types
typedef enum NoiseType
{
    WhiteNoise=0,
    PinkNoise,
    NoiseTypeNil,
} NoiseType;

// Noise
typedef struct _tNoise
{
    NoiseType type;
    float pinkb0, pinkb1, pinkb2;
    float(*rand)();
    
} tNoise;

// OnePole filter
typedef struct _tOnePole
{
    float gain;
    float a0,a1;
    float b0,b1;
    
    float lastIn, lastOut;
    
} tOnePole;



// TwoPole filter
typedef struct _tTwoPole
{
    float gain;
    float a0, a1, a2;
    float b0;
    
    float radius, frequency;
    oBool normalize;
    
    float lastOut[2];
    
    void (*sampleRateChanged)(struct _tTwoPole *self);
    
} tTwoPole;

// OneZero filter
typedef struct _tOneZero
{
    float gain;
    float b0,b1;
    float lastIn, lastOut, frequency;
    
} tOneZero;

// TwoZero filter
typedef struct _tTwoZero
{
    float gain;
    float b0, b1, b2;
    
    float frequency, radius;
    
    float lastIn[2];
    
    void (*sampleRateChanged)(struct _tTwoZero *self);
    
} tTwoZero;

// PoleZero filter
typedef struct _tPoleZero
{
    float gain;
    float a0,a1;
    float b0,b1;
    
    float lastIn, lastOut;
    
} tPoleZero;

// BiQuad filter
typedef struct _tBiQuad
{
    float gain;
    float a0, a1, a2;
    float b0, b1, b2;
    
    float lastIn[2];
    float lastOut[2];
    
    float frequency, radius;
    oBool normalize;
    
    void (*sampleRateChanged)(struct _tBiQuad *self);
    
    
} tBiQuad;

/* State Variable Filter types */
typedef enum SVFType {
    SVFTypeHighpass = 0,
    SVFTypeLowpass,
    SVFTypeBandpass,
    SVFTypeNotch,
    SVFTypePeak,
} SVFType;

// State Variable Filter, adapted from
typedef struct _tSVF {
    SVFType type;
    float cutoff, Q;
    float ic1eq,ic2eq;
    float g,k,a1,a2,a3;
    
} tSVF;

// State Variable Filter, adapted from ???
typedef struct _tSVFE {
    SVFType type;
    float cutoff, Q;
    float ic1eq,ic2eq;
    float g,k,a1,a2,a3;
    
} tSVFE;

// Butterworth Filter
#define NUM_SVF_BW 8
typedef struct _tButterworth
{
    float gain;
	
		float N;
	
		tSVF* low[NUM_SVF_BW];
		tSVF* high[NUM_SVF_BW];
	
		float f1,f2;
	
		void (*sampleRateChanged)(struct _tButterworth *self);
    
} tButterworth;

// Highpass filter
typedef struct _tHighpass
{
    float xs, ys, R;
    float frequency;
    
    void (*sampleRateChanged)(struct _tHighpass *self);
    
} tHighpass;

/* Ramp */
typedef struct _tRamp {
    float inc;
    float inv_sr_ms;
    float curr,dest;
    float time;
    int samples_per_tick;
    
    void (*sampleRateChanged)(struct _tRamp *self);
    
} tRamp;

// Non-interpolating delay line.
typedef struct _tDelay
{
    float gain;
    float buff[DELAY_LENGTH];
    
    float lastOut, lastIn;
    
    uint32_t inPoint, outPoint;
    
    uint32_t delay, maxDelay;
    
} tDelay;

// Linear interpolating delay line. User must supply own buffer.
typedef struct _tDelayL
{
    float gain;
    float buff[DELAY_LENGTH];
    
    float lastOut, lastIn;
    
    uint32_t inPoint, outPoint;
    
    uint32_t maxDelay;
    
    float delay;
    
    float alpha, omAlpha;
    
} tDelayL;

// Allpass delay line. User must supply own buffer.
typedef struct _tDelayA
{
    float gain;
    float buff[DELAY_LENGTH];
    
    float lastOut, lastIn;
    
    uint32_t inPoint, outPoint;
    
    uint32_t maxDelay;
    
    float delay;
    
    float alpha, omAlpha, coeff;
    
    float apInput;
    
} tDelayA;


// Basic Attack-Decay envelope
typedef struct _tEnvelope {
    
    const float *exp_buff;
    const float *inc_buff;
    uint32_t buff_size;
    
    float next;
    
    float attackInc, decayInc, rampInc;
    
    oBool inAttack, inDecay, inRamp;
    
    oBool loop;
    
    float gain, rampPeak;
    
    float attackPhase, decayPhase, rampPhase;
    
} tEnvelope;

// Attack-Decay-Sustain-Release envelope
typedef struct _tADSR
{

    
    void (*sampleRateChanged)(struct _tADSR *self);
    
} tADSR;

/* Karplus Strong model */
typedef struct _tPluck
{
    tDelayA*     delayLine; // Allpass or Linear??  big difference...
    tOneZero*    loopFilter;
    tOnePole*    pickFilter;
    tNoise*      noise;
    
    float lastOut;
    float loopGain;
    float lastFreq;
    
    float sr;
    
    void (*sampleRateChanged)(struct _tPluck *self);
    
} tPluck;

/* Stif Karplus Strong model */
typedef struct _tStifKarp
{
    tDelayA*  delayLine;
    tDelayL* combDelay;
    tOneZero* filter;
    tNoise*   noise;
    tBiQuad*  biquad[4];
    
    uint32_t length;
    float loopGain;
    float baseLoopGain;
    float lastFrequency;
    float lastLength;
    float stretching;
    float pluckAmplitude;
    float pickupPosition;
    
    float lastOut;
    
    void (*sampleRateChanged)(struct _tStifKarp *self);
    
} tStifKarp;

// Envelope Follower
typedef struct _tEnvelopeFollower
{
    float y;
    float a_thresh;
    float d_coeff;
    
} tEnvelopeFollower;



// PRCRev: Reverb based on Perry Cook algorithm.
typedef struct _tPRCRev
{
    float mix, t60;
    
    float inv_441;
    
    tDelay* allpassDelays[2];
    tDelay* combDelay;
    float allpassCoeff;
    float combCoeff;
    
    float lastIn, lastOut;
    
    void (*sampleRateChanged)(struct _tPRCRev *self);
    
} tPRCRev;

// NRev: Reverb
typedef struct _tNRev
{
    float mix, t60;
    
    float inv_sr, inv_441;
    
    tDelay* allpassDelays[8];
    tDelay* combDelays[6];
    float allpassCoeff;
    float combCoeffs[6];
    float lowpassState;
    
    float lastIn, lastOut;
    
    void (*sampleRateChanged)(struct _tNRev *self);
    
} tNRev;

typedef enum NeuronMode
{
    NeuronNormal = 0,
    NeuronTanh,
    NeuronAaltoShaper,
    NeuronModeNil
} NeuronMode;

typedef struct _tNeuron
{
    
    tPoleZero* f;
    
    NeuronMode mode;
    
    float voltage, current;
    float timeStep;
    
    float alpha[3];
    float beta[3];
    float rate[3];
    float V[3];
    float P[3];
    float gK, gN, gL, C;
    
    void (*sampleRateChanged)(struct _tNeuron *self);
    
} tNeuron;


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
void     tPhasorSampleRateChanged (tPhasor *p);
void     tCycleSampleRateChanged (tCycle *c);
void     tSawtoothSampleRateChanged (tSawtooth *c);
void     tTriangleSampleRateChanged (tTriangle *c);
void     tSquareSampleRateChanged (tSquare *c);
void     tRampSampleRateChanged(tRamp *r);
void     tTwoPoleSampleRateChanged (tTwoPole *c);
void     tTwoZeroSampleRateChanged (tTwoZero *c);
void     tBiQuadSampleRateChanged (tBiQuad *c);
void     tHighpassSampleRateChanged (tHighpass *c);
void     tADSRSampleRateChanged (tADSR *c);
void     tPRCRevSampleRateChanged (tPRCRev *c);
void     tNRevSampleRateChanged (tNRev *c);
void     tPluckSampleRateChanged (tPluck *c);
void     tStifKarpSampleRateChanged (tStifKarp *c);

void     tNeuronSampleRateChanged(tNeuron* n);
void     tCompressorSampleRateChanged(tCompressor* n);
void     tButterworthSampleRateChanged(tCompressor* n);

typedef enum OOPSRegistryIndex
{
    T_PHASOR = 0,
    T_CYCLE,
    T_SAWTOOTH,
    T_TRIANGLE,
    T_SQUARE,
    T_NOISE,
    T_ONEPOLE,
    T_TWOPOLE,
    T_ONEZERO,
    T_TWOZERO,
    T_POLEZERO,
    T_BIQUAD,
    T_SVF,
    T_SVFE,
    T_HIGHPASS,
    T_DELAY,
    T_DELAYL,
    T_DELAYA,
    T_ENVELOPE,
    T_ADSR,
    T_RAMP,
    T_ENVELOPEFOLLOW,
    T_PRCREV,
    T_NREV,
    T_PLUCK,
    T_STIFKARP,
    T_NEURON,
    T_COMPRESSOR,
		T_BUTTERWORTH,
    T_INDEXCNT
}OOPSRegistryIndex;

typedef struct _OOPS
{
    float sampleRate, invSampleRate;
    
    float   (*random)(void);
    
#if N_PHASOR
    tPhasor            tPhasorRegistry          [N_PHASOR];
#endif
        
#if N_CYCLE
    tCycle             tCycleRegistry           [N_CYCLE];
#endif
       
#if N_SAWTOOTH
    tSawtooth          tSawtoothRegistry        [N_SAWTOOTH];
#endif
        
#if N_TRIANGLE
    tTriangle          tTriangleRegistry        [N_TRIANGLE];
#endif
        
#if N_SQUARE
    tSquare            tSquareRegistry          [N_SQUARE];
#endif
        
#if N_NOISE
    tNoise             tNoiseRegistry           [N_NOISE];
#endif
        
#if N_ONEPOLE
    tOnePole           tOnePoleRegistry         [N_ONEPOLE];
#endif

        
#if N_BUTTERWORTH
    tButterworth     	tButterworthRegistry      [N_BUTTERWORTH];
#endif
        
#if N_TWOPOLE
    tTwoPole           tTwoPoleRegistry         [N_TWOPOLE];
#endif
        
#if N_ONEZERO
    tOneZero           tOneZeroRegistry         [N_ONEPOLE];
#endif
        
#if N_TWOZERO
    tTwoZero           tTwoZeroRegistry         [N_TWOZERO];
#endif
        
#if N_POLEZERO
    tPoleZero          tPoleZeroRegistry        [N_POLEZERO];
#endif
        
#if N_BIQUAD
    tBiQuad            tBiQuadRegistry          [N_BIQUAD];
#endif
        
#if N_SVF
    tSVF               tSVFRegistry             [N_SVF];
#endif
        
#if N_SVFE
    tSVFE              tSVFERegistry            [N_SVFE];
#endif
        
#if N_HIGHPASS
    tHighpass          tHighpassRegistry        [N_HIGHPASS];
#endif
        
#if N_DELAY
    tDelay             tDelayRegistry           [N_DELAY];
#endif
        
#if N_DELAYL
    tDelayL            tDelayLRegistry          [N_DELAYL];
#endif
        
#if N_DELAYA
    tDelayA            tDelayARegistry          [N_DELAYA];
#endif
        
#if N_ENVELOPE
    tEnvelope          tEnvelopeRegistry        [N_ENVELOPE];
#endif
        
#if N_ADSR
    tADSR              tADSRRegistry            [N_ADSR];
#endif
        
#if N_RAMP
    tRamp              tRampRegistry            [N_RAMP];
#endif
        
#if N_ENVELOPEFOLLOW
    tEnvelopeFollower  tEnvelopeFollowerRegistry[N_ENVELOPEFOLLOW];
#endif
		
#if N_PRCREV
    tPRCRev            tPRCRevRegistry          [N_PRCREV];
#endif

#if N_NREV
    tNRev              tNRevRegistry            [N_NREV];
#endif

        
#if N_PLUCK
    tPluck             tPluckRegistry           [N_PLUCK];
#endif
		
        
#if N_STIFKARP
    tStifKarp          tStifKarpRegistry        [N_STIFKARP];
#endif
    
#if N_NEURON
    tNeuron            tNeuronRegistry          [N_NEURON];
#endif
    
#if N_COMPRESSOR    
    tCompressor        tCompressorRegistry      [N_COMPRESSOR];
#endif
    
    
    int registryIndex[T_INDEXCNT];
		
} OOPS;

extern OOPS oops;

#endif  // CAUDIO_H_INCLUDED
