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

#define TCYCLE_SIZE sizeof(tCycle)

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
    const float *exp_buff;
    const float *inc_buff;
    uint32_t buff_size;
    
    float next;
    
    float attackInc, decayInc, releaseInc, rampInc;
    
    oBool inAttack, inDecay, inSustain, inRelease, inRamp;
    
    float sustain, gain, rampPeak, releasePeak;
    
    float attackPhase, decayPhase, releasePhase, rampPhase;
    
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

typedef struct _tPoly
{
    tMidiNode midiNodes[128];
    tMidiNode* onListFirst;
    tMidiNode* offListFirst;
    
} tPoly;

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

#define STACK_SIZE 128
typedef struct _tStack
{
    int data[STACK_SIZE];
    uint16_t pos;
    uint16_t size;
    uint16_t capacity;
    oBool ordered;
    
} tStack;


/* tMPoly */
typedef struct _tMPoly
{
    int numVoices;
    int numVoicesActive;
    
    int voices[NUM_VOICES][2];
    
    int notes[128][2];
    
    int CCs[128];
    
    uint8_t CCsRaw[128];
    
    int lastVoiceToChange;
    
    tStack* stack;
    tStack* orderStack;
    
    int32_t pitchBend;
    
    int currentNote;
    int currentVoice;
    int currentVelocity;
    int maxLength;
    
} tMPoly;

#define LOOPSIZE 4096           // loop size must be power of two
#define LOOPMASK (LOOPSIZE - 1)
#define PITCHFACTORDEFAULT 1.0f
#define INITPERIOD 64.0f
#define MAXPERIOD (float)((LOOPSIZE - w->blocksize) * 0.8f)
#define MINPERIOD 8.0f

/* tSoladPS : pitch shifting algorithm */
typedef struct _tSOLAD
{
    int timeindex;              // current reference time, write index
    int blocksize;              // signal input / output block size
    float pitchfactor;        // pitch factor between 0.25 and 4
    float readlag;            // read pointer's lag behind write pointer
    float period;             // period length in input signal
    float jump;               // read pointer jump length and direction
    float xfadelength;        // crossfade length expressed at input sample rate
    float xfadevalue;         // crossfade phase and value
    float delaybuf[LOOPSIZE+16];
    
    void (*sampleRateChanged)(struct _tSOLAD *self);
} tSOLAD;

#define DEFFRAMESIZE 1024           // default analysis framesize
#define DEFOVERLAP 1                // default overlap
#define DEFBIAS 0.2f        // default bias
#define DEFMINRMS 0.003f   // default minimum RMS
#define SEEK 0.85f       // seek-length as ratio of framesize

typedef struct _tSNAC
{
    float *inputbuf;
    float *processbuf;
    float *spectrumbuf;
    float *biasbuf;
    
    int timeindex;
    int framesize;
    int overlap;
    int periodindex;
    
    float periodlength;
    float fidelity;
    float biasfactor;
    float minrms;
    
    void (*sampleRateChanged)(struct _tSNAC *self);
} tSNAC;

#define DEFSAMPLERATE 44100
#define DEFBLOCKSIZE 1024
#define DEFTHRESHOLD 6
#define DEFATTACK    10
#define DEFRELEASE    10

typedef struct _tAtkDtk
{
    float env;
    
    //Attack & Release times in msec
    int atk;
    int rel;
    
    //Attack & Release coefficients based on times
    float atk_coeff;
    float rel_coeff;
    
    int blocksize;
    int samplerate;
    
    //RMS amplitude of previous block - used to decide if attack is present
    float prevAmp;
    
    float threshold;
} tAtkDtk;

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

void     tSOLADSampleRateChanged(tSOLAD* n);
void     tSNACSampleRateChanged(tSNAC* n);

typedef struct _OOPS
{
    float sampleRate, invSampleRate;
    
    float   (*random)(void);
		
} OOPS;

extern OOPS oops;

#endif  // CAUDIO_H_INCLUDED
