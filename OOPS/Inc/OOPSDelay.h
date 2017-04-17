/*
  ==============================================================================

    OOPSDelay.h
    Created: 20 Jan 2017 12:01:24pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef OOPSDELAY_H_INCLUDED
#define OOPSDELAY_H_INCLUDED

#include "OOPSMath.h"
#include "OOPSCore.h"

/* Non-interpolating delay, reimplemented from STK (Cook and Scavone). */
tDelay*  tDelayInit      (uint32_t delay);
int      tDelaySetDelay  (tDelay*  const, uint32_t delay);
uint32_t tDelayGetDelay  (tDelay*  const);
void     tDelayTapIn     (tDelay*  const, float in, uint32_t tapDelay);
float    tDelayTapOut    (tDelay*  const, uint32_t tapDelay);
float    tDelayAddTo     (tDelay*  const, float value, uint32_t tapDelay);
float    tDelayTick      (tDelay*  const, float sample);
float    tDelayGetLastOut(tDelay*  const);
float    tDelayGetLastIn (tDelay*  const);

/* Linearly-interpolating delay, reimplemented from STK (Cook and Scavone). */
tDelayL* tDelayLInit      (float delay);
int      tDelayLSetDelay  (tDelayL*  const, float delay);
float    tDelayLGetDelay  (tDelayL*  const);
void     tDelayLTapIn     (tDelayL*  const, float in, uint32_t tapDelay);
float    tDelayLTapOut    (tDelayL*  const, uint32_t tapDelay);
float    tDelayLAddTo     (tDelayL*  const, float value, uint32_t tapDelay);
float    tDelayLTick      (tDelayL*  const, float sample);
float    tDelayLGetLastOut(tDelayL*  const);
float    tDelayLGetLastIn (tDelayL*  const);

/* Allpass-interpolating delay, reimplemented from STK (Cook and Scavone). */
tDelayA* tDelayAInit      (float delay);
int      tDelayASetDelay  (tDelayA*  const, float delay);
float    tDelayAGetDelay  (tDelayA*  const);
void     tDelayATapIn     (tDelayA*  const, float in, uint32_t tapDelay);
float    tDelayATapOut    (tDelayA*  const, uint32_t tapDelay);
float    tDelayAAddTo     (tDelayA*  const, float value, uint32_t tapDelay);
float    tDelayATick      (tDelayA*  const, float sample);
float    tDelayAGetLastOut(tDelayA*  const);
float    tDelayAGetLastIn (tDelayA*  const);


#endif  // OOPSDELAY_H_INCLUDED
