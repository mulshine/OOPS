# OOPS
A C library for Audio synthesis and processing, written using semi-OOP style, created by Mike Mulshine and Jeff Snyder. 

The library consists of a set of high-level audio synthesis components (Oscillators, Filters, Envelopes, Delays, Reverbs, and other Utilities) and a central registry system, the OOPS Core, which manages components and provides access to important system-wide audio settings like the sample rate. Components are all statically allocated (1) at compile-time as part of the OOPS core. 

Our primary use case is embedded audio computing on 32-bit ARM microcontrollers that can run "bare-metal" (without an OS), such as the STM32f4 and STM32f7. The code, however, is general enough to be used in many other situations as well. We have included a JUCE VST/AU generating template to test the library (2), and the python script we use to generate wavetables. 

Most of these algorithms are sourced from other projects, especially the STK (Sound Toolkit) library and various discussions on the music-DSP mailing list. We also owe a lot to open source computer programming languages, such as C-sound, ChucK, PureData, and Supercollider. 

Other interesting projects to check out that similarly target embedded applicatons are: TeensyAudio (C++), Hoxton Owl (C++), Axoloti (C), and Mutable Instruments (C++). 

(1) Dynamic allocation is avoided to meet the needs of a larger number of embedded applications. Embedded developers often utilize architectures that don't fully support stdlib memory allocation functions like calloc, malloc, and free.

(2) The template features an easily reconfigurable UI and simple block and tick setup to test the library components. Of course, if you intend to use the provided JUCE plugin project, you need to get JUCE and the Projucer ( https://www.juce.com/get-juce ). Check out the first tutorial to get started - it's fun an easy! If you intend to include the OOPS framework in your own C++ project using JUCE or other platforms, you will probably need to rename each source file from .c to .cpp. This should be straightforward. Contact the developer if you have any troubles (mrmulshine@gmail.com).

Documentation: 

[embed]http://plork.princeton.edu/OOPS_documentation.pdf[/embed]

Usage:

To begin using OOPS, the developer should add the library source to their project and include OOPS.h in a convenient header  file.   The  OOPS  core  needs  to  be  initialized  with a  call  to  OOPSInit(),  which  takes  as  arguments  the  desired system sample rate and a pointer to a random number generating function.  Developers using OOPS should create pointers to audio components and assign to them the return value  of  their  associated  initialization  functions.   At  that point, they may begin setting parameters of their components and ticking them to process and refill the audio buffer in the main audio callback. The developer can reconfigure the number of statically allocated components by redefining a number of macros in OOPSMemConfig.h. The macros define the number of each component to allocate during compilation. 

    // A brief example illustrating how to perform
    // frequency modulation with two sine waves.
    
    // Declare carrier and modulator oscillators. 
    tCycle* car, mod; // or tCycle* sine[2]; 
    
    float carPitch, modGain; 
    
    void myApplicationInit(float sampleRate, 
                           float (*randomNumber)(void))
    {
        OOPSInit(sampleRate, randomNumber);
        
        car = tCycle();
        mod = tCycle();
    }
    
    float myTick(float input)
    {
        tCycleSetFreq(car, carPitch + 
                     modGain * tCycleTick(mod));
        
        return tCycleTick(car);
    }
    
    void myBlockRateCallback(void)
    {
        // Change frequency and gain of 
        // carrier and modulator here.

         // Set modulator frequency.
        tCycleSetFreq(mod, getKnobVal(KNOB1);
        
        // Set modulator gain.
        modGain = getKnobVal(KNOB2);
        
        // Iterate through buffer and tick.
        for (int i = 0; i < buffLen; i++)
            outBuff[i] = myTick( inBuff[i]);
    }




