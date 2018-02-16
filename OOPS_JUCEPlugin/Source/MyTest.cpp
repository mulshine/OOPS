/*
  ==============================================================================

    FM.c
    Created: 23 Jan 2017 9:39:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPSTest.h"
#include "MyTest.h"

/* Variables used in ProcessBlock() *************/
float left_samples[MICROBLOCK_LEN/2] = {0};
float right_samples[MICROBLOCK_LEN/2] = {0};
float null_buffer[MICROBLOCK_LEN/2] = {0};
float solad_out_right[MICROBLOCK_LEN/2] = {0};
float solad_out_left[MICROBLOCK_LEN/2] = {0};
int cur_read_block = 2, cur_write_block = 0;

float desPitchRatio = 2.0;
/**********************************************/

void    OOPSTest_init            (float sampleRate, int samplesPerBlock)
{
    OOPSInit(sampleRate, &randomNumberGenerator);

    /* Initialize devices for pitch shifting */
    snac = tSNAC_init(samplesPerBlock, DEFOVERLAP);
    sola = tSOLAD_init();
    atk = tAtkDtk_init(samplesPerBlock);
    
    osc = tCycleInit();
    tCycleSetFreq(osc, 220.0f);
    
    tSOLAD_setPitchFactor(sola, desPitchRatio);
}

int count = 0;

float   OOPSTest_tick            (float input)
{

    float sample = 0;

    return sample;
}

#define TEST_PS 1

static void DPS_pitchshift(float* in, float* out, int numSamples)
{
    int cc, i = 0;
    int atk_hit = 0;
    float req_readlag = 0;
    
    // first separate channels
    for(cc=0; cc < MICROBLOCK_LEN; cc+=2){
        left_samples[cc/2] = (int16_t)in[cc];
    }
    
    for(cc=0; cc < MICROBLOCK_LEN; cc+=2){
        right_samples[cc/2+1] = (int16_t)in[cc];
    }
    
    tSNAC_ioSamples(snac, left_samples, null_buffer, MICROBLOCK_LEN/2);
    
    // process left channel
    tSOLAD_setPeriod(sola, tSNAC_getPeriod(snac));
    
    //Attack detection - find where in 1024 attack occurs
    while(atk_hit != 1 && i < 16){
        atk_hit = tAtkDtk_detect(atk, &left_samples[i*64]);
        ++i;
    }
    
    //print attack to determine if attack detector is working (can be commented out if not required)
    printf("Attack at: %d\r\n", i*64);
    
    // i<16 indicates there was an attack in the input buffer
    if(i < 16){
        req_readlag = i*64; //Try to shift readlag pointer back 128 samples from where attack was detected
        tSOLAD_setReadLag(sola, req_readlag);
    }
    
    for (cc=0; cc < MICROBLOCK_LEN; cc+=2)
    {
        //int16_t in_datum=(int16_t)solad_out_left[cc/2];
        int16_t in_datum = (int16_t)left_samples[cc/2];
        int out_datum = in_datum;
        
        
        out[cc]= (uint16_t)out_datum;
    }
    
    // now the right channel
    tSOLAD_setPitchFactor(sola, 2.0f);
    tSOLAD_ioSamples(sola, left_samples, solad_out_right, MICROBLOCK_LEN/2);
    
    for (cc=1; cc < MICROBLOCK_LEN; cc+=2)
    {
        int16_t in_datum=(int16_t)solad_out_right[(cc-1)/2]; //To output solad pitch shifted output
        //int16_t in_datum = (int16_t)right_samples[(cc-1)/2]; //To ignore solad
        //int16_t in_datum = 0; //To output silence
        
        int out_datum= in_datum;
        out[cc]= (uint16_t)out_datum;
    }
}

float samp;
int ind;

int counter = 0;

#define SINE_IN 0
#define SINE_OUT 0
#define SNAC 1
#define SOLAD 1

void    OOPSTest_block           (float* inL, float* inR, float* outL, float* outR, int numSamples)
{
    float s1 = getSliderValue("s1");
    float s2 = getSliderValue("s2");
    float samp = 0.0f, period = 50.0f, sine_freq, sine_period;
    
#if SINE_IN
    for (int cc=0; cc < numSamples; cc++)
    {
        samp = tCycleTick(osc);
        inBuffer[cur_read_block*numSamples+(2*cc)] = samp;
        inBuffer[cur_read_block*numSamples+(2*cc)+1] = samp;
    }
    
    sine_freq = s1 * 8000.0f + 40.0f;
    sine_period = (1 / sine_freq)  * 44100.0f;
    tCycleSetFreq(osc, sine_freq);
    setSliderModelValue("s1", sine_freq);
#else
    for (int cc=0; cc < numSamples; cc++)
    {
        inBuffer[cur_read_block*numSamples+(2*cc)] = inL[cc];
        inBuffer[cur_read_block*numSamples+(2*cc)+1] = inR[cc];
    }
#endif
    
    // DPS_pitchshift(&inBuffer[cur_read_block*MICROBLOCK_LEN], &outBuffer[cur_write_block*MICROBLOCK_LEN], MICROBLOCK_LEN);
    // tSNAC period detection
#if SNAC
    tSNAC_ioSamples(snac, &inBuffer[cur_read_block*numSamples], &outBuffer[cur_write_block*numSamples], 2*numSamples);

    period = tSNAC_getPeriod(snac);
    if (++counter >= 50)
    {
        counter = 0;
        DBG("expected period: " + String(sine_period));
        DBG("snac     period: " + String(period));
        DBG("pitch    factor: " + String(s2 * 3.5f + 0.5f));
    }
#endif
    
    
#if SOLAD
    tSOLAD_setPeriod(sola, period);
    
    tSOLAD_setPitchFactor(sola, 2.0f);
    setSliderModelValue("s2", s2 * 3.5f + 0.5f);
    
    //  tSOLAD pshift works
    tSOLAD_ioSamples(sola, &inBuffer[cur_read_block*numSamples], &outBuffer[cur_write_block*numSamples], 2*numSamples);
#endif
    
#if SINE_OUT
    for (int cc=0; cc < numSamples; cc++)
    {
        outL[cc] = inBuffer[cur_read_block*numSamples+(2*cc)];
        outR[cc] = inBuffer[cur_read_block*numSamples+(2*cc)+1];
    }
#else
    for (int cc=0; cc < numSamples; cc++)
    {
        /*
        outL[cc] = outBuffer[cur_write_block*MICROBLOCK_LEN+(2*cc)];
        outR[cc] = outBuffer[cur_write_block*MICROBLOCK_LEN+(2*cc)+1];
         */
        outL[cc] = outBuffer[cur_write_block*numSamples+(2*cc)];
        outR[cc] = outBuffer[cur_write_block*numSamples+(2*cc)+1];
    }
#endif
    
    cur_read_block++;
    if (cur_read_block >= TOTAL_BUFFERS)
        cur_read_block=0;
    
    cur_write_block++;
    if (cur_write_block >= TOTAL_BUFFERS)
        cur_write_block=0;
}

void    OOPSTest_controllerInput (int controller, float value)
{

}

void    OOPSTest_pitchBendInput  (int pitchBend)
{

}

void    OOPSTest_noteOn          (int midiNoteNumber, float velocity)
{

}


void    OOPSTest_noteOff         (int midiNoteNumber)
{

}



void    OOPSTest_end             (void)
{

}
