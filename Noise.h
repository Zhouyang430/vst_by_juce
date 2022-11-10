/**
@date 08-Nov-2022
@version 1.0
@brief To create the sound of waves, I used a band-pass filter with varying frequencies and 10 delay lines to achieve a superimposed delay effect. Due to the relatively low frequency range of the bandpass filter (50-150Hz), I obtained a "grey noise" (smoother and thicker)
*/
#include <JuceHeader.h>
#include "Oscillators.h"
#pragma once

class Noise
{
public:
    // set sample rate for noise
    void setSampleRate(float _sampleRate)
    {
        sampleRate = _sampleRate;
        filterLFO.setSampleRate(sampleRate);
        filterLFO.setFrequency(1);
    }

    float process()
    {   //get next random float, I have included srand( static_cast<unsigned int>(time(nullptr))); in processor
        noise = random.nextFloat();
        // use band pass filter for noise
        float cutoffFreq = (filterLFO.process() * 50) + 100;
        filter.setCoefficients(juce::IIRCoefficients::makeBandPass(sampleRate, cutoffFreq));
        float filterOut = filter.processSingleSampleRaw(noise);
        return filterOut;
    }
    
private:
    //random number
    juce::Random random;
    //noise
    float noise;
    // filter
    juce::IIRFilter filter;
    // lfo to change the filter
    SinOsc filterLFO;
    //sample rate
    float sampleRate;
};

