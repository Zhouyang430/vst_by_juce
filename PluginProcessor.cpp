/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
B200097PluginAudioProcessor::B200097PluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

B200097PluginAudioProcessor::~B200097PluginAudioProcessor()
{
}

//==============================================================================
const juce::String B200097PluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool B200097PluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool B200097PluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool B200097PluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double B200097PluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int B200097PluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int B200097PluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void B200097PluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String B200097PluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void B200097PluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void B200097PluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    //10 delay lines for overlaying delay effects
    chorus.initailize(sampleRate, 10, 0.05, 0.3);
    //initialize the bach music
    bachMusic.initialize(sampleRate);
    //initialize the filtered Noise
    bandFilteredNoise.setSampleRate(sampleRate);
    //initialize the delayline for bach music
    delay.setSizeInSamples(sampleRate);
    delay.setDelayTimeInSamples(sampleRate * 0.5);
    delay.setFeedback(0.3);
    // set up pulse width modulation parameters using our setter functions
    phaseMod.setSampleRate(sampleRate);
    //initialize the pwm oscillator
    pwmOsc.setSampleRate(sampleRate);
    pwmOsc.setFrequency(10);
    pwmOsc.setPWMFrequency(0.01);
    
    // using lfo for phase, amplitude modulation and to change the cut off value for the filter
    lfo.setSampleRate(sampleRate);
    lfo.setFrequency(0.01);
    // using lfo3 for reverb control
    lfo3.setSampleRate(sampleRate);
    lfo3.setFrequency(0.01);
    //set a low pass filter for bach
    juce::IIRCoefficients myCoefficientsHigh = juce::IIRCoefficients::makeHighPass(sampleRate, 100);
    // apply the filter parameters
    filterHigh.setCoefficients(myCoefficientsHigh);
    
    filterHigh.reset();
    //Destroys the object currently
    reverb.reset();
    filter.reset();
    //track the sample rate
    sr = sampleRate;
}

void B200097PluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool B200097PluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void B200097PluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
        
    
        // find out how many samples are in the block
        // e.g. 512
        int numberOfSamples = buffer.getNumSamples();
        
        // get pointers to both the left and the right channels in the buffer
        float* leftSamples = buffer.getWritePointer(0);
        float* rightSamples = buffer.getWritePointer(1);

        // make sure the random number for noise is different every time
        srand( static_cast<unsigned int>(time(nullptr)));
        // Our good old DSP loop
        for (int i=0; i<numberOfSamples; i++)
        {
            //store the final result
            float mix = 0;
            //using freq to store the initial frequency of bach notes from begining
            float freq = bachMusic.Midi2Fre();
            //set the freq to Osc so we can hear bach note sequence playing!
            bachMusic.setFreq4Osc(freq);
            // add band filter on noise to make it sounds like a wave (push and pull dynamically)
            float filtereNoise = bandFilteredNoise.process();
            //I want to achieve the effect of gray noise through chorus(in which I have 10 delaylines with slightly different LFO). People have different sensitivity to different frequencies, grey noise has been adjusted accordingly. Although the frequency distribution is uneven, people feel it is evenly distributed.
            float chorusNoise = chorus.process(filtereNoise,0.5);
            //change frequency of carrier wave in phase modulation
            float freoff = lfo.process() * 10.0f + 40.0f;
            //change cut off value of low pass filter,
            float cutoff = lfo.process() * 600.0f + 1200.0f;
            //process the pwm oscillator to make ”squeaky” rhythm at the end of my music
            float pwmOscVal = pwmOsc.process();
            // set coefficients for low pass filter
            juce::IIRCoefficients myCoefficients =  juce::IIRCoefficients::makeLowPass(sr, cutoff,10);
            // apply the filter parameters
            filter.setCoefficients(myCoefficients);
            //store the filter pwm result in variable filteredPWM
            float filteredPWM = filter.processSingleSampleRaw(pwmOscVal);
       
            //set frequency for carrier and modulaor for phase modulation
            phaseMod.setFrequencyC(freoff);
            //make the difference equals to 5
            phaseMod.setFrequencyM(freoff-5);
            
            //for these 2 lines below I am a little confused, they don't take any particular role in my VST output (at least I think)
            //However, when I delete them some part of my drone music sounds wired
            //so I just leave them there and keep figuring it out!!
            //If you have any advice please comment in my feedback, thank you very much!!
            //************************
            phaseMod.process(cutoff);
            delay.process(filtereNoise);
            //************************
            
            //for if statements below, I make bach sound changing along time, pass the current timer in the right function defined in Bach.h, and get the result from it.
            
            //for following part bach music remain the same as preset
            //1. Bach in the sea waves [0:00-0:20]
            
            //2. Distorted Bach [0:20-0:25]
            if(timer >= 20 * sr)
            {
                bachMusic.bachDistord(timer);
            }
            //for following part bach music remain the same
            //3. Space-time transport [0:25-0:30]
            
            //4. Particle acceleration [0:30-0:54]
            if(timer >= 30 * sr)
            {
                bachMusic.bachSpeedUp(timer);
            }
            //for following part bach music remain the same
            //5. Rest [0:54-0:57]
            
            //6. Stereo Explosions [0:57-1:67]
            if(timer >=  57 * sr and timer < 100 * sr)
            {
                bachMusic.bachSpeedBoom(timer);
            }
            //7. Medieval Organ [1:67-2:00]
            if(timer >=  100 * sr and timer < 120 * sr)
            {
                bachMusic.bachSpeedDown(timer);
            }
            //for following part bach music remain the same
            //8. Distortion again [2:00-2:08]
            
            //9. Vanish [2:08-2:20] for this part we don't need bach music
            
            //after set all things done, process the bach music and store it in bach so that we can use it for mix below!
            float bach = bachMusic.process();
            
            //set reverb parameters
            juce::Reverb::Parameters reverbParams;
            //change dry level, damping, roomsize varing along time too, using lfo3.
            // vary in [0,2]
            reverbParams.dryLevel = lfo3.process()+1;
            // vary in [1,3]
            reverbParams.damping = lfo3.process()+2;
            //exp() function to make things change very differently when lfo3 is in [-1,0] and [0,1]
            reverbParams.roomSize = exp(lfo3.process());
            reverb.setParameters(reverbParams);
            
            //for "if conditions" below, get the result from bach, then add it in mix to make the final sound!
            
            //1. Bach in the sea waves [0:00-0:20]
            if(timer < 20 * sr)
            {
                mix = 0.5 * chorusNoise + 0.2 * bach + 0.1 * kOsc.process(lfo.process() * 2 + 3,1);

            }
            //2. Distorted Bach [0:20-0:25]
            if(timer >=  20 * sr and timer < 25 * sr)
            {
                mix = 0.5 * chorusNoise + 0.2 * bach + lfo.process() * 0.2 * phaseMod.process(1);
            }
            //3. Space-time transport [0:25-0:30]
            if(timer >=  25 * sr and timer < 30 * sr)
            {
                mix = 0.2 * bach + 0.2 * delay.process(bach) +lfo.process() * 0.2 * phaseMod.process(1);
            }
            //4. Particle acceleration [0:30-0:54]
            if(timer >=  30 * sr and timer < 54 * sr)
            {
                phaseMod.setFrequencyC(freoff);
                //phaseMod.setFrequencyM(freoff-5);
                phaseMod.setFrequencyM(0.2);
                //osc.setFrequency((freq+lfo2.process()*20)-50);
                mix = 0.25 * lfo.process() * phaseMod.process(0.9) + 0.2 * bach;
            }
            
            //5. Rest [0:54-0:57], do nothing
            
            //6. Stereo Explosions [0:57-1:67]
            if(timer >=  57 * sr and timer < 100 * sr)
            {
                mix = 0.1 * filteredPWM + 0.2 * bach;
            }
            //7. Medieval Organ [1:67-2:00]
            if(timer >=  100 * sr and timer < 120 * sr)
            {
                mix = 0.25 * filteredPWM + 0.15 * bach + filterHigh.processSingleSampleRaw(0.2*delay.process(bach));
            }
            //8. Distortion again [2:00-2:08]
            if(timer >=  120 * sr and timer < 125 * sr)
            {
                mix = 0.25 * filteredPWM + filterHigh.processSingleSampleRaw(0.15*bach) + filterHigh.processSingleSampleRaw(0.15*delay.process(bach));
            }
            //9. Vanish [2:08-2:20] for this part we don't need bach music
            if(timer >=  125 * sr and timer < 132 * sr)
            {
                phaseMod.setFrequencyC(880+timer/10);
                phaseMod.setFrequencyM(40-timer/10);
             
                mix = 0.15 * phaseMod.process(0.001 * (timer + 1));

            }
            // store the resulting sample in the left and right channel arrays
            
            //stero effect for 6. Stereo Explosions [0:57-1:67]
            if(timer >=  57 * sr and timer < 100 * sr)
            {
            //using LFOs to automate the panning position
            leftSamples[i] =  mix * (lfo.process() * 0.15 + 0.15);
            rightSamples[i] = mix * ((-lfo.process()* 0.15 + 0.15));
            }
            //don't need stero for other parts
            else
            {
                leftSamples[i] = mix * 0.15;
                rightSamples[i] = mix * 0.15;
            }
            //track samples
            timer ++;
        }
        // apply reverb for 2 channels
        reverb.processStereo(leftSamples, rightSamples, numberOfSamples);
        

}

//==============================================================================
bool B200097PluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* B200097PluginAudioProcessor::createEditor()
{
    return new B200097PluginAudioProcessorEditor (*this);
}

//==============================================================================
void B200097PluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void B200097PluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new B200097PluginAudioProcessor();
}
