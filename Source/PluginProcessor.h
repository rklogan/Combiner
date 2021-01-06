/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum class FilterType { lopass, hipass };

//==============================================================================
/**
*/
class CombinerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    CombinerAudioProcessor();
    ~CombinerAudioProcessor() override;

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

    //==============================================================================
    /**
    * Sets all filter memory to 0.0
    */
    void reset();
    /**
    * Calculates the values of all the required filter co-efficients for both filters
    */
    void prepare();
    /**
    * Helper function to call reset() then prepare()
    * @see reset()
    * @see prepare()
    */
    void resetAndPrepare();

private:
    unsigned int numChannels{ 2 }, order{ 4 };
    bool loAndHiLinked{ true };
    double fc[2]{ 750.0, 750.0 };
    double w[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double k[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double a[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double b[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double loX[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double loY[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double hiX[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double hiY[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double tmp1{ 0.0 }, tmp2{ 0.0 }, tmp_a{ 0.0 };

    /**
    * Helper function that calculates all intermediary parameters for a filter
    * @param type The type of filter. If the lopass and hipass have the same cutoff frequency this function only needs to be called once with any value for type.
    */
    void prepHelper(FilterType type);

    /**
    * Helper function to calculate the filter coefficients
    * @param type Chooses whether to update the lopass or hipass filter
    */
    void calculateCoefficients(FilterType type);
    
    /**
    * Applies a filter to the input sample. Choose whether to use a second, fourth, or eigth order Linkwitz-Riley filter
    * @param inputSample The sample to be processed
    * @param channelNo The channel number in the filter
    * @param type The filter to apply
    * @return The filtered output sample
    * @see filterSample2
    * @see filterSample4
    * @see filterSample8
    */
    float filterSample(float inputSample, unsigned int channelNo, FilterType type);

    /**
    * Helper function to apply a 2nd order LinkWitz-Riley Filter
    * @param inputSample The sample to be processed
    * @param channelNo The channel number in the filter
    * @param type The filter to apply
    * @return The filtered output sample
    */
    float filterSample2(float inputSample, unsigned int channelNo, FilterType type);
    
    /**
    * Helper function to apply a 4th order LinkWitz-Riley Filter
    * @param inputSample The sample to be processed
    * @param channelNo The channel number in the filter
    * @param type The filter to apply
    * @return The filtered output sample
    */
    float filterSample4(float inputSample, unsigned int channelNo, FilterType type);
    
    /**
    * Helper function to apply a 8th order LinkWitz-Riley Filter
    * @param inputSample The sample to be processed
    * @param channelNo The channel number in the filter
    * @param type The filter to apply
    * @return The filtered output sample
    */
    float filterSample8(float inputSample, unsigned int channelNo, FilterType type);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CombinerAudioProcessor)
};
