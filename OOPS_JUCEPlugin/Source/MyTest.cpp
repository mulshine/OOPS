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

int count  = 0;


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
    
    osc = tCycleInit();
    
    tCycleSetFreq(osc, 220.0f);
    
    fs = tFormantShifterInit();

    folder = tLockhartWavefolderInit();
    
    hp = tHighpassInit(40.0f);
    
    tSOLAD_setPitchFactor(sola, desPitchRatio);
    
    setTimeConstant(timeConstant);
    
    setSliderValue("s1", 0.2f);
    setSliderValue("s2", 0.1f);
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

float porch = 1.0f;
#define PORCH 1.0f        // Threshold for fold actuation
#define STEPS 12      // Number of consecutively applied folds
#define SCREENDOOR 12.0f   // Maximum input gain pre-fold

static float fold1(float sample)
{
    if (sample > PORCH)
    {
        sample = 2.0f * PORCH - sample;
    }
    else if (sample < -PORCH)
    {
        sample = (-2.0f * PORCH) - sample;
    }
    
    return sample;
}

static float fold2(float sample)
{
    if (fabsf(sample) > PORCH)
    {
        sample = sinf(PI * 0.5f * sample);
    }
    
    return sample;
}

static float fold3(float sample)
{
    if (sample > PORCH)
    {
        sample = sinf(PI * cosf(PI * sample)) + 1.0f;
    }
    else if (sample < -PORCH)
    {
        sample = -sinf(PI * cosf(PI * sample)) - 1.0f;
    }
    
    return sample;
}

static float fold4(float sample)
{
    if (sample > porch)
    {
        sample = cosf((PI / 3.0f) * sinf(PI * sinf(PI * 0.5f * sample)));
    }
    else if (sample < -porch)
    {
        sample = -cosf((PI / 3.0f) * sinf(PI * sinf(PI * 0.5f * sample)));
    }
    
    return sample;
}


float   OOPSTest_tick            (float input)
{
    float sample = tCycleTick(osc) * wfGain * SCREENDOOR;
    
    porch = 1.0f;
    for (int i = 0; i < STEPS; i++)
    {
        int fold = i % 4;
        
        switch (fold) {
            case 0:
                sample = fold1(sample);
                break;
            case 1:
                sample = fold2(sample);
                break;
            case 2:
                sample = fold3(sample);
                break;
            case 3:
                sample = fold4(sample);
                break;
                
            default:
                break;
        }
        //porch -= 0.05f;
    }
    
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


}

void    OOPSTest_block           (int numSamples)
{
    float s1 = getSliderValue("GAIN");
    
    wfGain = s1;
    
    float s2 = getSliderValue("s2");
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
