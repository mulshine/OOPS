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

tEnv* env;

tHighpass* hp;

tCycle* osc;



#define MICROBLOCK_LEN 512
#define TOTAL_BUFFERS 4


float inBuffer[9182];
float outBuffer[9182];
float monoBuffer[9182];

float sampleBuffer[9182];

#endif  // FM_H_INCLUDED
