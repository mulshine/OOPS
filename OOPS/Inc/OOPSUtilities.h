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

void    tCompressor_init    (tCompressor* const);
float   tCompressor_tick    (tCompressor* const, float input);


/* Attack-Decay envelope */
void    tEnvelope_init      (tEnvelope* const, float attack, float decay, oBool loop);
float   tEnvelope_tick      (tEnvelope* const);

int     tEnvelope_etAttack (tEnvelope*  const, float attack);
int     tEnvelope_setDecay  (tEnvelope*  const, float decay);
int     tEnvelope_loop      (tEnvelope*  const, oBool loop);
int     tEnvelope_on        (tEnvelope*  const, float velocity);

/* ADSR */
void    tADSR_init      (tADSR*  const, float attack, float decay, float sustain, float release);
void    tADSR_free      (tADSR*  const);

float   tADSR_tick      (tADSR*  const);
int     tADSR_setAttack (tADSR*  const, float attack);
int     tADSR_setDecay  (tADSR*  const, float decay);
int     tADSR_setSustain(tADSR*  const, float sustain);
int     tADSR_setRelease(tADSR*  const, float release);
int     tADSR_on        (tADSR*  const, float velocity);
int     tADSR_off       (tADSR*  const);

/* Ramp */
void    tRamp_init      (tRamp* const, float time, int samplesPerTick);
void    tRamp_free      (tRamp* const);

float   tRamp_tick      (tRamp* const);
float   tRamp_sample    (tRamp* const);
int     tRamp_setTime   (tRamp* const, float time);
int     tRamp_setDest   (tRamp* const, float dest);
int     tRamp_setVal    (tRamp* const, float val);

/* Envelope Follower */
void    tEnvelopeFollower_init           (tEnvelopeFollower*  const, float attackThreshold, float decayCoeff);
void    tEnvelopeFollower_free           (tEnvelopeFollower*  const);

float   tEnvelopeFollower_tick           (tEnvelopeFollower*  const, float x);
int     tEnvelopeFollower_decayCoeff     (tEnvelopeFollower*  const, float decayCoeff);
int     tEnvelopeFollower_attackThresh   (tEnvelopeFollower*  const, float attackThresh);

/* Stack */
void    tStack_init                 (tStack* const);
void    tStack_free                 (tStack* const);

void    tStack_setCapacity          (tStack* const, uint16_t cap);
int     tStack_addIfNotAlreadyThere (tStack* const, uint16_t item);
void    tStack_add                  (tStack* const, uint16_t item);
int     tStack_remove               (tStack* const, uint16_t item);
void    tStack_clear                (tStack* const);
int     tStack_first                (tStack* const);
int     tStack_getSize              (tStack* const);
int     tStack_contains             (tStack* const, uint16_t item);
int     tStack_next                 (tStack* const);
int     tStack_get                  (tStack* const, int which);

/* Polyphonic Handler */
void        tPoly_init(tPoly* const);
void        tPoly_free(tPoly* const);

tMidiNote*  tPoly_getMidiNote(tPoly* const, int8_t voiceIndex);
void        tPoly_noteOn(tPoly* poly, int midiNoteNumber, float velocity);
void        tPoly_noteOff(tPoly* poly, int midiNoteNumber);

/* MPoly*/
void    tMPoly_init(tMPoly* const, int numVoices);
void    tMPoly_free(tMPoly* const);

void    tMPoly_tick(tMPoly* const);

//ADDING A NOTE
int     tMPoly_noteOn(tMPoly* const, int note, uint8_t vel);
int     tMPoly_noteOff(tMPoly* const, uint8_t note);
void    tMPoly_orderedAddToStack(tMPoly* const, uint8_t noteVal);
void    tMPoly_pitchBend(tMPoly* const, int pitchBend);
void    tMPoly_setNumVoices(tMPoly* const, uint8_t numVoices);
void    tMPoly_setPitchGlideTime(tMPoly* const, float t);
int     tMPoly_getNumVoices(tMPoly* const);
float   tMPoly_getPitch(tMPoly* const, uint8_t voice);
int     tMPoly_getVelocity(tMPoly* const, uint8_t voice);
int     tMPoly_isOn(tMPoly* const, uint8_t voice);

/* tSOLAD : pitch shifting */
void    tSOLAD_init             (tSOLAD* const);
void    tSOLAD_free             (tSOLAD* const);

// send one block of input samples, receive one block of output samples
void    tSOLAD_ioSamples        (tSOLAD *w, float* in, float* out, int blocksize);

// set periodicity analysis data
void    tSOLAD_setPeriod        (tSOLAD *w, float period);

// set pitch factor between 0.25 and 4
void    tSOLAD_setPitchFactor   (tSOLAD *w, float pitchfactor);

// force readpointer lag
void    tSOLAD_setReadLag       (tSOLAD *w, float readlag);

// reset state variables
void    tSOLAD_resetState       (tSOLAD *w);

/* tSNAC: */
void    tSNAC_init          (tSNAC* const, int overlaparg);    // constructor
void    tSNAC_free          (tSNAC* const);    // destructor

void    tSNAC_ioSamples     (tSNAC *s, float *in, float *out, int size);
void    tSNAC_setOverlap    (tSNAC *s, int lap);
void    tSNAC_setBias       (tSNAC *s, float bias);
void    tSNAC_setMinRMS     (tSNAC *s, float rms);

/*To get freq, perform SAMPLE_RATE/snac_getperiod() */
float   tSNAC_getPeriod     (tSNAC *s);
float   tSNAC_getfidelity   (tSNAC *s);

/* tAtkDtk */
void    tAtkDtk_init            (tAtkDtk* const, int blocksize);
void    tAtkDtk_init_expanded   (tAtkDtk* const, int blocksize, int atk, int rel);
void    tAtkDtk_free            (tAtkDtk* const);

// set expected input blocksize
void    tAtkDtk_setBlocksize    (tAtkDtk* const, int size);

// change atkDetector sample rate
void    tAtkDtk_setSamplerate   (tAtkDtk* const, int inRate);

// set attack time and coeff
void    tAtkDtk_setAtk          (tAtkDtk* const, int inAtk);

// set release time and coeff
void    tAtkDtk_setRel          (tAtkDtk* const, int inRel);

// set level above which values are identified as attacks
void    tAtkDtk_setThreshold    (tAtkDtk* const, float thres);

// find largest transient in input block, return index of attack
int     tAtkDtk_detect          (tAtkDtk* const, float *in);

/* tLockhartWavefolder */
void    tLockhartWavefolder_init    (tLockhartWavefolder* const);
void    tLockhartWavefolder_free    (tLockhartWavefolder* const);

float   tLockhartWavefolder_tick    (tLockhartWavefolder* const, float samp);


// ENV~ from PD, modified for OOPS
void    tEnv_init           (tEnv* const, int windowSize, int hopSize, int blockSize);
void    tEnv_free           (tEnv* const);
float   tEnv_tick           (tEnv* const);
void    tEnv_processBlock   (tEnv* const, float* in);

#endif  // OOPSUTILITIES_H_INCLUDED


