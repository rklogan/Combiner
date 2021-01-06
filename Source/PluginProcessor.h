/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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
    void reset();
    void prepare();
    void resetAndPrepare();

private:
    unsigned int numChannels{ 2 };
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

    void prepHelper(bool low = true);
    void calculateCoefficients(bool low = true);
    float filterSample4(float input, unsigned int channelNo, bool hipass = false);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CombinerAudioProcessor)
};
