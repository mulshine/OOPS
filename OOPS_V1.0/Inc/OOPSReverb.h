/*
  ==============================================================================

    OOPSReverb.h
    Created: 20 Jan 2017 12:02:04pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef OOPSREVERB_H_INCLUDED
#define OOPSREVERB_H_INCLUDED

#include "OOPSMath.h"
#include "OOPSCore.h"

/* PRCRev: Reverb, reimplemented from STK (Cook and Scavone). */
tPRCRev*    tPRCRevInit      (float t60);
float       tPRCRevTick      (tPRCRev*  const, float input);

// Set reverb time in seconds.
void        tPRCRevSetT60    (tPRCRev*  const, float t60);

// Set mix between dry input and wet output signal.
void        tPRCRevSetMix    (tPRCRev*  const, float mix);




/* NRev: Reverb, reimplemented from STK (Cook and Scavone). */
tNRev*      tNRevInit   (float t60);
float       tNRevTick   (tNRev*  const, float input);

// Set reverb time in seconds.
void        tNRevSetT60 (tNRev*  const, float t60);

// Set mix between dry input and wet output signal.
void        tNRevSetMix (tNRev*  const, float mix);


#endif  // OOPSREVERB_H_INCLUDED
