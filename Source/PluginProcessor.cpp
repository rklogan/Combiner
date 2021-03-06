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
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withInput("Input 2", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif
    parameters(*this, nullptr, juce::Identifier("Combiner"),
        {
            // Create all the UI parameters
            std::make_unique<juce::AudioParameterBool>(LINKED_ID, LINKED_NAME, true),
            std::make_unique<juce::AudioParameterChoice>(SLOPE_ID, SLOPE_NAME, slopes, 1),
            std::make_unique<juce::AudioParameterFloat>(LOPASS_FREQ_ID, LOPASS_FREQ_NAME, frequencyRange, 750.0f),
            std::make_unique<juce::AudioParameterFloat>(HIPASS_FREQ_ID, HIPASS_FREQ_NAME, frequencyRange, 750.0f)
        })
{
}

CombinerAudioProcessor::~CombinerAudioProcessor()
{

}

//======================= JUCE Utility Functions ===============================
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

//======================= JUCE Playback Functions ==============================
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

    // clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //process lopass
    unsigned int channelNo{ 0 };
    for (; channelNo < 2; ++channelNo)
    {
        auto channelData = buffer.getWritePointer(channelNo);
        for (unsigned int sampleNo{ 0 }; sampleNo < buffer.getNumSamples(); ++sampleNo)
        {
            channelData[sampleNo] = filterSample(channelData[sampleNo], channelNo, FilterType::lopass);
        }
    }

    //process hipass
    for (; channelNo < 4; ++channelNo)
    {
        auto inputData = buffer.getReadPointer(channelNo);
        auto outputData = buffer.getWritePointer(channelNo % 2);
        for (unsigned int sampleNo{ 0 }; sampleNo < buffer.getNumSamples(); ++sampleNo)
        {
            outputData[sampleNo] += filterSample(inputData[sampleNo], channelNo % 2, FilterType::hipass);
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
    //generate XML of current state
    juce::XmlElement* combiner = new juce::XmlElement(juce::String("Combiner"));
    juce::XmlElement* hpf = new juce::XmlElement(juce::String("PARAM"));
    juce::XmlElement* linked = new juce::XmlElement(juce::String("PARAM"));
    juce::XmlElement* lpf = new juce::XmlElement(juce::String("PARAM"));
    juce::XmlElement* slope = new juce::XmlElement(juce::String("PARAM"));

    // create xml elements for each parameter
    hpf->setAttribute(juce::Identifier("id"), HIPASS_FREQ_ID);
    hpf->setAttribute(
        juce::Identifier("value"),
        juce::String(fc[1])
    );

    linked->setAttribute(juce::Identifier("id"), LINKED_ID);
    linked->setAttribute(
        juce::Identifier("value"),
        juce::String(parameters.getRawParameterValue(LINKED_ID)->load())
    );
    
    lpf->setAttribute(juce::Identifier("id"), LOPASS_FREQ_ID);
    lpf->setAttribute(
        juce::Identifier("value"),
        juce::String(fc[0])
    );

    slope->setAttribute(juce::Identifier("id"), SLOPE_ID);
    slope->setAttribute(
        juce::Identifier("value"),
        juce::String(parameters.getRawParameterValue(SLOPE_ID)->load())
    );

    // add parameter elements to main element
    combiner->addChildElement(hpf);
    combiner->addChildElement(linked);
    combiner->addChildElement(lpf);
    combiner->addChildElement(slope);

    //write to output
    copyXmlToBinary(*combiner, destData);
}

void CombinerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    //load saved state
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CombinerAudioProcessor();
}

void CombinerAudioProcessor::reset()
{
    // set all sample of all channels of memory to 0
    for (unsigned int channel{ 0 }; channel < 4; ++channel)
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
    prepHelper(FilterType::lopass);
    calculateCoefficients(FilterType::lopass);
    prepHelper(FilterType::hipass);
    calculateCoefficients(FilterType::hipass);
}

void CombinerAudioProcessor::resetAndPrepare()
{
    reset();
    prepare();
}

void CombinerAudioProcessor::updateFrequencies(bool callReset, bool callPrepare)
{
    fc[0] = parameters.getRawParameterValue(LOPASS_FREQ_ID)->load();
    fc[1] = parameters.getRawParameterValue(HIPASS_FREQ_ID)->load();

    if (callReset) reset();
    if (callPrepare) prepare();
}

void CombinerAudioProcessor::prepHelper(FilterType type)
{
    unsigned int mode = int(round(parameters.getRawParameterValue(SLOPE_ID)->load()));
    switch (mode)
    {
    case 0:
        prepHelper2(type);
        break;
    case 1:
        prepHelper4(type);
        break;
    case 2:
        prepHelper8(type);
        break;
    default:
        throw new _exception;
        break;
    }
}

void CombinerAudioProcessor::prepHelper2(FilterType type)
{
    int i = type == FilterType::lopass ? 0 : 1;

    w[i][1] = juce::MathConstants<double>::pi * fc[i];
    w[i][2] = w[i][1] * w[i][1];
    k[i][1] = w[i][1] / tan(w[i][1] / getSampleRate());
    k[i][2] = k[i][1] * k[i][1];

    tmp1 = 2 * k[i][1] * w[i][1];
    tmp2 = k[i][2] + w[i][2] + tmp1;
}

void CombinerAudioProcessor::prepHelper4(FilterType type)
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

void CombinerAudioProcessor::prepHelper8(FilterType type) 
{
    prepHelper4(type);
}

void CombinerAudioProcessor::calculateCoefficients(FilterType type)
{
    unsigned int mode = round(parameters.getRawParameterValue(SLOPE_ID)->load());
    switch (mode)
    {
    case 0:
        calculateCoefficients2(type);
        break;
    case 1:
        calculateCoefficients4(type);
        break;
    case 2:
        calculateCoefficients8(type);
        break;
    default:
        throw new _exception;
        break;
    }
}

void CombinerAudioProcessor::calculateCoefficients2(FilterType type)
{
    int i = type == FilterType::lopass ? 0 : 1;

    a[i][0] = (type == FilterType::lopass ? w[i][2] : k[i][2]) / tmp2;
    a[i][1] = (type == FilterType::lopass ? (2.0 * w[i][2]) : (-2.0 * k[i][2])) / tmp2;
    a[i][2] = a[i][0];

    b[i][1] = (2.0 * w[i][2] - 2.0 * k[i][2]) / tmp2;
    b[i][2] = (k[i][2] + w[i][2] - tmp1) / tmp2;
}

void CombinerAudioProcessor::calculateCoefficients4(FilterType type)
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

void CombinerAudioProcessor::calculateCoefficients8(FilterType type)
{
    calculateCoefficients4(type);
}

float CombinerAudioProcessor::filterSample(float inputSample, unsigned int channelNo, FilterType type)
{
    unsigned int mode = round(parameters.getRawParameterValue(SLOPE_ID)->load());
    switch (mode)
    {
    case 0:
        return filterSample2(inputSample, channelNo, type);
    case 1:
        return filterSample4(inputSample, channelNo, type);
    case 2:
        return filterSample8(inputSample, channelNo, type);
    default:
        throw new _exception;
        break;
    }
}

float CombinerAudioProcessor::filterSample2(float inputSample, unsigned int channelNo, FilterType type)
{
    // select filter type
    unsigned int mode = type == FilterType::lopass ? 0 : 1;
    double* x_mem = type == FilterType::lopass ? loX[channelNo] : hiX[channelNo];
    double* y_mem = type == FilterType::lopass ? loY[channelNo] : hiY[channelNo];

    //process
    double output = a[mode][0] * inputSample
        + a[mode][1] * x_mem[1]
        + a[mode][2] * x_mem[2]
        - b[mode][1] * y_mem[1]
        - b[mode][2] * y_mem[2];

    //propogate memory
    x_mem[2] = x_mem[1];
    x_mem[1] = inputSample;
    y_mem[2] = y_mem[1];
    y_mem[1] = output;

    //apply polarity flip to hpf and return
    return (type == FilterType::lopass ? output : (-1.0 * output));
}

float CombinerAudioProcessor::filterSample4(float inputSample, unsigned int channelNo, FilterType type, unsigned int stage)
{
    if (stage < 0 || stage > 1) throw new _exception;

    //select the filter type
    unsigned int mode = type == FilterType::hipass ? 1 : 0;
    double* x_mem = type == FilterType::hipass ? hiX[channelNo + 2 * stage] : loX[channelNo + 2 * stage];
    double* y_mem = type == FilterType::hipass ? hiY[channelNo + 2 * stage] : loY[channelNo + 2 * stage];

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
    return filterSample4(
        filterSample4(inputSample, channelNo, type, 0),
        channelNo, 
        type, 
        1
    );
}