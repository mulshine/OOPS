/*
  ==============================================================================

    OOPSUtilities.h
    Created: 20 Jan 2017 12:02:17pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef OOPSUTILITIES_H_INCLUDED
#define OOPSUTILITIES_H_INCLUDED

#include "OOPSCore.h"
/* Compressor */

tCompressor*            tCompressorInit    (void);
float                   tCompressorTick    (tCompressor* const, float input);


/* Attack-Decay envelope */
tEnvelope*              tEnvelopeInit      (float attack, float decay, oBool loop);
float                   tEnvelopeTick      (tEnvelope*  const);

int                     tEnvelopeSetAttack (tEnvelope*  const, float attack);

int                     tEnvelopeSetDecay  (tEnvelope*  const, float decay);

int                     tEnvelopeLoop      (tEnvelope*  const, oBool loop);

int                     tEnvelopeOn        (tEnvelope*  const, float velocity);

/* ADSR */
tADSR*                  tADSRInit      (float attack, float decay, float sustain, float release);
float                   tADSRTick      (tADSR*  const);

int                     tADSRSetAttack (tADSR*  const, float attack);

int                     tADSRSetDecay  (tADSR*  const, float decay);

int                     tADSRSetSustain (tADSR*  const, float sustain);

int                     tADSRSetRelease (tADSR*  const, float release);

int                     tADSROn        (tADSR*  const, float velocity);

int                     tADSROff        (tADSR*  const);

/* Ramp */
tRamp*                  tRampInit   (float time, int samplesPerTick);
float                   tRampTick   (tRamp*  const);
float 									tRampSample (tRamp*  const);

int                     tRampSetTime(tRamp*  const, float time);

int                     tRampSetDest(tRamp*  const, float dest);

/* Envelope Follower */
tEnvelopeFollower*      tEnvelopeFollowerInit           (float attackThreshold, float decayCoeff);
float                   tEnvelopeFollowerTick           (tEnvelopeFollower*  const, float x);

int                     tEnvelopeFollowerDecayCoeff     (tEnvelopeFollower*  const, float decayCoeff);

int                     tEnvelopeFollowerAttackThresh   (tEnvelopeFollower*  const, float attackThresh);

/* Stack */
tStack*     tStack_init                 (void);
void        tStack_setCapacity          (tStack* const, uint16_t cap);
int         tStack_addIfNotAlreadyThere (tStack* const, uint16_t item);
void        tStack_add                  (tStack* const, uint16_t item);
int         tStack_remove               (tStack* const, uint16_t item);
void        tStack_clear                (tStack* const);
int         tStack_first                (tStack* const);
int         tStack_getSize              (tStack* const);
int         tStack_contains             (tStack* const, uint16_t item);
int         tStack_next                 (tStack* const);
int         tStack_get                  (tStack* const, int which);

/* Polyphonic Handler */
tPoly*     tPolyInit(void);
tMidiNote* tPolyGetMidiNote(tPoly* poly, int8_t voiceIndex);
void tPolyNoteOn(tPoly* poly, int midiNoteNumber, float velocity);
void tPolyNoteOff(tPoly* poly, int midiNoteNumber);

/* MPoly*/
tMPoly*     tMPoly_init(int numVoices);

void        tMPoly_tick(tMPoly* const);

//ADDING A NOTE
void        tMPoly_noteOn(tMPoly* const, int note, uint8_t vel);

int         tMPoly_noteOff(tMPoly* const, uint8_t note);

void        tMPoly_orderedAddToStack(tMPoly* const, uint8_t noteVal);

void        tMPoly_pitchBend(tMPoly* const, int pitchBend);

void        tMPoly_setNumVoices(tMPoly* const, uint8_t numVoices);

void        tMPoly_setPitchGlideTime(tMPoly* const, float t);

int         tMPoly_getNumVoices(tMPoly* const);

float       tMPoly_getPitch(tMPoly* const, uint8_t voice);

int         tMPoly_getVelocity(tMPoly* const, uint8_t voice);

int         tMPoly_isOn(tMPoly* const, uint8_t voice);

/* tSOLAD : pitch shifting */
tSOLAD*     tSOLAD_init(void);

// send one block of input samples, receive one block of output samples
void tSOLAD_ioSamples(tSOLAD *w, float* in, float* out, int blocksize);

// set periodicity analysis data
void tSOLAD_setPeriod(tSOLAD *w, float period);

// set pitch factor between 0.25 and 4
void tSOLAD_setPitchFactor(tSOLAD *w, float pitchfactor);

// force readpointer lag
void tSOLAD_setReadLag(tSOLAD *w, float readlag);

// reset state variables
void tSOLAD_resetState(tSOLAD *w);

/* tSNAC: */
tSNAC *tSNAC_init(int periodarg, int overlaparg);    // constructor

void tSNAC_ioSamples(tSNAC *s, float *in, float *out, int size);
void tSNAC_setFramesize(tSNAC *s, int frame);
void tSNAC_setOverlap(tSNAC *s, int lap);
void tSNAC_setBias(tSNAC *s, float bias);
void tSNAC_setMinRMS(tSNAC *s, float rms);

/*To get freq, perform SAMPLE_RATE/snac_getperiod() */
float tSNAC_getPeriod(tSNAC *s);
float tSNAC_getfidelity(tSNAC *s);

/* tAtkDtk */
tAtkDtk* tAtkDtk_init(int blocksize);

tAtkDtk* tAtkDtk_init_expanded(int blocksize, int atk, int rel);

void tAtkDtk_free(tAtkDtk *a);

// set expected input blocksize
void tAtkDtk_setBlocksize(tAtkDtk *a, int size);

// change atkDetector sample rate
void tAtkDtk_setSamplerate(tAtkDtk *a, int inRate);

// set attack time and coeff
void tAtkDtk_setAtk(tAtkDtk *a, int inAtk);

// set release time and coeff
void tAtkDtk_setRel(tAtkDtk *a, int inRel);

// set level above which values are identified as attacks
void tAtkDtk_setThreshold(tAtkDtk *a, float thres);

// find largest transient in input block, return index of attack
int tAtkDtk_detect(tAtkDtk *a, float *in);

/* tLockhartWavefolder */
tLockhartWavefolder* tLockhartWavefolderInit(void);
float tLockhartWavefolderTick(tLockhartWavefolder* const, float samp);


// ENV~ from PD, modified for OOPS
tEnv* tEnvInit(int windowSize, int hopSize);
float tEnvTick (tEnv* x);
void tEnvProcessBlock(tEnv* x, float* in);

#endif  // OOPSUTILITIES_H_INCLUDED


