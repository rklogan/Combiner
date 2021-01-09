/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define LINKED_ID "linked"
#define LINKED_NAME "Linked"
#define SLOPE_ID "slope_id"
#define SLOPE_NAME "Slope"

enum class FilterType { lopass, hipass };
const juce::StringArray slopes("12", "24", "48");

//==============================================================================
/**
*/
class CombinerAudioProcessor  : public juce::AudioProcessor
{
public:
    juce::AudioProcessorValueTreeState parameters;

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

    //================================== UI Hooks ==================================
    /**
    * Set both cutoff frequencies to the same value regardless of whether they are linked
    * @param newCutoff The new cutoff frequency for both filters in Hz
    * @param callReset true will reset the filter memories. Defaults to false
    * @param callPrepare true will recalculate the filter coefficients. Defaults to false
    */
    void setBothCutoffFrequencies(double newCutoff, bool callReset = false, bool callPrepare = false);

    /**
    * Set the cutoff for both filters independently
    * @param newLow The new cutoff frequency for the low pass filter
    * @param newHigh the new cutoff frequency for the high pass filter. If the two filters are linked, this parameter will be ignored
    * @param callReset true will reset the filter memories. Defaults to false
    * @param callPrepare true will recalculate the filter coefficients. Defaults to false
    */
    void setCutoffFrequencies(double newLow, double newHigh, bool callReset = false, bool callPrepare = false);

    /**
    * Set the cutoff for the low pass filter.
    * @param newLow The new cutoff frequency for the low pass filter. If the filters are linked, the high pass will also be updated.
    * @param callReset true will reset the filter memories. Defaults to false
    * @param callPrepare true will recalculate the filter coefficients. Defaults to false
    */
    void setLowPassCutoff(double newLow, bool callReset = false, bool callPrepare = false);

    /**
    * Set the cutoff for the high pass filter.
    * @param newHigh The new cutoff frequency for the high pass filter. If the filters are linked, the low pass will also be updated.
    * @param callReset true will reset the filter memories. Defaults to false
    * @param callPrepare true will recalculate the filter coefficients. Defaults to false
    */
    void setHighPassCutoff(double newHigh, bool callReset = false, bool callPrepare = false);

    /**
    * Get the cutoff frequency of the low pass filter.
    * @return The cutoff frequency of the low pass filter.
    */
    double getLowPassCutoff();

    /**
    * Get the cutoff frequency of the high pass filter.
    * @return The cutoff frequency of the high pass filter.
    */
    double getHighPassCutoff();

private:
    unsigned int numChannels{ 2 };
    double fc[2]{ 750.0, 750.0 };
    double w[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double k[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double loX[4][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},
                      {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double loY[4][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},
                      {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double hiX[4][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},
                      {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double hiY[4][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},
                      {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double a[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double b[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double tmp1{ 0.0 }, tmp2{ 0.0 }, tmp_a{ 0.0 };

    /**
    * Helper function that calculates all intermediary parameters for a filter
    * @param type The type of filter. If the lopass and hipass have the same cutoff frequency this function only needs to be called once with any value for type.
    * @see prepHelper2()
    * @see prepHelper4()
    * @see prepHelper8()
    */
    void prepHelper(FilterType type);

    /**
    * Helper function that calculates all intermediary parameters for a filter
    * @param type The type of filter. If the lopass and hipass have the same cutoff frequency this function only needs to be called once with any value for type.
    */
    void prepHelper2(FilterType type);

    /**
    * Helper function that calculates all intermediary parameters for a filter
    * @param type The type of filter. If the lopass and hipass have the same cutoff frequency this function only needs to be called once with any value for type.
    */
    void prepHelper4(FilterType type);

    /**
    * Helper function that calculates all intermediary parameters for a filter
    * @param type The type of filter. If the lopass and hipass have the same cutoff frequency this function only needs to be called once with any value for type.
    * @see prepHelper4()
    */
    void prepHelper8(FilterType type);

    /**
    * Helper function to calculate the filter coefficients
    * @param type Chooses whether to update the lopass or hipass filter
    * @see calculateCoefficients2()
    * @see calculateCoefficients4()
    * @see calculateCoefficients8()
    */
    void calculateCoefficients(FilterType type);

    /**
    * Helper function to calculate the filter coefficients
    * @param type Chooses whether to update the lopass or hipass filter
    */
    void calculateCoefficients2(FilterType type);

    /**
    * Helper function to calculate the filter coefficients
    * @param type Chooses whether to update the lopass or hipass filter
    */
    void calculateCoefficients4(FilterType type);

    /**
    * Helper function to calculate the filter coefficients
    * @param type Chooses whether to update the lopass or hipass filter
    * @see calculateCoefficients4()
    */
    void calculateCoefficients8(FilterType type);
    
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
    * @param stage When cascading a value of 1 can be supplied to use a different history
    * @return The filtered output sample
    */
    float filterSample4(float inputSample, unsigned int channelNo, FilterType type, unsigned int stage=0);
    
    /**
    * Helper function to apply a 8th order LinkWitz-Riley Filter
    * @param inputSample The sample to be processed
    * @param channelNo The channel number in the filter
    * @param type The filter to apply
    * @return The filtered output sample
    * @see filterSample4()
    */
    float filterSample8(float inputSample, unsigned int channelNo, FilterType type);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CombinerAudioProcessor)
};

