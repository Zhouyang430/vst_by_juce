/**
@date 08-Nov-2022
@version 1.0
@brief The Chorus class (Vector) implements the effect of superposition of several slightly different DelayLines. Its function is to deal with noise
*/

#include "Oscillators.h"
#include "DelayLIne.h"
#pragma once

class Chorus
{
public:
    //tips
    //delay = delay time - ~800 sounds good
    //feedback = 0 - 1
    //speed = lfo speed in Hz, 0.0001 - 10 sounds good
    //depth = 0 - 1
    /**
    @brief initialize the chorus
    @param sampleRate sample rate
    @param numOfDelayLine numuber of delaylines
    @param beginLfoFreq lfo frequncy we start  with to change
    @param beginFeedback feedback we start  with to change
    */
    void initailize(float sampleRate, int numOfDelayLine,float beginLfoFreq,float beginFeedback)
    {
        //pass the sample rate in
        sr = sampleRate;
        for (int i=0; i<numOfDelayLine; i++)
        {   //push every object in the vector
            lfoVector.push_back(std::make_unique<SinOsc>());
            //set the sample rate for each object
            lfoVector[i]->setSampleRate(sr);
            //change each frequency slightly different
            lfoVector[i]->setFrequency(beginLfoFreq+(i+1)*0.01);
            //add the delayline into the delay vector
            delayVector.push_back(std::make_unique<DelayLine>());
            //initialize each delayline
            delayVector[i]->setSizeInSamples(sr);
            //change each frequency slightly different
            delayVector[i]->setFeedback(beginFeedback+(i+1)*0.005);
        }
   }
float process(float sample,float depth)
{
    for (int j=0; j<delayVector.size(); j++)
    {
        
        delayTime = lfoVector[j]->process() *0.2+0.6;
        delayVector[j]->setDelayTimeInSamples(sr * delayTime);
        delayChorus =  delayVector[j]->process(sample);
        // add oscillator output onto the mix
        sample += delayChorus;
    }
    // we divide by oscCount so that the volume never goes above ±1
    // and we avoid digitial distortion
    return sample/delayVector.size();
}
private:
    //use smart pointers instead, that automatically clean up after themselves
    std::vector<std::unique_ptr<DelayLine>> delayVector;
    std::vector<std::unique_ptr<SinOsc>> lfoVector;
    //temp varible to store the chorus result
    float delayChorus;
    //dalay time from record and play
    float delayTime;
    //sample rate
    float sr;
    
};

