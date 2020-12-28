/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CombinerAudioProcessorEditor::CombinerAudioProcessorEditor (CombinerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 300);
}

CombinerAudioProcessorEditor::~CombinerAudioProcessorEditor()
{
}

//==============================================================================
void CombinerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    int numIns = audioProcessor.getNumInputChannels();
    int numOuts = audioProcessor.getNumOutputChannels();

    juce::String ni = juce::String(numIns);
    juce::String no = juce::String(numOuts);
    juce::String op = ni + juce::String(" ") + no;
    g.drawFittedText(op, getLocalBounds(), juce::Justification::centred, 1);
}

void CombinerAudioProcessorEditor::resized()
{

}
