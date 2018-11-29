/*
  ==============================================================================

    OOPSInstrument.c
    Created: 20 Jan 2017 12:01:54pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#if _WIN32 || _WIN64

#include "..\Inc\OOPSInstrument.h"
#include "..\Inc\OOPS.h"

#else

#include "../Inc/OOPSInstrument.h"
#include "../Inc/OOPS.h"

#endif



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void tTalkbox_init(tTalkbox* const v)
{
    v->param[0] = 0.5f;  //wet
    v->param[1] = 0.0f;  //dry
    v->param[2] = 0; // Swap
    v->param[3] = 1.0f;  //quality
    
    tTalkbox_update(v);
}

void tTalkbox_free(tTalkbox* const v)
{
    oops_free(v);
}

void tTalkbox_update(tTalkbox* const v) ///update internal parameters...
{
    float fs = oops.sampleRate;
    if(fs <  8000.0f) fs =  8000.0f;
    if(fs > 96000.0f) fs = 96000.0f;
    
    int32_t n = (int32_t)(0.01633f * fs);
    if(n > TALKBOX_BUFFER_LENGTH) n = TALKBOX_BUFFER_LENGTH;
    
    //O = (VstInt32)(0.0005f * fs);
    v->O = (int32_t)((0.0001f + 0.0004f * v->param[3]) * fs);
    
    if(n != v->N) //recalc hanning window
    {
        v->N = n;
        float dp = TWO_PI / v->N;
        float p = 0.0f;
        for(n=0; n<v->N; n++)
        {
            v->window[n] = 0.5f - 0.5f * cosf(p);
            p += dp;
        }
    }
    v->wet = 0.5f * v->param[0] * v->param[0];
    v->dry = 2.0f * v->param[1] * v->param[1];
}

void tTalkbox_suspend(tTalkbox* const v) ///clear any buffers...
{
    v->pos = v->K = 0;
    v->emphasis = 0.0f;
    v->FX = 0;
    
    v->u0 = v->u1 = v->u2 = v->u3 = v->u4 = 0.0f;
    v->d0 = v->d1 = v->d2 = v->d3 = v->d4 = 0.0f;
    
    for (int32_t i = 0; i < TALKBOX_BUFFER_LENGTH; i++)
    {
        v->buf0[i] = 0;
        v->buf1[i] = 0;
        v->car0[i] = 0;
        v->car1[i] = 0;
    }
}


#define ORD_MAX           100 // Was 50. Increasing this gets rid of glitchiness, lowering it breaks it; not sure how it affects performance
void tTalkbox_lpc(float *buf, float *car, int32_t n, int32_t o)
{
    float z[ORD_MAX], r[ORD_MAX], k[ORD_MAX], G, x;
    int32_t i, j, nn=n;
    
    for(j=0; j<=o; j++, nn--)  //buf[] is already emphasized and windowed
    {
        z[j] = r[j] = 0.0f;
        for(i=0; i<nn; i++) r[j] += buf[i] * buf[i+j]; //autocorrelation
    }
    r[0] *= 1.001f;  //stability fix
    
    float min = 0.00001f;
    if(r[0] < min) { for(i=0; i<n; i++) buf[i] = 0.0f; return; }
    
    tTalkbox_lpcDurbin(r, o, k, &G);  //calc reflection coeffs
    
    for(i=0; i<=o; i++)
    {
        if(k[i] > 0.995f) k[i] = 0.995f; else if(k[i] < -0.995f) k[i] = -.995f;
    }
    
    for(i=0; i<n; i++)
    {
        x = G * car[i];
        for(j=o; j>0; j--)  //lattice filter
        {
            x -= k[j] * z[j-1];
            z[j] = z[j-1] + k[j] * x;
        }
        buf[i] = z[0] = x;  //output buf[] will be windowed elsewhere
    }
}


void tTalkbox_lpcDurbin(float *r, int p, float *k, float *g)
{
    int i, j;
    float a[ORD_MAX], at[ORD_MAX], e=r[0];
    
    for(i=0; i<=p; i++) a[i] = at[i] = 0.0f; //probably don't need to clear at[] or k[]
    
    for(i=1; i<=p; i++)
    {
        k[i] = -r[i];
        
        for(j=1; j<i; j++)
        {
            at[j] = a[j];
            k[i] -= a[j] * r[i-j];
        }
        if(fabs(e) < 1.0e-20f) { e = 0.0f;  break; }
        k[i] /= e; // This might be costing us
        
        a[i] = k[i];
        for(j=1; j<i; j++) a[j] = at[j] + k[i] * at[i-j];
        
        e *= 1.0f - k[i] * k[i];
    }
    
    if(e < 1.0e-20f) e = 0.0f;
    *g = sqrtf(e);
}

float tTalkbox_tick(tTalkbox* const v, float synth, float voice)
{

    int32_t  p0=v->pos, p1 = (v->pos + v->N/2) % v->N;
    float e=v->emphasis, w, o, x, dr, fx=v->FX;
    float p, q, h0=0.3f, h1=0.77f;
    
    o = voice;
    x = synth;
    
    dr = o;
    
    p = v->d0 + h0 *  x; v->d0 = v->d1;  v->d1 = x  - h0 * p;
    q = v->d2 + h1 * v->d4; v->d2 = v->d3;  v->d3 = v->d4 - h1 * q;
    v->d4 = x;
    x = p + q;
    
    if(v->K++)
    {
        v->K = 0;
        
        v->car0[p0] = v->car1[p1] = x; //carrier input
        
        x = o - e;  e = o;  //6dB/oct pre-emphasis
        
        w = v->window[p0]; fx = v->buf0[p0] * w;  v->buf0[p0] = x * w;  //50% overlapping hanning windows
        if(++p0 >= v->N) { tTalkbox_lpc(v->buf0, v->car0, v->N, v->O);  p0 = 0; }
        
        w = 1.0f - w;  fx += v->buf1[p1] * w;  v->buf1[p1] = x * w;
        if(++p1 >= v->N) { tTalkbox_lpc(v->buf1, v->car1, v->N, v->O);  p1 = 0; }
    }
    
    p = v->u0 + h0 * fx; v->u0 = v->u1;  v->u1 = fx - h0 * p;
    q = v->u2 + h1 * v->u4; v->u2 = v->u3;  v->u3 = v->u4 - h1 * q;
    v->u4 = fx;
    x = p + q;
    
    o = x;
    
    v->emphasis = e;
    v->pos = p0;
    v->FX = fx;

    float den = 1.0e-10f; //(float)pow(10.0f, -10.0f * param[4]);
    if(fabs(v->d0) < den) v->d0 = 0.0f; //anti-denormal (doesn't seem necessary but P4?)
    if(fabs(v->d1) < den) v->d1 = 0.0f;
    if(fabs(v->d2) < den) v->d2 = 0.0f;
    if(fabs(v->d3) < den) v->d3 = 0.0f;
    if(fabs(v->u0) < den) v->u0 = 0.0f;
    if(fabs(v->u1) < den) v->u1 = 0.0f;
    if(fabs(v->u2) < den) v->u2 = 0.0f;
    if(fabs(v->u3) < den) v->u3 = 0.0f;
    return o;
}

void tTalkbox_setQuality(tTalkbox* const v, float quality)
{
	v->param[3] = quality;
	v->O = (int32_t)((0.0001f + 0.0004f * v->param[3]) * oops.sampleRate);
}


void   tVocoder_init        (tVocoder* const v)
{
    v->param[0] = 0.33f;  //input select
    v->param[1] = 0.50f;  //output dB
    v->param[2] = 0.40f;  //hi thru
    v->param[3] = 0.40f;  //hi band
    v->param[4] = 0.16f;  //envelope
    v->param[5] = 0.55f;  //filter q
    v->param[6] = 0.6667f;//freq range
    v->param[7] = 0.33f;  //num bands
    
    tVocoder_update(v);
}

void        tVocoder_update      (tVocoder* const v)
{
    float tpofs = 6.2831853f * oops.invSampleRate;
    
    float rr, th, re;
    
    float sh;
    
    int32_t i;
    
    v->gain = (float)pow(10.0f, 2.0f * v->param[1] - 3.0f * v->param[5] - 2.0f);
    
    v->thru = (float)pow(10.0f, 0.5f + 2.0f * v->param[1]);
    v->high =  v->param[3] * v->param[3] * v->param[3] * v->thru;
    v->thru *= v->param[2] * v->param[2] * v->param[2];
    
    if(v->param[7]<0.5f)
    {
        v->nbnd=8;
        re=0.003f;
        v->f[1][2] = 3000.0f;
        v->f[2][2] = 2200.0f;
        v->f[3][2] = 1500.0f;
        v->f[4][2] = 1080.0f;
        v->f[5][2] = 700.0f;
        v->f[6][2] = 390.0f;
        v->f[7][2] = 190.0f;
    }
    else
    {
        v->nbnd=16;
        re=0.0015f;
        v->f[ 1][2] = 5000.0f; //+1000
        v->f[ 2][2] = 4000.0f; //+750
        v->f[ 3][2] = 3250.0f; //+500
        v->f[ 4][2] = 2750.0f; //+450
        v->f[ 5][2] = 2300.0f; //+300
        v->f[ 6][2] = 2000.0f; //+250
        v->f[ 7][2] = 1750.0f; //+250
        v->f[ 8][2] = 1500.0f; //+250
        v->f[ 9][2] = 1250.0f; //+250
        v->f[10][2] = 1000.0f; //+250
        v->f[11][2] =  750.0f; //+210
        v->f[12][2] =  540.0f; //+190
        v->f[13][2] =  350.0f; //+155
        v->f[14][2] =  195.0f; //+100
        v->f[15][2] =   95.0f;
    }
    
    if(v->param[4]<0.05f) //freeze
    {
        for(i=0;i<v->nbnd;i++) v->f[i][12]=0.0f;
    }
    else
    {
        v->f[0][12] = (float)pow(10.0, -1.7 - 2.7f * v->param[4]); //envelope speed
        
        rr = 0.022f / (float)v->nbnd; //minimum proportional to frequency to stop distortion
        for(i=1;i<v->nbnd;i++)
        {
            v->f[i][12] = (float)(0.025 - rr * (double)i);
            if(v->f[0][12] < v->f[i][12]) v->f[i][12] = v->f[0][12];
        }
        v->f[0][12] = 0.5f * v->f[0][12]; //only top band is at full rate
    }
    
    rr = 1.0 - pow(10.0f, -1.0f - 1.2f * v->param[5]);
    sh = (float)pow(2.0f, 3.0f * v->param[6] - 1.0f); //filter bank range shift
    
    for(i=1;i<v->nbnd;i++)
    {
        v->f[i][2] *= sh;
        th = acos((2.0 * rr * cos(tpofs * v->f[i][2])) / (1.0 + rr * rr));
        v->f[i][0] = (float)(2.0 * rr * cos(th)); //a0
        v->f[i][1] = (float)(-rr * rr);           //a1
        //was .98
        v->f[i][2] *= 0.96f; //shift 2nd stage slightly to stop high resonance peaks
        th = acos((2.0 * rr * cos(tpofs * v->f[i][2])) / (1.0 + rr * rr));
        v->f[i][2] = (float)(2.0 * rr * cos(th));
    }
}

float       tVocoder_tick        (tVocoder* const v, float synth, float voice)
{
    float a, b, o=0.0f, aa, bb, oo = v->kout, g = v->gain, ht = v->thru, hh = v->high, tmp;
    uint32_t i, k = v->kval, nb = v->nbnd;

    a = voice; //speech
    b = synth; //synth
    
    tmp = a - v->f[0][7]; //integrate modulator for HF band and filter bank pre-emphasis
    v->f[0][7] = a;
    a = tmp;
    
    if(tmp<0.0f) tmp = -tmp;
    v->f[0][11] -= v->f[0][12] * (v->f[0][11] - tmp);      //high band envelope
    o = v->f[0][11] * (ht * a + hh * (b - v->f[0][3])); //high band + high thru
    
    v->f[0][3] = b; //integrate carrier for HF band
    
    if(++k & 0x1) //this block runs at half sample rate
    {
        oo = 0.0f;
        aa = a + v->f[0][9] - v->f[0][8] - v->f[0][8];  //apply zeros here instead of in each reson
        v->f[0][9] = v->f[0][8];  v->f[0][8] = a;
        bb = b + v->f[0][5] - v->f[0][4] - v->f[0][4];
        v->f[0][5] = v->f[0][4];  v->f[0][4] = b;
        
        for(i=1; i<nb; i++) //filter bank: 4th-order band pass
        {
            tmp = v->f[i][0] * v->f[i][3] + v->f[i][1] * v->f[i][4] + bb;
            v->f[i][4] = v->f[i][3];
            v->f[i][3] = tmp;
            tmp += v->f[i][2] * v->f[i][5] + v->f[i][1] * v->f[i][6];
            v->f[i][6] = v->f[i][5];
            v->f[i][5] = tmp;
            
            tmp = v->f[i][0] * v->f[i][7] + v->f[i][1] * v->f[i][8] + aa;
            v->f[i][8] = v->f[i][7];
            v->f[i][7] = tmp;
            tmp += v->f[i][2] * v->f[i][9] + v->f[i][1] * v->f[i][10];
            v->f[i][10] = v->f[i][9];
            v->f[i][9] = tmp;
            
            if(tmp<0.0f) tmp = -tmp;
            v->f[i][11] -= v->f[i][12] * (v->f[i][11] - tmp);
            oo += v->f[i][5] * v->f[i][11];
        }
    }
    o += oo * g; //effect of interpolating back up to Fs would be minimal (aliasing >16kHz)

    v->kout = oo;
    v->kval = k & 0x1;
    if(fabs(v->f[0][11])<1.0e-10) v->f[0][11] = 0.0f; //catch HF envelope denormal
    
    for(i=1;i<nb;i++)
        if(fabs(v->f[i][3])<1.0e-10 || fabs(v->f[i][7])<1.0e-10)
            for(k=3; k<12; k++) v->f[i][k] = 0.0f; //catch reson & envelope denormals
    
    if(fabs(o)>10.0f) tVocoder_suspend(v); //catch instability
    
    return o;
    
}

void        tVocoder_suspend     (tVocoder* const v)
{
    int32_t i, j;
    
    for(i=0; i<v->nbnd; i++) for(j=3; j<12; j++) v->f[i][j] = 0.0f; //zero band filters and envelopes
    v->kout = 0.0f;
    v->kval = 0;
}


/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ tPluck ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
void    tPluck_init         (tPluck* const p, float lowestFrequency)
{
    if ( lowestFrequency <= 0.0f )  lowestFrequency = 10.0f;
    
    p->noise = (tNoise*)oops_alloc(sizeof(tNoise));
    tNoise_init(p->noise, WhiteNoise);
    

    p->pickFilter = (tOnePole*) oops_alloc(sizeof(tOnePole));
    tOnePole_init(p->pickFilter, 0.0f);
    
    p->loopFilter = (tOneZero*) oops_alloc(sizeof(tOneZero));
    tOneZero_init(p->loopFilter, 0.0f);
    
    p->delayLine = (tDelayA*) oops_alloc(sizeof(tDelayA));
    tDelayA_init(p->delayLine, 0.0f, oops.sampleRate * 2);
    
    tPluck_setFrequency(p, 220.0f);
}

float   tPluck_getLastOut    (tPluck *p)
{
    return p->lastOut;
}

float   tPluck_tick          (tPluck *p)
{
    return (p->lastOut = 3.0f * tDelayA_tick(p->delayLine, tOneZero_tick(p->loopFilter, tDelayA_getLastOut(p->delayLine) * p->loopGain ) ));
}

void    tPluck_pluck         (tPluck* const p, float amplitude)
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    tOnePole_setPole(p->pickFilter, 0.999f - (amplitude * 0.15f));
    tOnePole_setGain(p->pickFilter, amplitude * 0.5f );
    
    // Fill delay with noise additively with current contents.
    for ( uint32_t i = 0; i < (uint32_t)tDelayA_getDelay(p->delayLine); i++ )
        tDelayA_tick(p->delayLine, 0.6f * tDelayA_getLastOut(p->delayLine) + tOnePole_tick(p->pickFilter, tNoise_tick(p->noise) ) );
}

// Start a note with the given frequency and amplitude.;
void    tPluck_noteOn        (tPluck* const p, float frequency, float amplitude )
{
    p->lastFreq = frequency;
    tPluck_setFrequency( p, frequency );
    tPluck_pluck( p, amplitude );
}

// Stop a note with the given amplitude (speed of decay).
void    tPluck_noteOff       (tPluck* const p, float amplitude )
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->loopGain = 1.0f - amplitude;
}

// Set instrument parameters for a particular frequency.
void    tPluck_setFrequency  (tPluck* const p, float frequency )
{
    if ( frequency <= 0.0f )   frequency = 0.001f;
    
    // Delay = length - filter delay.
    float delay = ( oops.sampleRate / frequency ) - tOneZero_getPhaseDelay(p->loopFilter, frequency );
    
    tDelayA_setDelay(p->delayLine, delay );
    
    p->loopGain = 0.99f + (frequency * 0.000005f);
    
    if ( p->loopGain >= 0.999f ) p->loopGain = 0.999f;
    
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tPluck_controlChange (tPluck* const p, int number, float value)
{
    return;
}

void tPluckSampleRateChanged(tPluck* const p)
{
    //tPluckSetFrequency(p, p->lastFreq);
}

/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ tStifKarp ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
void    tStifKarp_init          (tStifKarp* const p, float lowestFrequency)
{
    if ( lowestFrequency <= 0.0f )  lowestFrequency = 8.0f;
    
    p->delayLine = (tDelayA*) oops_alloc(sizeof(tDelayA));
    tDelayA_init(p->delayLine, 0.0f, oops.sampleRate * 2);
    
    p->combDelay = (tDelayL*) oops_alloc(sizeof(tDelayL));
    tDelayL_init(p->combDelay, 0.0f, oops.sampleRate * 2);
    
    p->filter = (tOneZero*) oops_alloc(sizeof(tOneZero));
    tOneZero_init(p->filter, 0.0f);
    
    p->noise = (tNoise*) oops_alloc(sizeof(tNoise));
    tNoise_init(p->noise, WhiteNoise);
    
    for (int i = 0; i < 4; i++)
    {
        p->biquad[i] = (tBiQuad*) oops_alloc(sizeof(tBiQuad));
        tBiQuad_init(p->biquad[i]);
    }

    p->pluckAmplitude = 0.3f;
    p->pickupPosition = 0.4f;
    
    p->stretching = 0.9999f;
    p->baseLoopGain = 0.995f;
    p->loopGain = 0.999f;
    
    tStifKarp_setFrequency( p, 220.0f );

}

void tStifKarp_free (tStifKarp* const p)
{
    tDelayA_free(p->delayLine);
    tDelayL_free(p->combDelay);
    tOneZero_free(p->filter);
    tNoise_free(p->noise);

    for (int i = 0; i < 4; i++)
    {
        tBiQuad_free(p->biquad[i]);
    }
    
    oops_free(p);
}

float   tStifKarp_getLastOut    (tStifKarp* const p)
{
    return p->lastOut;
}

float   tStifKarp_tick          (tStifKarp* const p)
{
    float temp = tDelayA_getLastOut(p->delayLine) * p->loopGain;
    
    // Calculate allpass stretching.
    for (int i=0; i<4; i++)     temp = tBiQuad_tick(p->biquad[i],temp);
    
    // Moving average filter.
    temp = tOneZero_tick(p->filter, temp);
    
    float out = tDelayA_tick(p->delayLine, temp);
    out = out - tDelayL_tick(p->combDelay, out);
    p->lastOut = out;
    
    return p->lastOut;
}

void    tStifKarp_pluck         (tStifKarp* const p, float amplitude)
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->pluckAmplitude = amplitude;
    
    for ( uint32_t i=0; i < (uint32_t)tDelayA_getDelay(p->delayLine); i++ )
    {
        // Fill delay with noise additively with current contents.
        tDelayA_tick(p->delayLine, (tDelayA_getLastOut(p->delayLine) * 0.6f) + 0.4f * tNoise_tick(p->noise) * p->pluckAmplitude );
        //delayLine_.tick( combDelay_.tick((delayLine_.lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude_) );
    }
}

// Start a note with the given frequency and amplitude.;
void    tStifKarp_noteOn        (tStifKarp* const p, float frequency, float amplitude )
{
    tStifKarp_setFrequency( p, frequency );
    tStifKarp_pluck( p, amplitude );
}

// Stop a note with the given amplitude (speed of decay).
void    tStifKarp_noteOff       (tStifKarp* const p, float amplitude )
{
    if ( amplitude < 0.0f)      amplitude = 0.0f;
    else if (amplitude > 1.0f)  amplitude = 1.0f;
    
    p->loopGain = 1.0f - amplitude;
}

// Set instrument parameters for a particular frequency.
void    tStifKarp_setFrequency  (tStifKarp* const p, float frequency )
{
    if ( frequency <= 0.0f )   frequency = 0.001f;
    
    p->lastFrequency = frequency;
    p->lastLength = oops.sampleRate / p->lastFrequency;
    float delay = p->lastLength - 0.5f;
    tDelayA_setDelay(p->delayLine, delay );
    
    // MAYBE MODIFY LOOP GAINS
    p->loopGain = p->baseLoopGain + (frequency * 0.000005f);
    if (p->loopGain >= 1.0f) p->loopGain = 0.99999f;
    
    tStifKarp_setStretch(p, p->stretching);
    
    tDelayL_setDelay(p->combDelay, 0.5f * p->pickupPosition * p->lastLength );
    
}

// Set the stretch "factor" of the string (0.0 - 1.0).
void    tStifKarp_setStretch         (tStifKarp* const p, float stretch )
{
    p->stretching = stretch;
    float coefficient;
    float freq = p->lastFrequency * 2.0f;
    float dFreq = ( (0.5f * oops.sampleRate) - freq ) * 0.25f;
    float temp = 0.5f + (stretch * 0.5f);
    if ( temp > 0.9999f ) temp = 0.9999f;
    
    for ( int i=0; i<4; i++ )
    {
        coefficient = temp * temp;
        tBiQuad_setA2(p->biquad[i], coefficient);
        tBiQuad_setB0(p->biquad[i], coefficient);
        tBiQuad_setB2(p->biquad[i], 1.0f);
        
        coefficient = -2.0f * temp * cos(TWO_PI * freq / oops.sampleRate);
        tBiQuad_setA1(p->biquad[i], coefficient);
        tBiQuad_setB1(p->biquad[i], coefficient);
        
        freq += dFreq;
    }
}

// Set the pluck or "excitation" position along the string (0.0 - 1.0).
void    tStifKarp_setPickupPosition  (tStifKarp* const p, float position )
{
    if (position < 0.0f)        p->pickupPosition = 0.0f;
    else if (position <= 1.0f)  p->pickupPosition = position;
    else                        p->pickupPosition = 1.0f;
    
    tDelayL_setDelay(p->combDelay, 0.5f * p->pickupPosition * p->lastLength);
}

// Set the base loop gain.
void    tStifKarp_setBaseLoopGain    (tStifKarp* const p, float aGain )
{
    p->baseLoopGain = aGain;
    p->loopGain = p->baseLoopGain + (p->lastFrequency * 0.000005f);
    if ( p->loopGain > 0.99999f ) p->loopGain = 0.99999f;
}

// Perform the control change specified by \e number and \e value (0.0 - 128.0).
void    tStifKarp_controlChange (tStifKarp* const p, SKControlType type, float value)
{
    if ( value < 0.0f )         value = 0.0f;
    else if (value > 128.0f)   value = 128.0f;
    
    float normalizedValue = value * INV_128;
    
    if (type == SKPickPosition) // 4
        tStifKarp_setPickupPosition( p, normalizedValue );
    else if (type == SKStringDamping) // 11
        tStifKarp_setBaseLoopGain( p, 0.97f + (normalizedValue * 0.03f) );
    else if (type == SKDetune) // 1
        tStifKarp_setStretch( p, 0.91f + (0.09f * (1.0f - normalizedValue)) );
    
}

void    tStifKarpSampleRateChanged (tStifKarp* const c)
{
    tStifKarp_setFrequency(c, c->lastFrequency);
    tStifKarp_setStretch(c, c->stretching);
}

#define USE_STICK 0
void t808Cowbell_on(t808Cowbell* const cowbell, float vel)
{
    
    tEnvelope_on(&cowbell->envGain, vel);
    
#if USE_STICK
    tEnvelope_on(&cowbell->envStick,vel);
#endif
    
}

float t808Cowbell_tick(t808Cowbell* const cowbell) {
    
    float sample = 0.0f;
    
    // Mix oscillators.
    sample = (cowbell->oscMix * tSquare_tick(&cowbell->p[0])) + ((1.0f-cowbell->oscMix) * tSquare_tick(&cowbell->p[1]));
    
    // Filter dive and filter.
    tSVF_setFreq(&cowbell->bandpassOsc, cowbell->filterCutoff + 1000.0f * tEnvelope_tick(&cowbell->envFilter));
    sample = tSVF_tick(&cowbell->bandpassOsc,sample);
    
    sample *= (0.9f * tEnvelope_tick(&cowbell->envGain));
    
#if USE_STICK
    sample += (0.1f * tEnvelope_tick(&cowbell->envStick) * tSVF_tick(&cowbell->bandpassStick, tNoise_tick(&cowbell->stick)));
#endif
    
    sample = tHighpass_tick(&cowbell->highpass, sample);
    
    return sample;
}

void t808Cowbell_setDecay(t808Cowbell* const cowbell, float decay)
{
    tEnvelope_setDecay(&cowbell->envGain,decay);
}

void t808Cowbell_setHighpassFreq(t808Cowbell *cowbell, float freq)
{
    tHighpass_setFreq(&cowbell->highpass,freq);
}

void t808Cowbell_setBandpassFreq(t808Cowbell* const cowbell, float freq)
{
    cowbell->filterCutoff = freq;
}

void t808Cowbell_setFreq(t808Cowbell* const cowbell, float freq)
{
    
    tSquare_setFreq(&cowbell->p[0],freq);
    tSquare_setFreq(&cowbell->p[1],1.48148f*freq);
}

void t808Cowbell_setOscMix(t808Cowbell* const cowbell, float oscMix)
{
    cowbell->oscMix = oscMix;
}

void t808Cowbell_init(t808Cowbell* const cowbell) {
    
    tSquare_init(&cowbell->p[0]);
    tSquare_setFreq(&cowbell->p[0], 540.0f);
    
    tSquare_init(&cowbell->p[1]);
    tSquare_setFreq(&cowbell->p[1], 1.48148f * 540.0f);
    
    cowbell->oscMix = 0.5f;
    
    tSVF_init(&cowbell->bandpassOsc, SVFTypeBandpass, 2500, 1.0f);
    
    tSVF_init(&cowbell->bandpassStick, SVFTypeBandpass, 1800, 1.0f);
    
    tEnvelope_init(&cowbell->envGain, 5.0f, 100.0f, OFALSE);
    
    tEnvelope_init(&cowbell->envFilter, 5.0, 100.0f, OFALSE);
    
    tHighpass_init(&cowbell->highpass, 1000.0f);
    
#if USE_STICK
    tNoise_init(&cowbell->stick, NoiseTypeWhite);
    tEnvelope_init(&cowbell->envStick, 5.0f, 5.0f, 0);
#endif
}

void t808Hihat_on(t808Hihat* const hihat, float vel) {
    
    tEnvelope_on(&hihat->envGain, vel);
    tEnvelope_on(&hihat->envStick, vel);
    
}

void t808Hihat_setOscNoiseMix(t808Hihat* const hihat, float oscNoiseMix) {
    
    hihat->oscNoiseMix = oscNoiseMix;
    
}

float t808Hihat_tick(t808Hihat* const hihat) {
    
    float sample = 0.0f;
    float gainScale = 0.1666f;
    
    for (int i = 0; i < 6; i++)
    {
        sample += tSquare_tick(&hihat->p[i]);
    }
    
    sample *= gainScale;
    
    sample = (hihat->oscNoiseMix * sample) + ((1.0f-hihat->oscNoiseMix) * (0.8f * tNoise_tick(&hihat->n)));
    
    sample = tSVF_tick(&hihat->bandpassOsc, sample);
    
    sample *= tEnvelope_tick(&hihat->envGain);
    
    sample = 0.85f * OOPS_clip(0.0f, tHighpass_tick(&hihat->highpass, sample), 1.0f);
    
    sample += 0.15f * tEnvelope_tick(&hihat->envStick) * tSVF_tick(&hihat->bandpassStick, tNoise_tick(&hihat->stick));
    
    return sample;
}

void t808Hihat_setDecay(t808Hihat* const hihat, float decay)
{
    tEnvelope_setDecay(&hihat->envGain,decay);
}

void t808Hihat_setHighpassFreq(t808Hihat* const hihat, float freq)
{
    tHighpass_setFreq(&hihat->highpass,freq);
}

void t808Hihat_setOscBandpassFreq(t808Hihat* const hihat, float freq)
{
    tSVF_setFreq(&hihat->bandpassOsc,freq);
}


void t808Hihat_setOscFreq(t808Hihat* const hihat, float freq)
{
    tSquare_setFreq(&hihat->p[0], 2.0f * freq);
    tSquare_setFreq(&hihat->p[1], 3.00f * freq);
    tSquare_setFreq(&hihat->p[2], 4.16f * freq);
    tSquare_setFreq(&hihat->p[3], 5.43f * freq);
    tSquare_setFreq(&hihat->p[4], 6.79f * freq);
    tSquare_setFreq(&hihat->p[5], 8.21f * freq);
    
}

void t808Hihat_init(t808Hihat* const hihat)
{
    for (int i = 0; i < 6; i++)
    {
        tSquare_init(&hihat->p[i]);
    }
    
    tNoise_init(&hihat->stick, WhiteNoise);
    tNoise_init(&hihat->n, WhiteNoise);
    
    // need to fix SVF to be generic
    tSVF_init(&hihat->bandpassStick, SVFTypeBandpass,2500.0,1.5f);
    tSVF_init(&hihat->bandpassOsc, SVFTypeBandpass,3500,0.5f);
    
    tEnvelope_init(&hihat->envGain, 5.0f, 50.0f, OFALSE);
    tEnvelope_init(&hihat->envStick, 5.0f, 15.0f, OFALSE);
    
    tHighpass_init(&hihat->highpass, 7000.0f);
    
    float freq = 40.0f;
    
    tSquare_setFreq(&hihat->p[0], 2.0f * freq);
    tSquare_setFreq(&hihat->p[1], 3.00f * freq);
    tSquare_setFreq(&hihat->p[2], 4.16f * freq);
    tSquare_setFreq(&hihat->p[3], 5.43f * freq);
    tSquare_setFreq(&hihat->p[4], 6.79f * freq);
    tSquare_setFreq(&hihat->p[5], 8.21f * freq);
}

void t808Snare_on(t808Snare* const snare, float vel)
{
    for (int i = 0; i < 2; i++)
    {
        tEnvelope_on(&snare->toneEnvOsc[i], vel);
        tEnvelope_on(&snare->toneEnvGain[i], vel);
        tEnvelope_on(&snare->toneEnvFilter[i], vel);
    }
    
    tEnvelope_on(&snare->noiseEnvGain, vel);
    tEnvelope_on(&snare->noiseEnvFilter, vel);
}

void t808Snare_setTone1Freq(t808Snare* const snare, float freq)
{
    snare->tone1Freq = freq;
    tTriangle_setFreq(&snare->tone[0], freq);
    
}

void t808Snare_setTone2Freq(t808Snare* const snare, float freq)
{
    snare->tone2Freq = freq;
    tTriangle_setFreq(&snare->tone[1],freq);
}

void t808Snare_setTone1Decay(t808Snare* const snare, float decay)
{
    tEnvelope_setDecay(&snare->toneEnvGain[0],decay);
}

void t808Snare_setTone2Decay(t808Snare* const snare, float decay)
{
    tEnvelope_setDecay(&snare->toneEnvGain[1],decay);
}

void t808Snare_setNoiseDecay(t808Snare* const snare, float decay)
{
    tEnvelope_setDecay(&snare->noiseEnvGain,decay);
}

void t808Snare_setToneNoiseMix(t808Snare* const snare, float toneNoiseMix)
{
    snare->toneNoiseMix = toneNoiseMix;
}

void t808Snare_setNoiseFilterFreq(t808Snare* const snare, float noiseFilterFreq)
{
    snare->noiseFilterFreq = noiseFilterFreq;
}

void t808Snare_setNoiseFilterQ(t808Snare* const snare, float noiseFilterQ)
{
    tSVF_setQ(&snare->noiseLowpass, noiseFilterQ);
}


static float tone[2];

float t808Snare_tick(t808Snare* const snare)
{
    for (int i = 0; i < 2; i++)
    {
        tTriangle_setFreq(&snare->tone[i], snare->tone1Freq + (50.0f * tEnvelope_tick(&snare->toneEnvOsc[i])));
        tone[i] = tTriangle_tick(&snare->tone[i]);
        
        tSVF_setFreq(&snare->toneLowpass[i], 2000 + (500 * tEnvelope_tick(&snare->toneEnvFilter[i])));
        tone[i] = tSVF_tick(&snare->toneLowpass[i], tone[i]) * tEnvelope_tick(&snare->toneEnvGain[i]);
    }
    
    float noise = tNoise_tick(&snare->noiseOsc);
    tSVF_setFreq(&snare->noiseLowpass, snare->noiseFilterFreq +(500 * tEnvelope_tick(&snare->noiseEnvFilter)));
    noise = tSVF_tick(&snare->noiseLowpass, noise) * tEnvelope_tick(&snare->noiseEnvGain);
    
    float sample = (snare->toneNoiseMix)*(tone[0] * snare->toneGain[0] + tone[1] * snare->toneGain[1]) + (1.0f-snare->toneNoiseMix) * (noise * snare->noiseGain);
    
    return sample;
}

void t808Snare_init(t808Snare* const snare)
{
    float ratio[2] = {1.0, 1.5};
    for (int i = 0; i < 2; i++)
    {
        tTriangle_init(&snare->tone[i]);
        tTriangle_setFreq(&snare->tone[i], ratio[i] * 400.0f);
        tSVF_init(&snare->toneLowpass[i], SVFTypeLowpass, 2000, 1.0f);
        tEnvelope_init(&snare->toneEnvOsc[i], 3.0f, 20.0f, OFALSE);
        tEnvelope_init(&snare->toneEnvGain[i], 10.0f, 200.0f, OFALSE);
        tEnvelope_init(&snare->toneEnvFilter[i], 3.0f, 200.0f, OFALSE);
        
        snare->toneGain[i] = 0.5f;
    }
    
    
    tNoise_init(&snare->noiseOsc, WhiteNoise);
    tSVF_init(&snare->noiseLowpass, SVFTypeLowpass, 2000, 3.0f);
    tEnvelope_init(&snare->noiseEnvGain, 10.0f, 125.0f, OFALSE);
    tEnvelope_init(&snare->noiseEnvFilter, 3.0f, 100.0f, OFALSE);
    snare->noiseGain = 0.3f;
}

void        t808Snare_free                  (t808Snare* const snare)
{
    for (int i = 0; i < 2; i++)
    {
        tTriangle_free(&snare->tone[i]);
        tSVF_free(&snare->toneLowpass[i]);
        tEnvelope_free(&snare->toneEnvOsc[i]);
        tEnvelope_free(&snare->toneEnvGain[i]);
        tEnvelope_free(&snare->toneEnvFilter[i]);
    }
    
    
    tNoise_free(&snare->noiseOsc);
    tSVF_free(&snare->noiseLowpass);
    tEnvelope_free(&snare->noiseEnvGain);
    tEnvelope_free(&snare->noiseEnvFilter);
}


