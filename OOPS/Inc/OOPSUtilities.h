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

/* ADSR coming soon. */

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



/* Polyphonic Handler */
tPoly*     tPolyInit(void);
tMidiNote* tPolyGetMidiNote(tPoly* poly, int8_t voiceIndex);
void tPolyNoteOn(tPoly* poly, int midiNoteNumber, float velocity);
void tPolyNoteOff(tPoly* poly, int midiNoteNumber);

#endif  // OOPSUTILITIES_H_INCLUDED


