/**
@date 08-Nov-2022
@version 1.0
@brief This head file contains Bach class, which initializes Bach's music through conversion form MIDI to frequency, and adds different effectors to realize the development of Bach's music. Including filter, amplitude modulation, frequency modulation, etc.
*/
#include "Oscillators.h"
#include <JuceHeader.h>
#ifndef Bach_h
#define Bach_h
#endif /* Bach_h */
#pragma once

// This class implements an independent processing function for bach music for convenient calling in DSP loop
class Bach
{
public:
    /**
    @brief initialize the sampleRate for osc and phasor
    @param sampleRate sample rate
    */
    void initialize(float sampleRate)
    {
        //pass in and set the value of sample rate
        sr = sampleRate;
        //osc - control the frequency (pitch) of bach
        osc.setSampleRate(sampleRate);
        //initialized with a preset value
        osc.setFrequency(432);
        //phasor - control the tempo of bach
        phasor.setSampleRate(sampleRate);
        phasor.setFrequency(0.05);
        //lfo for phasor
        lfo.setSampleRate(sampleRate);
        lfo.setFrequency(0.01);
        //lfo for osc
        lfo2.setSampleRate(sampleRate);
        lfo2.setFrequency(0.3);

    }
    /**
        @brief set frequency for osc
        @param freq frequency of osc
    */
    void setFreq4Osc(float freq)
    {
        osc.setFrequency(freq);
    }
    /**
        @brief set frequency for phasor
        @param freq frequency of phasor
    */
    void setFreq4Phasor(float freq)
    {
        phasor.setFrequency(freq);
    }
    /**
        @brief set frequency for lfo
        @param freq frequency of lfo
    */
    void setFreq4Lfo(float freq)
    {
        lfo.setFrequency(freq);
    }
    /**
        @brief convert midi to frequency
        @return frequency after MIDI-convertion
    */
    float Midi2Fre()
    {
        phasorOutput = phasor.process();
        //convert phase into interger
        noteIndex = int(phasorOutput * noteCount);
        //create note sequences
        note = notes[noteIndex];
        //convert midi to frequency 
        freq = juce::MidiMessage::getMidiNoteInHertz(note);
        return freq;
    }
    /**
        @brief process bach using a lfo for amplitude modulation
        @return bach music sample
    */
    float process()
    {
        bach = (lfo.process()*0.2+1.1) * osc.process();
        return bach;
    }
    //for if statements below, pass the current timer in the right function defined here, and return the result.
    
    //This funtion is used for 2. Distorted Bach [0:20-0:25]
    void bachDistord(int timer)
    {
        phasor.setFrequency(lfo.process()* 0.01+0.1);
        //2*(10*sr-timer)/timer is used to change frequency of osc smoothly
        osc.setFrequency(2*(10*sr-timer)/timer+freq+lfo2.process()*10);
    }
    //This funtion is used for 3. Space-time transport [0:25-0:30]
    void bachSpeedUp(int timer)
    {
        phasor.setFrequency(lfo.process() * 0.5 + 1);
        osc.setFrequency(2*(10*sr-timer)/timer+freq+lfo2.process()*10);
    }
    
    //This funtion is used for 4. Particle acceleration [0:30-0:54]
    void bachSpeedBoom(int timer)
    {
        phasor.setFrequency(lfo.process() * 5 + 5);
        osc.setFrequency(freq);
    }
    //This funtion is used for 7. Medieval Organ [1:67-2:00]
    void bachSpeedDown(int timer)
    {
        phasor.setFrequency(lfo.process() * 0.005 + 0.05);
        osc.setFrequency(freq);
    }
    
private:
    //osc - control the frequency (pitch) of bach
    SquareOsc osc;
    //phasor - control the tempo of bach
    Phasor phasor;
    //lfo for phasor
    SinOsc lfo;
    //lfo for osc
    SinOsc lfo2;
    // note sequetnce, this is piece of music from BWV846 I mentioned in the report
    int notes[64] = {48, 52, 55, 60 ,64, 55, 60 ,64,48, 52, 55, 60 ,64, 55, 60 ,64, 48, 50, 57, 62, 65, 57, 62, 65,48, 50, 57, 62, 65, 57, 62, 65,47, 50, 55, 62, 65,55, 62, 65,47, 50, 55, 62, 65,55, 62, 65,47,48,52, 55, 60,52, 55, 60,47,48,52, 55, 60,52, 55, 60};
    //sizeof(notes);
    int noteCount = 64;
    // note index
    int noteIndex;
    // every single note
    int note;
    // freqency of notes sequency
    float freq;
    //final sample for bach music
    float bach;
    //phasor
    float phasorOutput;
    // sample rate
    float sr;
};

