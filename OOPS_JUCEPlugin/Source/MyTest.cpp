/*
  ==============================================================================

    FM.c
    Created: 23 Jan 2017 9:39:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPSTest.h"
#include "MyTest.h"

void    OOPSTest_init            (float sampleRate)
{
    OOPSInit(sampleRate, &randomNumberGenerator);
    
    poly = tPolyphonicHandlerInit();
    for (int8_t i = 0; i < NUMBER_VOICES; i++)
    {
        sawtooths[i] = tSawtoothInit();
    }
}

int count = 0;

float   OOPSTest_tick            (float input)
{
    float sample = 0;
    for (int8_t i = 0; i < NUMBER_VOICES; i++)
    {
        tMidiNote midiNote = tPolyphonicHandlerGetMidiNote(poly, i);
        float velocityMod = midiNote.velocity / 127.0;
        sample += velocityMod * tSawtoothTick(sawtooths[i]) / NUMBER_VOICES;
    }
    
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
    tPolyphonicHandlerNoteOn(poly, midiNoteNumber, velocity * 127);
    
    for (int8_t i = 0; i < NUMBER_VOICES; i++)
    {
        tMidiNote midiNote = tPolyphonicHandlerGetMidiNote(poly, i);
        tSawtoothSetFreq(sawtooths[i], OOPS_midiToFrequency(midiNote.pitch));
    }
    
//    DBG("note on: " + String(midiNote1.pitch));
//    DBG("note VELOCITY: " + String(midiNote1.velocity));
}



void    OOPSTest_noteOff         (int midiNoteNumber)
{
    tPolyphonicHandlerNoteOff(poly, midiNoteNumber);
    
    for (int8_t i = 0; i < NUMBER_VOICES; i++)
    {
        tMidiNote midiNote = tPolyphonicHandlerGetMidiNote(poly, i);
        tSawtoothSetFreq(sawtooths[i], OOPS_midiToFrequency(midiNote.pitch));
    }
}



void    OOPSTest_end             (void)
{

}
