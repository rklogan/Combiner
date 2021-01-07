/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class CombinerAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                      public juce::Slider::Listener,
                                      public juce::Button::Listener
{
public:
    CombinerAudioProcessorEditor (CombinerAudioProcessor&);
    ~CombinerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonStateChanged(juce::Button* button);
    void buttonClicked(juce::Button* button);

    void sliderValueChanged(juce::Slider* slider);
    void sliderDragStarted(juce::Slider* slider);
    void sliderDragEnded(juce::Slider* slider);

private:
    CombinerAudioProcessor& audioProcessor;
    juce::TextButton linkButton;
    juce::OwnedArray<juce::TextButton> slopeButtons;
    juce::Slider lpfFreqSlider, hpfFreqSlider;
    juce::Label lopassfilter, hipassfilter;

    const juce::String LINK_TEXT = juce::String("<- LINK ->");
    const juce::String UNLINK_TEXT = juce::String("<- UNLINK ->");

    void setupLinkButton();
    void setupSlopeButtons();
    void setupFrequencySliders();

    void switchSlopeButtons(unsigned int newSlope);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CombinerAudioProcessorEditor)
};
