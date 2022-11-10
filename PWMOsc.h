/**
@date 08-Nov-2022
@version 1.0
@brief modulate the pulse width of a square wave with a slow sine wave
*/

#pragma once
#include "Oscillators.h"
/**
 A composite class that modulates the pulse width of a square wave with a slow sine wave
 @TODO A setter function to be able to control the depth of the modulation would be really useful. It is currently hard coded as 0.1-0.9
 */
class PWMOsc
{
public:
    /**
    @brief set samplerate in Hz
    @param _sampleRate the sample rate in Hz
    */
    void setSampleRate(float _sampleRate)
    {
        osc.setSampleRate(_sampleRate);
        lfoPulseWidth.setSampleRate(_sampleRate);
    }
    /**
    @brief set samplerate in Hz
    @param freq the frequency of the oscillator
    */
    void setFrequency(float freq)
    {
        osc.setFrequency(freq);
    }
    
    /**
    @brief set modulation rate in Hz
    @param freq the modulation rate in Hz
     */
    void setPWMFrequency(float freq)
    {
        lfoPulseWidth.setFrequency(freq);
    }
    
    /**
    @brief modulate the pulse width and return the square wave output
     */
    float process()
    {
        // shift range from ±1 to 0.1-0.9
        float pwVal = lfoPulseWidth.process() * 0.4 + 0.5;
        
        // apply this to square osc
        osc.setPulseWidth(pwVal);
        
        // get output of square osc
        float oscOut = osc.process();
        
        return oscOut;
    }
    
    
private:
    // square wave osc
    SquareOsc osc;
    // low frequency sine oscillator for modulation
    SinOsc lfoPulseWidth;
    
    
};
