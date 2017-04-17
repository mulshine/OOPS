/*
  ==============================================================================

    OOPSMath.h
    Created: 22 Jan 2017 7:02:56pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef OOPSMATH_H_INCLUDED
#define OOPSMATH_H_INCLUDED

#include "math.h"
#include "stdint.h"
#include "stdlib.h"


typedef enum oBool
{
    OTRUE  = 1,
    OFALSE = 0
}oBool;



#define PI              (3.14159265358979f)
#define TWO_PI          (2 * PI)



#define VSF             1.0e-38f

#define MAX_DELAY       8192
#define INV_128         (1.0f / 128.0f)

#define INV_20         0.05f
#define INV_40         0.025
#define INV_80         0.0125f
#define INV_160        0.00625f
#define INV_320        0.003125f
#define INV_640        0.0015625f
#define INV_1280       0.00078125f
#define INV_2560       0.000390625f
#define INV_5120       0.0001953125f
#define INV_10240      0.00009765625f
#define INV_20480      0.000048828125f

float       OOPS_clip               (float min, float val, float max);
oBool       OOPS_isPrime            (uint64_t number );
float       OOPS_midiToFrequency    (float f);



#endif  // OOPSMATH_H_INCLUDED
