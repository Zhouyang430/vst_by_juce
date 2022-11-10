/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class B200097PluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    B200097PluginAudioProcessorEditor (B200097PluginAudioProcessor&);
    ~B200097PluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    B200097PluginAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (B200097PluginAudioProcessorEditor)
};
