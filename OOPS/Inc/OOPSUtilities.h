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
tMPoly*     tMPoly_init(void);

//ADDING A NOTE
void        tMPoly_noteOn(tMPoly* const, int note, uint8_t vel);

void        tMPoly_noteOff(tMPoly* const, uint8_t note);

void        tMPoly_orderedAddToStack(tMPoly* const, uint8_t noteVal);

void        tMPoly_pitchBend(tMPoly* const, uint8_t bend);




#endif  // OOPSUTILITIES_H_INCLUDED


