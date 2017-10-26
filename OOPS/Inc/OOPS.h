/*
  ==============================================================================

    OOPSCore.h
    Created: 20 Jan 2017 12:07:26pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef OOPSCORE_H_INCLUDED
#define OOPSCORE_H_INCLUDED


#include "OOPSCore.h"

void        OOPSInit            (float sampleRate, float(*randomNumberFunction)(void));
void        OOPSSetSampleRate   (float sampleRate);
float       OOPSGetSampleRate   (void);

#if INC_UTILITIES
#include "OOPSUtilities.h"
#endif

#if INC_DELAY
#include "OOPSDelay.h"
#endif

#if INC_FILTER
#include "OOPSFilter.h"
#endif

#if INC_OSCILLATOR
#include "OOPSOscillator.h"
#endif

#if INC_REVERB
#include "OOPSReverb.h"
#endif

#if INC_INSTRUMENT
#include "OOPSInstrument.h"
#endif

#endif  // OOPSCORE_H_INCLUDED
