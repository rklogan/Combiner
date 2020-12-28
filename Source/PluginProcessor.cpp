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
                       .withInput  ("Input 2", juce::AudioChannelSet::stereo(), true)
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
    temporaryOutput = new juce::AudioBuffer<float>(outputChannels, samplesPerBlock);
}

void CombinerAudioProcessor::releaseResources()
{
    delete(temporaryOutput);
    temporaryOutput = NULL;
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

    // clear unused output channels
    for (auto i = outputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //clear temporary buffer
    for (unsigned int i{ 0 }; i < outputChannels; ++i)
        temporaryOutput->clear(i, 0, temporaryOutput->getNumSamples());

    for (unsigned int inputChannelNo{ 0 }; inputChannelNo < totalNumInputChannels; ++inputChannelNo)
    {
        auto inputData = buffer.getReadPointer(inputChannelNo);
        temporaryOutput->addFrom(
            (inputChannelNo % 2) % outputChannels,
            0,
            inputData,
            buffer.getNumSamples(),
            1.0F
        );
    }

    // copy the data from the temporary buffer to the output
    for (unsigned int i{ 0 }; i < outputChannels; ++i)
    {
        auto output = buffer.getWritePointer(i);
        
        for (unsigned int j{ 0 }; j < buffer.getNumSamples(); ++j)
        {
            output[j] = temporaryOutput->getSample(i, j);
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
