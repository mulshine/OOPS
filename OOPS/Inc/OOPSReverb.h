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
void    tPRCRev_init    (tPRCRev* const, float t60);
void    tPRCRev_free    (tPRCRev* const);

float   tPRCRev_tick    (tPRCRev* const, float input);

// Set reverb time in seconds.
void    tPRCRev_setT60  (tPRCRev* const, float t60);

// Set mix between dry input and wet output signal.
void    tPRCRev_setMix  (tPRCRev* const, float mix);



/* NRev: Reverb, reimplemented from STK (Cook and Scavone). */
void    tNRev_init      (tNRev* const, float t60);
void    tNRev_free      (tNRev* const);

float   tNRev_tick      (tNRev* const, float input);

// Set reverb time in seconds.
void    tNRev_setT60    (tNRev* const, float t60);

// Set mix between dry input and wet output signal.
void    tNRev_setMix    (tNRev*  const, float mix);


#endif  // OOPSREVERB_H_INCLUDED
