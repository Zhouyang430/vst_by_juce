/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Oscillators.h"
#include "PWMOsc.h"
#include "PhaseModulation.h"
#include "DelayLIne.h"
#include "Kuramoto.h"
#include "Noise.h"
#include "Bach.h"
#include "Chorus.h"
//==============================================================================
/**
*/
class B200097PluginAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    B200097PluginAudioProcessor();
    ~B200097PluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //track the sample rate
    float sr;
    //track the samples
    int timer;
    //all sets of bach music
    Bach bachMusic;
    //pulse width modulation oscillator
    PWMOsc pwmOsc;
    //low frequency osscilator for modulations
    SinOsc lfo;
    //low frequency osscilator for reverb
    TriOsc lfo3;
    //phase modulation
    PhaModulatedOsc phaseMod;
    //low pass filter
    juce::IIRFilter filter;
    //high pass filter
    juce::IIRFilter filterHigh;
    //Reverb reverb;
    juce::Reverb reverb;
    //delay
    DelayLine delay;
    // noise
    Noise bandFilteredNoise;
    //chorus
    Chorus chorus;
    //kuramoto model with 5 different phases
    KuramotoOscillatorSet kOsc = KuramotoOscillatorSet(5);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (B200097PluginAudioProcessor)
};
