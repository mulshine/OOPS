/*
  ==============================================================================

    CAudioLibraryTest.h
    Created: 4 Dec 2016 9:14:16pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef OOPSTEST1_H_INCLUDED
#define OOPSTEST1_H_INCLUDED

#include "OOPSLink.h"

#include "../../OOPS/Inc/OOPS.h"

#include "Yin.h"

// OOPSTest API
float   OOPSTest_tick            (float input);
void    OOPSTest_init            (float sampleRate, int blocksize);
void    OOPSTest_end             (void);
void    OOPSTest_block           (void);

void    OOPSTest_noteOn          (int midiNoteNumber, float velocity);
void    OOPSTest_noteOff         (int midiNoteNumber);

#endif  // OOPSTEST1_H_INCLUDED
