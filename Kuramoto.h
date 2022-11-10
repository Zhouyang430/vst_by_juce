/**
@date 08-Nov-2022
@version 1.0
@brief This is not written by me, but I really want to use the concep to create my music! It is made by Maximilian (the code can be find in github)
Kuramoto model. the Kuramoto model is a model of the behaviour of a large number of coupled oscillators and is used to describe synchronization. It is motivated by the behaviour of chemical and biological oscillator systems and it has found a wide range of applications in the fields of neuroscience, oscillatory flame dynamics.
see details in "https://www.complexity-explorables.org/explorables/ride-my-kuramotocycle/"
The original code does not include comments, but I have tried to add some according to my understanding for important functions
*/

#include<math.h>
#pragma once
class KuramotoOscillator
{
public:
    KuramotoOscillator() {}
    /**
    @brief calculate  the phase of oscillator i (theta)
    @param freq natural frequency of oscillator i
    @param K a constant referred to as the coupling constant.
    @param phases
    @return new phase
     */
    inline float process(float freq, float K, std::vector<float> phases)
    {

        float phaseAdj = 0;
        for (float v : phases)
        {
            phaseAdj += sin(v - phase);
        }
        phase += dt * (freq + ((K / phases.size()) * phaseAdj));
        if (phase >= 2* M_PI)
            phase -= 2* M_PI;
        else if (phase < 0)
            phase += 2* M_PI;
        return phase;
    }
    inline void setPhase(float newPhase)
    {
        phase = newPhase;
    }
    
    inline float getPhase()
    {
        return phase;
    }

private:
    float phase = 0.0;
    float sampleRate = 44100;
    float dt = 2* M_PI / sampleRate;
    
    
};

//a group of oscillators
class KuramotoOscillatorSet
{
public:
    //N is the total number of oscillators in the system
    KuramotoOscillatorSet(const size_t N)
    {
        oscs.resize(N);
        phases.resize(N);
    };
    void setPhases(const std::vector<float> &phases)
    {
        size_t iOsc = 0;
        for (float v : phases)
        {
            oscs[iOsc].setPhase(v);
            iOsc++;
        }
    }

    void setPhase(const float phase, const size_t oscillatorIdx)
    {
        oscs[oscillatorIdx].setPhase(phase);
    }

    float getPhase(size_t i)
    {
        return oscs[i].getPhase();
    }

    size_t size()
    {
        return oscs.size();
    }
    void setSampleRate(float sr)
    {
        sampleRate = sr;
    }
    /**
    @brief calculate the sum for coupled oscillators
    @param freq natural frequency of system
    @param K is a constant referred to as the coupling constant
    @return the sum for coupled oscillators, divided by total size to avoid distortion
     */
    float process(float freq, float K)
    {
        float mix = 0.0;
        //gather phases
        for (size_t i = 0; i < phases.size(); i++)
        {
            phases[i] = oscs[i].getPhase();
        }
        for (auto &v : oscs)
        {
            mix += v.process(freq, K, phases);
        }
        return mix / phases.size();
    }

private:
    std::vector<KuramotoOscillator> oscs;
    std::vector<float> phases;
    float sampleRate=44100;
};
