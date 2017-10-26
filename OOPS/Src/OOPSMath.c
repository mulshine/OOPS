/*
  ==============================================================================

    OOPSMath.c
    Created: 22 Jan 2017 7:02:56pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../Inc/OOPSMath.h"
#include "../Inc/OOPSWavetables.h"

// The C-embedded Audio Library.
#define TWO_TO_16 65536.f

#define EXPONENTIAL_TABLE_SIZE 65536



float   OOPS_clip(float min, float val, float max) {
    
    if (val < min) {
        return min;
    } else if (val > max) {
        return max;
    } else {
        return val;
    }
}

oBool     OOPS_isPrime(uint64_t number )
{
    if ( number == 2 ) return OTRUE;
    if ( number & 1 ) {
        for ( int i=3; i<(int)sqrt((double)number)+1; i+=2 )
            if ( (number % i) == 0 ) return OFALSE;
        return OTRUE; // prime
    }
    else return OFALSE; // even
}

// Adapted from MusicDSP: http://www.musicdsp.org/showone.php?id=238
float OOPS_tanh(float x)
{
    
    if( x < -3 )
        return -1;
    else if( x > 3 )
        return 1;
    else
        return x * ( 27 + x * x ) / ( 27 + 9 * x * x );
}

//-----------------------------------------------------------------------------
// name: mtof()
// desc: midi to freq, from PD source
//-----------------------------------------------------------------------------
float OOPS_midiToFrequency(float f)
{
    if( f <= -1500.0f ) return (0);
    else if( f > 1499.0f ) return (OOPS_midiToFrequency(1499.0f));
    else return ( powf(2.0f, (f - 69.0f) / 12.0f) * 440.0f );
}
