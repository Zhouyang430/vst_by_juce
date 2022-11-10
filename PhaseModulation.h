/**
@date 08-Nov-2022
@version 1.0
@brief perform phase modulation
*/

#include "Oscillators.h"
#include <math.h>

class PhaModulatedOsc
{
public:
    
    /**
     @brief set samplerate in Hz
     @param _sampleRate the sample rate in Hz
     */
    void setSampleRate(float _sampleRate)
    {
        sampleRate = _sampleRate;
        carrier.setSampleRate(sampleRate);
        modulator.setSampleRate(sampleRate);
        filterLFO.setSampleRate(sampleRate);
        // replace with setRate function
        filterLFO.setFrequency(0.0625);
    }
    
    /**
     @brief set samplerate in Hz. Best to have called setSampleRate() first
     @param freq the frequency of the carrier
    */
    void setFrequencyC(float _freq)
    {
        freqC = _freq;
        carrier.setFrequency(_freq);
    }
    /**
     @brief set samplerate in Hz. Best to have called setSampleRate() first
     @param freq the frequency of the modulator
    */
    void setFrequencyM(float _freq)
    {
        freqM = _freq;
        modulator.setFrequency(_freq);
    }

    /**
    @brief Mix the oscs together and put them through the modulating filter,
    @param _modIndex the modulation index, to control the amplitute modulator
    @return the modulated oscillator
     */
    float process(float _modIndex)
    {   modIndex = _modIndex;
      
        // get the pahase of carrier by arc sin function
        float ocsCarry = asin(carrier.process());
        // get osc values and sum them together
        float oscsOut = sin( ocsCarry + modulator.process() * modIndex) + ocsCarry;
        // set filter lfo
        float cutoffFreq = (filterLFO.process() * 500) + 1000;
        // set coefficients for a high filter
        filter.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, cutoffFreq));
        // perform filter to the
        float filterOut = filter.processSingleSampleRaw(oscsOut);
        return filterOut;
        
    }
private:
    // frequency of carrier
    float freqC;
    //frequency of modulator
    float freqM;
    // sample rate
    float sampleRate;
    // carrier, square osc from Oscillators.h header file
    SinOsc carrier;
    // modulator
    SinOsc modulator;
    // modIndex
    float modIndex = 10;
    // filter
    juce::IIRFilter filter;
    // lfo to change the filter
    SinOsc filterLFO;
    
};
