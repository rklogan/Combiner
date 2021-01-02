/*
  ==============================================================================

    LRFilter.cpp
    Created: 28 Dec 2020 1:54:44pm
    Author:  ryank

  ==============================================================================
*/

#include "LRFilter.h"

LRFilter::LRFilter() 
{
    loX = new juce::AudioBuffer<float>(numChannels, order);
    loY = new juce::AudioBuffer<float>(numChannels, order);
    hiX = new juce::AudioBuffer<float>(numChannels, order);
    hiY = new juce::AudioBuffer<float>(numChannels, order);
    reset();
};

LRFilter::LRFilter(FilterType t, double newCutoff, unsigned int newOrder, 
    double newFs, unsigned int newNumChannels)
{
    setOrder(newOrder, false);
    setNumChannels(newNumChannels);
    loX = new juce::AudioBuffer<float>(numChannels, order);
    loY = new juce::AudioBuffer<float>(numChannels, order);
    hiX = new juce::AudioBuffer<float>(numChannels, order);
    hiY = new juce::AudioBuffer<float>(numChannels, order);
    reset();
    setType(t, false);
    setCutoffFrequency(newCutoff, false);
    setSampleRate(newFs, false);
    prepare();
}

LRFilter::~LRFilter() 
{
    delete(loX); delete(loY);
    delete(hiX); delete(hiY);
}

void LRFilter::setType(FilterType newType, bool runPrepare)
{
    type = newType;
    if (runPrepare)
        prepare();
}

FilterType LRFilter::getType() { return type; }

void LRFilter::setCutoffFrequency(double newCutoff, bool runPrepare)
{
    cutoff = newCutoff;
    if (runPrepare)
        prepare();
}

double LRFilter::getCutoffFrequency() { return cutoff; }

void LRFilter::setSlope(unsigned int newSlope, bool runPrepare)
{
    if (newSlope == 12 || newSlope == 24 || newSlope == 48)
    {
        setOrder(newSlope / 6, runPrepare);
    }
}

void LRFilter::setOrder(unsigned int newOrder, bool runPrepare)
{
    if (newOrder == 2 || newOrder == 4 || newOrder == 8)
    {
        order = newOrder;
        if (runPrepare)
            prepare();
    }
}

unsigned int LRFilter::getSlope() { return 6 * order; }
unsigned int LRFilter::getOrder() { return order; }

void LRFilter::setSampleRate(double newFs, bool runPrepare)
{
    Fs = newFs;
    if (runPrepare)
        prepare(); 
}

double LRFilter::getSampleRate() { return Fs; }

void LRFilter::setNumChannels(unsigned int newNumChannels)
{
    numChannels = newNumChannels;
    reset();
}

unsigned int LRFilter::getNumChannels() { return numChannels; }

void LRFilter::reset()
{
    if (loX->getNumChannels() != numChannels || loX->getNumSamples() != order)
        loX->setSize(numChannels, order);

    if (hiX->getNumChannels() != numChannels || hiX->getNumSamples() != order)
        hiX->setSize(numChannels, order);

    if (loY->getNumChannels() != numChannels || loY->getNumSamples() != order)
        loY->setSize(numChannels, order);

    if (hiY->getNumChannels() != numChannels || hiY->getNumSamples() != order)
        hiY->setSize(numChannels, order);

    loX->clear();
    hiX->clear();
    loY->clear();
    hiY->clear();
}

void LRFilter::prepare()
{
    switch (order)
    {
        case 2:
            prepare2ndOrder();
            break;
        case 4:
            prepare4thOrder();
            break;
        case 8:
            prepare8thOrder();
            break;
        default:
            throw new _exception;
    }
}

void LRFilter::process(juce::AudioBuffer<float>* toLoPass, juce::AudioBuffer<float>* toHiPass)
{
    loPass(toLoPass);
    hiPass(toHiPass);
}

void LRFilter::loPass(juce::AudioBuffer<float>* toLoPass)
{
    switch (order)
    {
        case 2:
            loPass2(toLoPass);
            break;
        case 4:
            loPass4(toLoPass);
            break;
        case 8:
            loPass8(toLoPass);
            break;
        default:
            throw new _exception;
    }
}

void LRFilter::hiPass(juce::AudioBuffer<float>* toHiPass)
{
    switch (order) 
    {
        case 2:
            hiPass2(toHiPass);
            break;
        case 4:
            hiPass4(toHiPass);
            break;
        case 8:
            hiPass8(toHiPass);
            break;
        default:
            throw new _exception;
    }
}

void LRFilter::prepare2ndOrder()
{
    // TODO
}

void LRFilter::prepare4thOrder()
{
    //calculate the first four powers of omega at cutoff freq
    double omega[4] = { 0.0, 0.0, 0.0, 0.0 };
    omega[0] = 2 * juce::MathConstants<double>::pi * cutoff;
    omega[1] = omega[0] * omega[0];     //omega[0] ^ 2
    omega[2] = omega[1] * omega[0];     //omega[0] ^ 3
    omega[3] = omega[1] * omega[1];     //omega[0] ^ 4

    //calculate the first four powers of the gain
    double k[4] = { 0.0, 0.0, 0.0, 0.0 };
    k[0] = omega[0] / std::tan(juce::MathConstants<double>::pi * cutoff / Fs);
    k[1] = k[0] * k[0];     // k[0] ^ 2
    k[2] = k[1] * k[0];     // k[0] ^ 3
    k[3] = k[1] * k[1];     // k[0] ^ 4

    // compute intermediate values
    double tmp1 = juce::MathConstants<double>::sqrt2 * omega[2] * k[0];
    double tmp2 = juce::MathConstants<double>::sqrt2 * omega[0] * k[2];
    double tmp_a = 4 * omega[1] * k[1] + 2 * tmp1 + k[3] + 2 * tmp2 + omega[3];

    // compute b values
    b[0] = 0;
    b[1] = (4 * (omega[3] + tmp1 - k[3] - tmp2)) / tmp_a;
    b[2] = (6 * omega[3] - 8 * omega[1] * k[1] + 6 * k[3]) / tmp_a;
    b[3] = (4 * (omega[3] - tmp1 + tmp2 - k[3])) / tmp_a;
    b[4] = (k[3] - 2 * tmp1 + omega[3] - 2 * tmp2 + 4 * omega[1] * k[1]) / tmp_a;

    // compute a values for lopass
    loA[0] = omega[3] / tmp_a;
    loA[1] = 4 * omega[3] / tmp_a;
    loA[2] = 6 * omega[3] / tmp_a;
    loA[3] = loA[1];
    loA[4] = loA[0];

    //compute a values for hipass
    hiA[0] = k[3] / tmp_a;
    hiA[1] = -4 * k[3] / tmp_a;
    hiA[2] = 6 * k[3] / tmp_a;
    hiA[3] = hiA[1];
    hiA[4] = hiA[0];
}

void LRFilter::prepare8thOrder()
{
    // TODO
}

void LRFilter::loPass2(juce::AudioBuffer<float>* buffer)
{
    // TODO
}

void LRFilter::loPass4(juce::AudioBuffer<float>* buffer)
{
    for (unsigned int channel{ 0 }; channel < numChannels; ++channel)
    {
        auto data = buffer->getWritePointer(channel);
        auto inputMemory = loX->getWritePointer(channel);
        auto outputMemory = loY->getWritePointer(channel);
        for (unsigned int sampleNum{ 0 }; sampleNum < buffer->getNumSamples(); ++sampleNum)
        {
            //calculate the output sample
            float input = data[sampleNum];
            float output = loA[0] * input
                + loA[1] * inputMemory[0]
                + loA[2] * inputMemory[1]
                + loA[3] * inputMemory[2]
                + loA[4] * inputMemory[3]
                - b[1] * outputMemory[0]
                - b[2] * outputMemory[1]
                - b[3] * outputMemory[2]
                - b[4] * outputMemory[3];

            //propogate the filter's memory
            inputMemory[3] = inputMemory[2];
            inputMemory[2] = inputMemory[1];
            inputMemory[1] = inputMemory[0];
            inputMemory[0] = input;
            outputMemory[3] = outputMemory[2];
            outputMemory[2] = outputMemory[1];
            outputMemory[1] = outputMemory[0];
            outputMemory[0] = output;

            //write the value to the buffer
            data[sampleNum] = output;
        }
    }
}

void LRFilter::loPass8(juce::AudioBuffer<float>* buffer)
{
    // TODO
}

void LRFilter::hiPass2(juce::AudioBuffer<float>* buffer)
{
    // TODO
}

void LRFilter::hiPass4(juce::AudioBuffer<float>* buffer)
{
    for (unsigned int channel{ 0 }; channel < numChannels; ++channel)
    {
        auto data = buffer->getWritePointer(channel);
        auto inputMemory = loX->getWritePointer(channel);
        auto outputMemory = loY->getWritePointer(channel);
        for (unsigned int sampleNum{ 0 }; sampleNum < buffer->getNumSamples(); ++sampleNum)
        {
            //calculate the output sample
            float input = data[sampleNum];
            float output = hiA[0] * input
                + hiA[1] * inputMemory[0]
                + hiA[2] * inputMemory[1]
                + hiA[3] * inputMemory[2]
                + hiA[4] * inputMemory[3]
                - b[1] * outputMemory[0]
                - b[2] * outputMemory[1]
                - b[3] * outputMemory[2]
                - b[4] * outputMemory[3];

            //propogate the filter's memory
            inputMemory[3] = inputMemory[2];
            inputMemory[2] = inputMemory[1];
            inputMemory[1] = inputMemory[0];
            inputMemory[0] = input;
            outputMemory[3] = outputMemory[2];
            outputMemory[2] = outputMemory[1];
            outputMemory[1] = outputMemory[0];
            outputMemory[0] = output;

            //write the value to the buffer
            data[sampleNum] = output;
        }
    }
}

void LRFilter::hiPass8(juce::AudioBuffer<float>* buffer)
{
    // TODO
}
