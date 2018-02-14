/*
  ==============================================================================

    MyTest.h
    Created: 23 Jan 2017 9:39:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef FM_H_INCLUDED
#define FM_H_INCLUDED

tSOLAD* sola;
tSNAC* snac;
tAtkDtk* atk;

tCycle* osc;

#define MICROBLOCK_LEN      512
#define TOTAL_BUFFERS       4
#define BUFSIZE             (TOTAL_BUFFERS * MICROBLOCK_LEN)


float inBuffer[BUFSIZE];
float outBuffer[BUFSIZE];
float sampleBuffer[BUFSIZE];

#endif  // FM_H_INCLUDED
