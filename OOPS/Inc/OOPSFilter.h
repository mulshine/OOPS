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

#define HOLY_SHIT 0

void        tButterworth_init       (tButterworth* const, int N, float f1, float f2);
float       tButterworth_tick       (tButterworth* const, float input);
void        tButterworth_setF1      (tButterworth* const, float in);
void        tButterworth_setF2      (tButterworth* const, float in);
void        tButterworth_setFreqs   (tButterworth* const, float f1, float f2);

/* tOnePole: OnePole filter, reimplemented from STK (Cook and Scavone). */
void        tOnePole_init           (tOnePole*  const, float thePole);
float       tOnePole_tick           (tOnePole*  const, float input);
void        tOnePole_setB0          (tOnePole*  const, float b0);
void        tOnePole_setA1          (tOnePole*  const, float a1);
void        tOnePole_setPole        (tOnePole*  const, float thePole);
void        tOnePole_setCoefficients(tOnePole*  const, float b0, float a1);
void        tOnePole_setGain        (tOnePole*  const, float gain);

/* TwoPole filter, reimplemented from STK (Cook and Scavone). */
void        tTwoPole_init           (tTwoPole*  const);
float       tTwoPole_tick           (tTwoPole*  const, float input);

void        tTwoPole_setB0          (tTwoPole*  const, float b0);
void        tTwoPole_setA1          (tTwoPole*  const, float a1);
void        tTwoPole_setA2          (tTwoPole*  const, float a2);
void        tTwoPole_setResonance   (tTwoPole*  const, float freq, float radius, oBool normalize);
void        tTwoPole_setCoefficients(tTwoPole*  const, float b0, float a1, float a2);
void        tTwoPole_setGain        (tTwoPole*  const, float gain);

/* OneZero filter, reimplemented from STK (Cook and Scavone). */
void        tOneZero_init           (tOneZero*  const, float theZero);
float       tOneZero_tick           (tOneZero*  const, float input);

void        tOneZero_setB0          (tOneZero*  const, float b0);
void        tOneZero_setB1          (tOneZero*  const, float b1);
void        tOneZero_setZero        (tOneZero*  const, float theZero);
void        tOneZero_setCoefficients(tOneZero*  const, float b0, float b1);
void        tOneZero_setGain        (tOneZero*  const, float gain);
float       tOneZero_getPhaseDelay(tOneZero *f, float frequency );

/* TwoZero filter, reimplemented from STK (Cook and Scavone). */
void        tTwoZero_init           (tTwoZero*  const);
float       tTwoZero_tick           (tTwoZero*  const, float input);

void        tTwoZero_setB0          (tTwoZero*  const, float b0);
void        tTwoZero_setB1          (tTwoZero*  const, float b1);
void        tTwoZero_setB2          (tTwoZero*  const, float b2);
void        tTwoZero_setNotch       (tTwoZero*  const, float frequency, float radius);
void        tTwoZero_setCoefficients(tTwoZero*  const, float b0, float b1, float b2);
void        tTwoZero_setGain        (tTwoZero*  const, float gain);

/* PoleZero filter, reimplemented from STK (Cook and Scavone). */
void        tPoleZero_init              (tPoleZero*  const);
float       tPoleZero_tick              (tPoleZero*  const, float input);

void        tPoleZero_setB0             (tPoleZero*  const, float b0);
void        tPoleZero_setB1             (tPoleZero*  const, float b1);
void        tPoleZero_setA1             (tPoleZero*  const, float a1);
void        tPoleZero_setCoefficients   (tPoleZero*  const, float b0, float b1, float a1);
void        tPoleZero_setAllpass        (tPoleZero*  const, float coeff);
void        tPoleZero_setBlockZero      (tPoleZero*  const, float thePole);
void        tPoleZero_setGain           (tPoleZero*  const, float gain);

/* BiQuad filter, reimplemented from STK (Cook and Scavone). */
void        tBiQuad_init           (tBiQuad*  const);
float       tBiQuad_tick           (tBiQuad*  const, float input);

void        tBiQuad_setB0          (tBiQuad*  const, float b0);
void        tBiQuad_setB1          (tBiQuad*  const, float b1);
void        tBiQuad_setB2          (tBiQuad*  const, float b2);
void        tBiQuad_setA1          (tBiQuad*  const, float a1);
void        tBiQuad_setA2          (tBiQuad*  const, float a2);
void        tBiQuad_setNotch       (tBiQuad*  const, float freq, float radius);
void        tBiQuad_setResonance   (tBiQuad*  const, float freq, float radius, oBool normalize);
void        tBiQuad_setCoefficients(tBiQuad*  const, float b0, float b1, float b2, float a1, float a2);
void        tBiQuad_setGain        (tBiQuad*  const, float gain);

/* State Variable Filter, algorithm from Andy Simper. */
void        tSVF_init        (tSVF*  const, SVFType type, float freq, float Q);
float       tSVF_tick        (tSVF*  const, float v0);

int         tSVF_setFreq     (tSVF*  const, float freq);
int         tSVF_setQ        (tSVF*  const, float Q);

/* Efficient State Variable Filter for 14-bit control input, [0, 4096). */
void        tSVFE_init       (tSVFE*  const, SVFType type, uint16_t controlFreq, float Q);
float       tSVFE_tick       (tSVFE*  const, float v0);

int         tSVFE_setFreq    (tSVFE*  const, uint16_t controlFreq);
int         tSVFE_setQ       (tSVFE*  const, float Q);

/* Simple Highpass filter. */
void        tHighpass_init      (tHighpass*  const, float freq);
float       tHighpass_tick      (tHighpass*  const, float x);

void        tHighpass_setFreq   (tHighpass*  const, float freq);
float       tHighpass_setFreq   (tHighpass*  const);

#if HOLY_SHIT
void        tFormantShifter_init            (tFormantShifter* const);
float       tFormantShifter_tick            (tFormantShifter* const, float input, float fwarp);
float       tFormantShifter_remove          (tFormantShifter* const, float input);
float       tFormantShifter_add             (tFormantShifter* const, float input, float fwarp);
void        tFormantShifter_ioSamples       (tFormantShifter* const, float* in, float* out, int size, float fwarp);

void        tPitchShifter_init              (tPitchShifter* const, float* in, float* out, int bufSize, int frameSize);
float       tPitchShifter_tick              (tPitchShifter* const, float sample);
float       tPitchShifterToFreq_tick        (tPitchShifter* const, float sample, float freq);
float       tPitchShifterToFunc_tick        (tPitchShifter* const, float sample, float (*fun)(float));
void        tPitchShifter_ioSamples_toFreq  (tPitchShifter* const, float* in, float* out, int size, float toFreq);
void        tPitchShifter_ioSamples_toPeriod(tPitchShifter* const, float* in, float* out, int size, float toPeriod);
void        tPitchShifter_ioSamples_toFunc  (tPitchShifter* const, float* in, float* out, int size, float (*fun)(float));
void        tPitchShifter_setPitchFactor    (tPitchShifter* const, float pf);
void        tPitchShifter_setTimeConstant   (tPitchShifter* const, float tc);
void        tPitchShifter_setHopSize        (tPitchShifter* const, int hs);
void        tPitchShifter_setWindowSize     (tPitchShifter* const, int ws);
float		tPitchShifter_getPeriod			(tPitchShifter* const);

void        tPeriod_init					(tPeriod* const, float* in, float* out, int bufSize, int frameSize);
float 		tPeriod_findPeriod				(tPeriod* const, float sample);
void 		tPeriod_setHopSize				(tPeriod* p, int hs);
void 		tPeriod_setWindowSize			(tPeriod* p, int ws);

void        tPitchShift_init			    (tPitchShift* const, tPeriod* const, float* out, int bufSize);
float 		tPitchShift_shift				(tPitchShift* const);
float 		tPitchShift_shiftToFunc 		(tPitchShift* const, float (*fun)(float));
float		tPitchShift_shiftToFreq			(tPitchShift* const, float freq);
void 		tPitchShift_setPitchFactor		(tPitchShift* const, float pf);
#endif

#endif  // OOPSFILTER_H_INCLUDED
