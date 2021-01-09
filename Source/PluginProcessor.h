#pragma once

#include <JuceHeader.h>

// Parameter Identifiers
#define LINKED_ID "linked"
#define LINKED_NAME "Linked"
#define SLOPE_ID "slope_id"
#define SLOPE_NAME "Slope"
#define LOPASS_FREQ_ID "lpf_freq_id"
#define LOPASS_FREQ_NAME "Low-Pass Cutoff"
#define HIPASS_FREQ_ID "hpf_freq_id"
#define HIPASS_FREQ_NAME "High-Pass Cutoff"

// Global Parameters
enum class FilterType { lopass, hipass };
const juce::StringArray slopes("12", "24", "48");
const juce::NormalisableRange<float> frequencyRange(20.0f, 20000.0f, 0.1f, 0.25f);

//==============================================================================
/**
* CombinerAudioProcessor
* Implements the logic for the audio thread.
* @author Ryan Logan
* 
*/
class CombinerAudioProcessor  : public juce::AudioProcessor
{
public:
    // Holds all paramters visible in the UI
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
    * Updates the fequencies used in the processor.  
    * If the filters are linked, both will be updated
    * @param callReset If true, reset() will be called before returning
    * @param callPrepare If true, prepare() will be called before returning
    * @see reset()
    * @see prepare()
    */
    void updateFrequencies(bool callReset = false, bool callPrepare = false);

private:
    unsigned int numChannels{ 2 };

    // centre frequency for lo-pass and hi-pass respectively
    double fc[2]{ 750.0, 750.0 };

    // intermediate paramters for filters
    // rows are left and right
    // columns are used differently based on filter chosen
    double w[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double k[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };

    // memory for the filters
    // first two rows are left and right
    // second two rows are left and right for the second stage of a cascaded filter
    double loX[4][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},
                      {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double loY[4][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},
                      {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double hiX[4][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},
                      {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double hiY[4][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0},
                      {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };

    // filter coefficients
    // second row is only for the second stage of cascaded filters
    double a[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };
    double b[2][5]{ {0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0} };

    // misc internal filter parameters
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

