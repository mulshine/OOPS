/*
  ==============================================================================

    OPPSMemConfig.h
    Created: 23 Jan 2017 10:34:10pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef OPPSMEMCONFIG_H_INCLUDED
#define OPPSMEMCONFIG_H_INCLUDED

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                                       *
 * If your application requires use of many instances of one component or is facing memory limitations,  *
 * use this set of defines to increase or limit the number of instances of each component. The library   *
 * will pre-allocate only the number of instances defined here.                                          *
 *                                                                                                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define SHAPER1_TABLE_SIZE 65536
extern const float shaper1[SHAPER1_TABLE_SIZE];

#define NUM_VOICES 8
#define NUM_SHIFTERS 4
#define MPOLY_NUM_MAX_VOICES 8
#define NUM_OSC 4
#define INV_NUM_OSC (1.0f / NUM_OSC)
#define PS_FRAME_SIZE 1024 // SNAC_FRAME_SIZE in OOPSCore.h should match (or be smaller than?) this
#define ENV_WINDOW_SIZE 1024
#define ENV_HOP_SIZE 256
#define NUM_KNOBS 4

#define     DELAY_LENGTH        16000   // The maximum delay length of all Delay/DelayL/DelayA components.
                                            // Feel free to change to suit memory constraints or desired delay max length / functionality.

#define TALKBOX_BUFFER_LENGTH   1600    // Every talkbox instance introduces 5 buffers of this size

#endif  // OPPSMEMCONFIG_H_INCLUDED
