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

tCompressor*            tCompressorInit    (float tauAttack, float tauRelease);
float                   tCompressorTick    (tCompressor* const, float input);


int     tCompressorSetAttack(tCompressor* const comp, float attack);

int     tCompressorSetRelease(tCompressor* const comp, float release);

int     tCompressorSetThreshold(tCompressor* const comp, float thresh);

int     tCompressorSetRatio(tCompressor* const comp, float ratio);

int     tCompressorSetMakeupGain(tCompressor* const comp, float gain);

int     tCompressorSetKneeWidth(tCompressor* const comp, float knee);


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

int                     tRampSetTime(tRamp*  const, float time);

int                     tRampSetDest(tRamp*  const, float dest);



/* Envelope Follower */
tEnvelopeFollower*      tEnvelopeFollowerInit           (float attackThreshold, float decayCoeff);
float                   tEnvelopeFollowerTick           (tEnvelopeFollower*  const, float x);

int                     tEnvelopeFollowerDecayCoeff     (tEnvelopeFollower*  const, float decayCoeff);

int                     tEnvelopeFollowerAttackThresh   (tEnvelopeFollower*  const, float attackThresh);


#endif  // OOPSUTILITIES_H_INCLUDED
