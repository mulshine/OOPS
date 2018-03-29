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

int hopSize = 64, windowSize = 64;
float max, timeConstant = 100, envout, deltamax, radius;
/**********************************************/

static void setTimeConstant(float tc)
{
    timeConstant = tc;
    radius = exp(-1000.0f * hopSize * oops.invSampleRate / timeConstant);
}

void    OOPSTest_init            (float sampleRate, int samplesPerBlock)
{
    OOPSInit(sampleRate, samplesPerBlock, &randomNumberGenerator);

    /* Initialize devices for pitch shifting */
    snac = tSNAC_init(samplesPerBlock, DEFOVERLAP);
    sola = tSOLAD_init();
    env = tEnvInit(windowSize, hopSize);
    
    hp = tHighpassInit(40.0f);
    
    tSOLAD_setPitchFactor(sola, desPitchRatio);
    
    setTimeConstant(timeConstant);
}



float lastmax;
int fba = 20;
void attackDetect(void)
{
    envout = tEnvTick(env);

    if (envout >= 1.0f)
    {
        lastmax = max;
        if (envout > max)
        {
            max = envout;
        }
        else
        {
            deltamax = envout - max;
            max = max * radius;
        }
        deltamax = max - lastmax;
    }
    
    fba = fba ? (fba-1) : 0;
    
    if (fba == 0 && (max > 60 && deltamax > 6))
    {
        DBG("attack");
        fba = 5;
        tSOLAD_setReadLag(sola, oops.blockSize);
    }
    
}

int count = 0;

float   OOPSTest_tick            (float input)
{
    float sample = 0;
    
    return sample;
}

#define TEST_PS 1

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
    float  period;

    for (int cc=0; cc < numSamples; cc++)
    {
        inBuffer[cur_read_block*numSamples+cc] = inL[cc];
    }
    
    tEnvProcessBlock(env, &inBuffer[cur_read_block*numSamples]);
    attackDetect();

    // tSNAC period detection
#if SNAC
    tSNAC_ioSamples(snac, &inBuffer[cur_read_block*numSamples], &outBuffer[cur_write_block*numSamples], numSamples);

    period = tSNAC_getPeriod(snac);
    if (++counter >= 50)
    {
        counter = 0;
        
        DBG("time  constant: " + String(s1 * 480.0f + 20.0f));
        DBG("pitch factor: " + String(s2 * 3.5f + 0.5f));
    }
#endif
    
    
#if SOLAD
    tSOLAD_setPeriod(sola, period);
    
    tSOLAD_setPitchFactor(sola, s2 * 3.5f + 0.5f);
    setTimeConstant(s1 * 20.0f + 480.0f);
    
    //  tSOLAD pshift works
    tSOLAD_ioSamples(sola, &inBuffer[cur_read_block*numSamples], &outBuffer[cur_write_block*numSamples], numSamples);
#endif
    

    for (int cc=0; cc < numSamples; cc++)
    {
        outL[cc] = tHighpassTick(hp,outBuffer[cur_write_block*numSamples+cc]);
        outR[cc] = outL[cc];
    }
    
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
