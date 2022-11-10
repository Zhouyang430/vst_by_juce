/**
@date 08-Nov-2022
@version 1.0
@brief implement the delayline effect
*/

#pragma once
class DelayLine
{
    
public:
    
    /**
     IMPORTANT. clean up the dynamically allocated array
     when our object is destroyed!
     */
    ~DelayLine()
    {
        if (data != nullptr)
            delete[] data;
    }
 
    ///set the maximum size of the delay line
    void setSizeInSamples(int newSize)
    {
        // store the new size
        size = newSize;
        // free up existing data
        if (data != nullptr)
            delete[] data;
        // initialise our array
        data = new float[size];
        // set all values to zero
        for (int i=0; i<size; i++)
        {
            data[i] = 0.0;
        }
    }
    
    
    ///set the delay time in samples
    void setDelayTimeInSamples(float newDelayTime)
    {
        delayTime = newDelayTime;
        // set the read index to be behind the write index
        readIndex = writeIndex - delayTime;
        if (readIndex < 0)
            readIndex = readIndex + size;
        
    }
    /// call every sample to:
    ///--  store the new sample
    ///-- advance read index and write index
    ///-- return the value at the read index
    float process(float inputSample)
    {
        // get value of data at green read index
        float outputSample = linearInterpolation();
        // store value at red write index
        data[writeIndex] = inputSample  +  outputSample * feedback;
        // advance the read and write indexes
        // wrap them back into the correct range
        readIndex += 1;
        if (readIndex >= size)
            readIndex -= size;
        writeIndex += 1;
        if (writeIndex >= size)
            writeIndex -= size;
        return outputSample;
    }
    
    float linearInterpolation()
    {
        // readIndex = 2.3;
        
        // get the two indexes around our read index
        int indexA = int(readIndex);        // e.g. 2
        int indexB = indexA + 1;            // e.g. 3
        
        // wrap indexB if necessary
        if (indexB > size)
            indexB -= size;
        
        // get values at data indexes
        float valA = data[indexA];          // e.g. data[2]
        float valB = data[indexB];          // e.g. data[3]
        
        
        // calculate remainder
        float remainder = readIndex - indexA;   // e.g. 2.3 - 2 = 0.3
        // work out interpolated sample between two indexes
        float interpolatedSample = (1-remainder) * valA + remainder * valB;
        return interpolatedSample;
    }
    void setFeedback(float _feedback)
    {
        feedback = _feedback;
        if (feedback < 0)
            feedback = 0;
        if (feedback > 1.0)
            feedback = 1.0;
    }

    
private:
    // read position as an index
    float readIndex = 0;
    // write position as an index
    int writeIndex = 0;
    // maximum possible delay time
    int size;
    // store input audio data
    float* data = nullptr;
    // delay time in samples
    int delayTime;
    // must be between 0 and 1!!!
    float feedback = 0.99;
    
};

