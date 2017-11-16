/*
  ==============================================================================

    FM.c
    Created: 23 Jan 2017 9:39:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPSTest.h"
#include "MyTest.h"

float gainPerOsc = 1.0f / NUM_VOICES;

void    OOPSTest_init            (float sampleRate)
{
    OOPSInit(sampleRate, &randomNumberGenerator);
    
    poly = tMPoly_init();
    for (int8_t i = 0; i < NUM_VOICES; i++)
    {
        sawtooths[i] = tSawtoothInit();
    }
}

int count = 0;

float   OOPSTest_tick            (float input)
{

    float sample = 0;
    
    for (int8_t i = 0; i < NUM_VOICES; i++)
    {
        sample += tSawtoothTick(sawtooths[i]) * poly->voices[i][1] / 127.0f;
    }
    
    /*
    for (int8_t i = 0; i < NUM_VOICES; i++)
    {
        tMidiNote* midiNote = tPolyGetMidiNote(poly, i);
        float velocityMod = 0;
        if (midiNote != NO_VOICE)
            velocityMod = midiNote->velocity / 127.0;
        sample += velocityMod * tSawtoothTick(sawtooths[i]) / NUMBER_VOICES;
    }
     */
    
    return sample;
}


void    OOPSTest_block           (void)
{

}

void    OOPSTest_controllerInput (int controller, float value)
{

}

void    OOPSTest_pitchBendInput  (int pitchBend)
{

}

void    OOPSTest_noteOn          (int midiNoteNumber, float velocity)
{
    tMPoly_noteOn(poly, midiNoteNumber, (int) (velocity * 127.0f));
    
    for (int i = 0; i < NUM_VOICES; i++)
    {
        int note = poly->voices[i][0];
        
        DBG(String(note) + " " + String(poly->voices[i][1]));
        tSawtoothSetFreq(sawtooths[i], OOPS_midiToFrequency(note));
    }
    /*
    tPolyNoteOn(poly, midiNoteNumber, velocity * 127);
    
    for (int8_t i = 0; i < NUMBER_VOICES; i++)
    {
        tMidiNote* midiNote = tPolyGetMidiNote(poly, i);
        if (midiNote != NULL)
        {
            tSawtoothSetFreq(sawtooths[i], OOPS_midiToFrequency(midiNote->pitch));
            DBG("note on: " + String(midiNote->pitch));
            DBG("note VELOCITY: " + String(midiNote->velocity));
        }
    }
    */
}


void    OOPSTest_noteOff         (int midiNoteNumber)
{
    
    tMPoly_noteOff(poly, midiNoteNumber);
}



void    OOPSTest_end             (void)
{

}
