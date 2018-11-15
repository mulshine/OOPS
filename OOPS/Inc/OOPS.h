/*
  ==============================================================================

    OOPSCore.h
    Created: 20 Jan 2017 12:07:26pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef OOPSCORE_H_INCLUDED
#define OOPSCORE_H_INCLUDED

#include "../Externals/mpool.h"

#include "OOPSCore.h"

void        OOPSInit            (float sampleRate, float(*randomNumberFunction)(void));
void        OOPSSetSampleRate   (float sampleRate);
float       OOPSGetSampleRate   (void);

#include "OOPSUtilities.h"

#include "OOPSDelay.h"

#include "OOPSFilter.h"

#include "OOPSOscillator.h"

#include "OOPSReverb.h"

#include "OOPSInstrument.h"

#endif  // OOPSCORE_H_INCLUDED
