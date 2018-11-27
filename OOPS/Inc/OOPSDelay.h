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
void        tDelay_init         (tDelay*  const, uint32_t delay);
void        tDelay_free         (tDelay* const);

int         tDelay_setDelay     (tDelay*  const, uint32_t delay);
uint32_t    tDelay_getDelay     (tDelay*  const);
void        tDelay_tapIn        (tDelay*  const, float in, uint32_t tapDelay);
float       tDelay_tapOut       (tDelay*  const, uint32_t tapDelay);
float       tDelay_addTo        (tDelay*  const, float value, uint32_t tapDelay);
float       tDelay_tick         (tDelay*  const, float sample);
float       tDelay_getLastOut   (tDelay*  const);
float       tDelay_getLastIn    (tDelay*  const);


/* Linearly-interpolating delay, reimplemented from STK (Cook and Scavone). */
void        tDelayL_init        (tDelayL*  const, float delay);
void        tDelayL_free        (tDelayL* const);

int         tDelayL_setDelay    (tDelayL*  const, float delay);
float       tDelayL_getDelay    (tDelayL*  const);
void        tDelayL_tapIn       (tDelayL*  const, float in, uint32_t tapDelay);
float       tDelayL_tapOut      (tDelayL*  const, uint32_t tapDelay);
float       tDelayL_addTo       (tDelayL*  const, float value, uint32_t tapDelay);
float       tDelayL_tick        (tDelayL*  const, float sample);
float       tDelayL_getLastOut  (tDelayL*  const);
float       tDelayL_getLastIn   (tDelayL*  const);

/* Allpass-interpolating delay, reimplemented from STK (Cook and Scavone). */
void        tDelayA_init        (tDelayA*  const, float delay);
void        tDelayA_free        (tDelayA* const);
int         tDelayA_setDelay    (tDelayA*  const, float delay);
float       tDelayA_getDelay    (tDelayA*  const);
void        tDelayA_tapIn       (tDelayA*  const, float in, uint32_t tapDelay);
float       tDelayA_tapOut      (tDelayA*  const, uint32_t tapDelay);
float       tDelayA_addTo       (tDelayA*  const, float value, uint32_t tapDelay);
float       tDelayA_tick        (tDelayA*  const, float sample);
float       tDelayA_getLastOut  (tDelayA*  const);
float       tDelayA_getLastIn   (tDelayA*  const);

#endif  // OOPSDELAY_H_INCLUDED
