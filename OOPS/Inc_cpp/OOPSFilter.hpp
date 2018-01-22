/*
  ==============================================================================

    OOPSFilter.h
    Created: 20 Jan 2017 12:01:10pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef OOPSFILTER_H_INCLUDED
#define OOPSFILTER_H_INCLUDED

#include "OOPSCore.h"

tButterworth* tButterworthInit(int N, float f1, float f2);
float tButterworthTick(tButterworth* const, float input);

void tButterworthSetF1(tButterworth* const, float in);
void tButterworthSetF2(tButterworth* const, float in);
void tButterworthSetFreqs(tButterworth* const, float f1, float f2);

/* tOnePole: OnePole filter, reimplemented from STK (Cook and Scavone). */
tOnePole*   tOnePoleInit           (float thePole);
float       tOnePoleTick           (tOnePole*  const, float input);

void        tOnePoleSetB0          (tOnePole*  const, float b0);
void        tOnePoleSetA1          (tOnePole*  const, float a1);
void        tOnePoleSetPole        (tOnePole*  const, float thePole);
void        tOnePoleSetCoefficients(tOnePole*  const, float b0, float a1);
void        tOnePoleSetGain        (tOnePole*  const, float gain);

/* TwoPole filter, reimplemented from STK (Cook and Scavone). */
tTwoPole*   tTwoPoleInit           (void);
float       tTwoPoleTick           (tTwoPole*  const, float input);

void        tTwoPoleSetB0          (tTwoPole*  const, float b0);
void        tTwoPoleSetA1          (tTwoPole*  const, float a1);
void        tTwoPoleSetA2          (tTwoPole*  const, float a2);
void        tTwoPoleSetResonance   (tTwoPole*  const, float freq, float radius, oBool normalize);
void        tTwoPoleSetCoefficients(tTwoPole*  const, float b0, float a1, float a2);
void        tTwoPoleSetGain        (tTwoPole*  const, float gain);

/* OneZero filter, reimplemented from STK (Cook and Scavone). */
tOneZero*   tOneZeroInit           (float theZero);
float       tOneZeroTick           (tOneZero*  const, float input);

void        tOneZeroSetB0          (tOneZero*  const, float b0);
void        tOneZeroSetB1          (tOneZero*  const, float b1);
void        tOneZeroSetZero        (tOneZero*  const, float theZero);
void        tOneZeroSetCoefficients(tOneZero*  const, float b0, float b1);
void        tOneZeroSetGain        (tOneZero*  const, float gain);
float       tOneZeroGetPhaseDelay(tOneZero *f, float frequency );

/* TwoZero filter, reimplemented from STK (Cook and Scavone). */
tTwoZero*   tTwoZeroInit           (void);
float       tTwoZeroTick           (tTwoZero*  const, float input);

void        tTwoZeroSetB0          (tTwoZero*  const, float b0);
void        tTwoZeroSetB1          (tTwoZero*  const, float b1);
void        tTwoZeroSetB2          (tTwoZero*  const, float b2);
void        tTwoZeroSetNotch       (tTwoZero*  const, float frequency, float radius);
void        tTwoZeroSetCoefficients(tTwoZero*  const, float b0, float b1, float b2);
void        tTwoZeroSetGain        (tTwoZero*  const, float gain);

/* PoleZero filter, reimplemented from STK (Cook and Scavone). */
tPoleZero*  tPoleZeroInit              (void);
float       tPoleZeroTick              (tPoleZero*  const, float input);

void        tPoleZeroSetB0             (tPoleZero*  const, float b0);
void        tPoleZeroSetB1             (tPoleZero*  const, float b1);
void        tPoleZeroSetA1             (tPoleZero*  const, float a1);
void        tPoleZeroSetCoefficients   (tPoleZero*  const, float b0, float b1, float a1);
void        tPoleZeroSetAllpass        (tPoleZero*  const, float coeff);
void        tPoleZeroSetBlockZero      (tPoleZero*  const, float thePole);
void        tPoleZeroSetGain           (tPoleZero*  const, float gain);

/* BiQuad filter, reimplemented from STK (Cook and Scavone). */
tBiQuad*    tBiQuadInit           (void);
float       tBiQuadTick           (tBiQuad*  const, float input);

void        tBiQuadSetB0          (tBiQuad*  const, float b0);
void        tBiQuadSetB1          (tBiQuad*  const, float b1);
void        tBiQuadSetB2          (tBiQuad*  const, float b2);
void        tBiQuadSetA1          (tBiQuad*  const, float a1);
void        tBiQuadSetA2          (tBiQuad*  const, float a2);
void        tBiQuadSetNotch       (tBiQuad*  const, float freq, float radius);
void        tBiQuadSetResonance   (tBiQuad*  const, float freq, float radius, oBool normalize);
void        tBiQuadSetCoefficients(tBiQuad* const f, float b0, float b1, float b2, float a1, float a2);
void        tBiQuadSetGain        (tBiQuad*  const, float gain);

/* State Variable Filter, algorithm from Andy Simper. */
tSVF*       tSVFInit        (SVFType type, float freq, float Q);
float       tSVFTick        (tSVF*  const, float v0);

int         tSVFSetFreq     (tSVF*  const, float freq);
int         tSVFSetQ        (tSVF*  const, float Q);

/* Efficient State Variable Filter for 14-bit control input, [0, 4096). */
tSVFE*      tSVFEInit       (SVFType type, uint16_t controlFreq, float Q);
float       tSVFETick       (tSVFE*  const, float v0);

int         tSVFESetFreq    (tSVFE*  const, uint16_t controlFreq);
int         tSVFESetQ       (tSVFE*  const, float Q);

/* Simple Highpass filter. */
tHighpass*  tHighpassInit      (float freq);
float       tHighpassTick      (tHighpass*  const, float x);

void        tHighpassSetFreq   (tHighpass*  const, float freq);
float       tHighpassGetFreq   (tHighpass*  const);



#endif  // OOPSFILTER_H_INCLUDED
