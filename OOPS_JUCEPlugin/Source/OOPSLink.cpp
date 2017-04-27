/*
  ==============================================================================

    OOPSLink.c
    Created: 18 Jan 2017 11:25:00am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "OOPSLink.h"

#define MYTEST 0
#define FM     1

std::vector<std::string> cButtonNames =  std::vector<std::string>
{
    "Snare",
    "Hihat",
    "Cowbell"
};

std::vector<std::string> cSliderNames =  std::vector<std::string>
{
    
};

std::vector<std::string> cComboBoxNames =  std::vector<std::string>
{
    
};

std::vector<std::string> cWaveformTypes =  std::vector<std::string>
{
    
};

#if 0
std::vector<std::string> cButtonNames =
{

};

std::vector<std::string> cSliderNames =
{
    "OscPitch",
    "Attack",
    "Decay",
    "RevTime",
    "RevMix",
    "Gain"
};

std::vector<std::string> cComboBoxNames =
{
    "Waveform"
};

std::vector<std::string> cWaveformTypes =
{
    "Sine",
    "Triangle",
    "Sawtooth",
    "Square"
};
#endif


std::vector<float> cSliderModelValues(cSliderNames.size());
std::vector<float> cSliderValues(cSliderNames.size());
std::vector<bool> cButtonStates(cButtonNames.size());
std::vector<int> cComboBoxStates(cComboBoxNames.size());




void setSliderModelValue(String name, float val)
{
    for (int i = 0; i < cSliderNames.size(); i++)
    {
        if (name == cSliderNames[i])
        {
            cSliderModelValues[i] = val;
            break;
        }
    }
}

void printSliderValues(void)
{
    for (int i = 0; i < cSliderNames.size(); i++)
    {
        DBG(cSliderNames[i] + ": " + String(cSliderValues[i]));
    }
}

bool getButtonState(String name)
{
    for (int i = 0; i < cButtonNames.size(); i++)
    {
        if (name == cButtonNames[i])
        {
            return cButtonStates[i];
        }
    }
    
    return false;
}

void setButtonState(String name, bool on)
{
    for (int i = 0; i < cButtonNames.size(); i++)
    {
        if (name == cButtonNames[i])
        {
            cButtonStates[i] = on;
            break;
        }
    }
}

int getComboBoxState(String name)
{
    for (int i = 0; i < cComboBoxNames.size(); i++)
    {
        if (name == cComboBoxNames[i])
        {
            return cComboBoxStates[i];
        }
    }
}

void setComboBoxState(String name, int idx)
{
    DBG("set state: " + name + " " + String(idx));
    for (int i = 0; i < cComboBoxNames.size(); i++)
    {
        if (name == cComboBoxNames[i])
        {
            cComboBoxStates[i] = idx;
            break;
        }
    }
}

void setSliderValue(String name, float val)
{
    for (int i = 0; i < cSliderNames.size(); i++)
    {
        if (name == cSliderNames[i])
        {
            cSliderValues[i] = val;
            break;
        }
    }
}

float getSliderValue(String name)
{
    float value = 0.0f;
    
    for (int i = 0; i < cSliderNames.size(); i++)
    {
        if (name == cSliderNames[i])
        {
            value = cSliderValues[i];
            break;
        }
    }
    
    return value;
}

float randomNumberGenerator(void)
{
    return ((float)rand()/RAND_MAX);
}
