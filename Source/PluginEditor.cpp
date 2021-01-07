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

    /*juce::String ni = juce::String(numIns);
    juce::String no = juce::String(numOuts);
    juce::String op = ni + juce::String(" ") + no;*/
    /*juce::String lpf_a = juce::String("lpf a: ")
        + juce::String(audioProcessor.a[0][0]) + ' '
        + juce::String(audioProcessor.a[0][1]) + ' '
        + juce::String(audioProcessor.a[0][2]);
    juce::String lpf_b = juce::String("lpf b: ")
        + juce::String(audioProcessor.b[0][1]) + ' '
        + juce::String(audioProcessor.b[0][2]);
    juce::String hpf_a = juce::String("hpf a: ")
        + juce::String(audioProcessor.a[1][0]) + ' '
        + juce::String(audioProcessor.a[1][1]) + ' '
        + juce::String(audioProcessor.a[1][2]);
    juce::String hpf_b = juce::String("hpf b:")
        + juce::String(audioProcessor.b[1][1]) + ' '
        + juce::String(audioProcessor.b[1][2]);
    juce::String op = lpf_a + '\n' + lpf_b + '\n' + hpf_a + '\n' + hpf_b;
    g.drawFittedText(op, getLocalBounds(), juce::Justification::centred, 1);*/
}

void CombinerAudioProcessorEditor::resized()
{

}
