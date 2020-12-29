/*
  ==============================================================================

    LRFilter.h
    Created: 28 Dec 2020 1:54:44pm
    Author:  ryank

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_dsp/processors/juce_LinkwitzRileyFilter.h>

enum FilterType{ hipass, lopass };

/**
* Implements a Linkwitz-Riley Filter
*/
class LRFilter
{
public:
    /**
    * Default Constructor.
    * If this construtor is used, prepare() should be called before processing
    * @see prepare()
    */
    LRFilter();

    /**
    * Constructor to fully initialize the filter.
    * @param t The desired FilterType: hipass, or lopass
    * @param cutoff The cutoff frequency for the filter
    * @param order The order of the filter. 2 for 12db/8ve, 4 for 24db/8ve and 8 for 48db/8ve
    * @param Fs The sample rate
    * @param maxBlockSize The maximum block size that will be used
    * @param numChannels The number of channels that will be in a buffer that is to be processed
    */
    LRFilter(FilterType t, double newCutoff, unsigned int newOrder, 
        double newFs, unsigned int newNumChannels);
    
    /**
    * Destructor
    */
    ~LRFilter();

    /**
    * Set the type of Filter
    * @param newType The new FilterType
    * @param runPrepare True, if prepare should be called before this function returns
    * @see prepare()
    */
    void setType(FilterType newType, bool runPrepare);
    
    /**
    * Get the type of this filter
    * @return The FilterType of this filter
    */
    FilterType getType();

    /**
    * Set a new cutoff frequency
    * @param newCutoff The new cutoff frequency to be used
    * @param runPrepare If true, prepare() will be called before returning
    * @see prepare()
    */
    void setCutoffFrequency(double newCutoff, bool runPrepare);

    /**
    * Get the current cutoff frequency
    * @return The current cutoff frequency
    */
    double getCutoffFrequency();

    /**
    * Set the slope for the filter
    * @param newSlope The new slope to be used. Must be 12/24/48. Measured in dB/octave
    * @param runPrepare If true, prepare() will be called before returning
    * @see prepare()
    */
    void setSlope(unsigned int newSlope, bool runPrepare);

    /**
    * Set the order of the filter
    * @param newOrder The order of filter to be used. Must be 2, 4 or 8.
    * @param runPrepare If true, prepare() will be called before returning
    * @see prepare()
    */
    void setOrder(unsigned int newOrder, bool runPrepare);

    /**
    * Get the current slope
    * @return The current slope in dB/octave
    */
    unsigned int getSlope();

    /**
    * Get the order of the filter
    * @return The current order
    */
    unsigned int getOrder();

    /**
    * Set the sample rate for the filter
    * @param newFS The new sample rate
    * @param runPrepare If true, prepare() will be called before returning
    * @see prepare()
    */
    void setSampleRate(double newFs, bool runPrepare);

    /**
    * Get the current sample rate
    * @return The current sample rate
    */
    double getSampleRate();

    /**
    * Change the number of channels that this filter will expect
    * @param newNumChannels The new number of channels to be used
    */
    void setNumChannels(unsigned int newNumChannels);

    /**
    * Get the current maximum number of channels allowed
    * @return The maxumim number of channels this filter can process
    */
    unsigned int getNumChannels();

    /**
    * Update the size of the filter's memory and clear it's contents
    */
    void reset();

    /**
    * Prepare this filter for processing
    * @throws An exception if the order of this filter is in an invalid state
    */
    void prepare();

    /**
    * Applies the filter to both of the provided buffers. Pass by call.
    * @param toLoPass The buffer to be lo passed
    * @param toHiPass The buffer to be hi passed
    */
    void process(juce::AudioBuffer<float>* toLoPass, juce::AudioBuffer<float>* toHiPass);

    /**
    * Applies a lo pass to the buffer
    * @param buffer The buffer to be lo passed
    */
    void loPass(juce::AudioBuffer<float>* buffer);

    /**
    * Applied a high pass to the buffer
    * @param buffer The buffer to be hi passed
    */
    void hiPass(juce::AudioBuffer<float>* buffer);

private:
    FilterType type{ lopass };
    double cutoff{ 500.0 }, Fs{ 44100.0 };
    unsigned int order{ 4 }, numChannels{ 2 };
    double loA[9], hiA[9], b[9];
    juce::AudioBuffer<float>* loX, *loY, *hiX, *hiY;

    void prepare2ndOrder();
    void prepare4thOrder();
    void prepare8thOrder();

    // TODO: Refactor to filter 2/4/8

    void loPass2(juce::AudioBuffer<float>* buffer);
    void loPass4(juce::AudioBuffer<float>* buffer);
    void loPass8(juce::AudioBuffer<float>* buffer);

    void hiPass2(juce::AudioBuffer<float>* buffer);
    void hiPass4(juce::AudioBuffer<float>* buffer);
    void hiPass8(juce::AudioBuffer<float>* buffer);
};
