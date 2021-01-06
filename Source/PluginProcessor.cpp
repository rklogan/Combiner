/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CombinerAudioProcessor::CombinerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       //.withInput  ("Input 2", juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

CombinerAudioProcessor::~CombinerAudioProcessor()
{
}

//==============================================================================
const juce::String CombinerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CombinerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CombinerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CombinerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CombinerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CombinerAudioProcessor::getNumPrograms()
{
    return 1;
}

int CombinerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CombinerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CombinerAudioProcessor::getProgramName (int index)
{
    return {};
}

void CombinerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CombinerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    resetAndPrepare();
}

void CombinerAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CombinerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void CombinerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int hipass = 1;

    // clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (unsigned int channelNo{ 0 }; channelNo < totalNumInputChannels; ++channelNo)
    {
        auto channelData = buffer.getWritePointer(channelNo);
        for (unsigned int sampleNo{ 0 }; sampleNo < buffer.getNumSamples(); ++sampleNo)
        {
            channelData[sampleNo] = filterSample(channelData[sampleNo], channelNo, FilterType::hipass);
        }
    }

}

//==============================================================================
bool CombinerAudioProcessor::hasEditor() const
{
    return true; 
}

juce::AudioProcessorEditor* CombinerAudioProcessor::createEditor()
{
    return new CombinerAudioProcessorEditor (*this);
}

//==============================================================================
void CombinerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CombinerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CombinerAudioProcessor();
}

void CombinerAudioProcessor::reset()
{
    for (unsigned int channel{ 0 }; channel < 2; ++channel)
    {
        for (unsigned int i{ 0 }; i < 5; ++i)
        {
            loX[channel][i] = 0.0;
            loY[channel][i] = 0.0;
            hiX[channel][i] = 0.0;
            hiY[channel][i] = 0.0;
        }
    }
}

void CombinerAudioProcessor::prepare()
{
    //prepare the lpf
    prepHelper(FilterType::lopass);
    calculateCoefficients(FilterType::lopass);

    //TODO: Fix this optimization

    //prepare the intermediate parameters for hpf
    //if (loAndHiLinked)
    //{
    //    // copy omegas and gains
    //    for (unsigned int i{ 1 }; i < 5; ++i)
    //    {
    //        w[1][i] = w[0][i];
    //        k[1][i] = w[0][i];
    //    }
    //}
    //else
    prepHelper(FilterType::hipass);

    //calculate the coefficients of the hpf
    calculateCoefficients(FilterType::hipass);
}

void CombinerAudioProcessor::resetAndPrepare()
{
    reset();
    prepare();
}


void CombinerAudioProcessor::prepHelper(FilterType type)
{
    int i = type == FilterType::lopass ? 0 : 1;

    w[i][1] = juce::MathConstants<double>::twoPi * fc[i];
    w[i][2] = w[i][1] * w[i][1];
    w[i][3] = w[i][2] * w[i][1];
    w[i][4] = w[i][2] * w[i][2];

    k[i][1] = w[i][1] / tan(juce::MathConstants<double>::pi * fc[i] / getSampleRate());
    k[i][2] = k[i][1] * k[i][1];
    k[i][3] = k[i][2] * k[i][1];
    k[i][4] = k[i][2] * k[i][2];

    tmp1 = juce::MathConstants<double>::sqrt2 * w[i][3] * k[i][1];
    tmp2 = juce::MathConstants<double>::sqrt2 * w[i][1] * k[i][3];
    tmp_a = 4 * w[i][2] * k[i][2] + 2 * tmp1 + k[i][4] + 2 * tmp2 + w[i][4];
}

void CombinerAudioProcessor::calculateCoefficients(FilterType type)
{
    int i = type == FilterType::lopass ? 0 : 1;

    b[i][1] = (4 * (w[i][4] + tmp1 - k[i][4] - tmp2)) / tmp_a;
    b[i][2] = (6 * w[i][4] - 8 * w[i][2] * k[i][2] + 6 * k[i][4]) / tmp_a;
    b[i][3] = (4 * (w[i][4] - tmp1 + tmp2 - k[i][4])) / tmp_a;
    b[i][4] = (k[i][4] - 2 * tmp1 + w[i][4] - 2 * tmp2 + 4 * w[i][2] * k[i][2]) / tmp_a;

    a[i][0] = (type == FilterType::lopass ? w[i][4] : k[i][4]) / tmp_a;
    a[i][1] = (type == FilterType::lopass ? 4 : -4) * a[i][0];
    a[i][2] = 6 * a[i][0];
    a[i][3] = a[i][1];
    a[i][4] = a[i][0];
}

float CombinerAudioProcessor::filterSample(float inputSample, unsigned int channelNo, FilterType type)
{
    switch (order)
    {
    case 2:
        return filterSample2(inputSample, channelNo, type);
    case 4:
        return filterSample4(inputSample, channelNo, type);
    case 8:
        return filterSample8(inputSample, channelNo, type);
    default:
        throw new _exception;
    }
}

float CombinerAudioProcessor::filterSample2(float inputSample, unsigned int channelNo, FilterType type)
{
    //TODO
    return inputSample;
}

float CombinerAudioProcessor::filterSample4(float inputSample, unsigned int channelNo, FilterType type)
{
    //select the filter type
    unsigned int mode = type == FilterType::hipass ? 1 : 0;
    double* x_mem = type == FilterType::hipass ? hiX[channelNo] : loX[channelNo];
    double* y_mem = type == FilterType::hipass ? hiY[channelNo] : loY[channelNo];

    //Apply transfer function
    double output = a[mode][0] * inputSample
        + a[mode][1] * x_mem[1]
        + a[mode][2] * x_mem[2]
        + a[mode][3] * x_mem[3]
        + a[mode][4] * x_mem[4]
        - b[mode][1] * y_mem[1]
        - b[mode][2] * y_mem[2]
        - b[mode][3] * y_mem[3]
        - b[mode][4] * y_mem[4];

    //propogate memory
    x_mem[4] = x_mem[3];
    x_mem[3] = x_mem[2];
    x_mem[2] = x_mem[1];
    x_mem[1] = inputSample;
    y_mem[4] = y_mem[3];
    y_mem[3] = y_mem[2];
    y_mem[2] = y_mem[1];
    y_mem[1] = output;

    return output;
}

float CombinerAudioProcessor::filterSample8(float inputSample, unsigned int channelNo, FilterType type)
{
    // TODO
    return inputSample;
}