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
    float tauAttack, tauRelease;
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
		float minimum_time;
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

#define NUM_VOCODER_PARAM 8
#define NBANDS 16

typedef struct _tVocoder
{
    float param[NUM_VOCODER_PARAM];
    
    float gain;         //output level
    float thru, high;   //hf thru
    float kout;         //downsampled output
    int32_t  kval;      //downsample counter
    int32_t  nbnd;      //number of bands
    
    //filter coeffs and buffers - seems it's faster to leave this global than make local copy
    float f[NBANDS][13]; //[0-8][0 1 2 | 0 1 2 3 | 0 1 2 3 | val rate]
    
    void (*sampleRateChanged)(struct _tVocoder *self);
} tVocoder;

#define NUM_TALKBOX_PARAM 4

typedef struct _tTalkbox
{
    float param[NUM_TALKBOX_PARAM];
    
    ///global internal variables
    float car0[TALKBOX_BUFFER_LENGTH], car1[TALKBOX_BUFFER_LENGTH];
    float window[TALKBOX_BUFFER_LENGTH];
    float buf0[TALKBOX_BUFFER_LENGTH], buf1[TALKBOX_BUFFER_LENGTH];
    
    float emphasis;
    int32_t K, N, O, pos;
    float wet, dry, FX;
    float d0, d1, d2, d3, d4;
    float u0, u1, u2, u3, u4;
    
    void (*sampleRateChanged)(struct _tTalkbox *self);
} tTalkbox;

typedef struct _tMidiNote
{
    uint8_t pitch;
    uint8_t velocity;
    oBool on;
} tMidiNote;

typedef struct _tMidiNode tMidiNode;

typedef struct _tMidiNode
{
    tMidiNode* prev;
    tMidiNode* next;
    tMidiNote midiNote;
} tMidiNode;

typedef struct _tPolyphonicHandler
{
    tMidiNode midiNodes[128];
    tMidiNode* onListFirst;
    tMidiNode* offListFirst;
    
} tPolyphonicHandler;

typedef struct _t808Cowbell {
    
    tSquare* p[2];
    tNoise* stick;
    tSVF* bandpassOsc;
    tSVF* bandpassStick;
    tEnvelope* envGain;
    tEnvelope* envStick;
    tEnvelope* envFilter;
    tHighpass* highpass;
    float oscMix;
    float filterCutoff;
    
} t808Cowbell;

typedef struct _t808Hihat {
    
    // 6 Square waves
    tSquare* p[6];
    tNoise* n;
    tSVF* bandpassOsc;
    tSVF* bandpassStick;
    tEnvelope* envGain;
    tEnvelope* envStick;
    tHighpass* highpass;
    tNoise* stick;
    
    float oscNoiseMix;
    
    
} t808Hihat;

typedef struct _t808Snare {
    
    // Tone 1, Tone 2, Noise
    tTriangle* tone[2]; // Tri (not yet antialiased or wavetabled)
    tNoise* noiseOsc;
    tSVF* toneLowpass[2];
    tSVF* noiseLowpass; // Lowpass from SVF filter
    tEnvelope* toneEnvOsc[2];
    tEnvelope* toneEnvGain[2];
    tEnvelope* noiseEnvGain;
    tEnvelope* toneEnvFilter[2];
    tEnvelope* noiseEnvFilter;
    
    float toneGain[2];
    float noiseGain;
    
    float toneNoiseMix;
    
    float tone1Freq, tone2Freq;
    
    float noiseFilterFreq;
    
    
} t808Snare;


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
void     tButterworthSampleRateChanged(tButterworth* n);

void     tVocoderSampleRateChanged(tVocoder* n);

void     t808SnareSampleRateChanged(t808Snare* n);
void     t808HihatSampleRateChanged(t808Hihat* n);
void     t808CowbellSampleRateChanged(t808Cowbell* n);

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
    T_VOCODER,
    T_TALKBOX,
    T_POLYPHONICHANDLER,
    T_808SNARE,
    T_808HIHAT,
    T_808COWBELL,
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
    
#if N_VOCODER
    tVocoder           tVocoderRegistry      [N_VOCODER];
#endif
    
#if N_TALKBOX
    tTalkbox           tTalkboxRegistry         [N_TALKBOX];
#endif

#if N_POLYPHONICHANDLER
    tPolyphonicHandler  tPolyphonicHandlerRegistry     [N_POLYPHONICHANDLER];
#endif
    
#if N_808SNARE
    t808Snare        t808SnareRegistry      [N_808SNARE];
#endif
    
#if N_808HIHAT
    t808Hihat         t808HihatRegistry      [N_808HIHAT];
#endif
    
#if N_808COWBELL
    t808Cowbell       t808CowbellRegistry      [N_808COWBELL];
#endif
    
    
    int registryIndex[T_INDEXCNT];
		
} OOPS;

extern OOPS oops;

#endif  // CAUDIO_H_INCLUDED
