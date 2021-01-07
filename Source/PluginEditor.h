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
    juce::Label lopassfilter, hipassfilter, title;

    const juce::String LINK_TEXT = juce::String("<- LINK ->");
    const juce::String UNLINK_TEXT = juce::String("<- UNLINK ->");

    const juce::Colour RICH_BLACK = juce::Colour(0x11, 0x35, 0x37);
    const juce::Colour CHARCOAL = juce::Colour(0x37, 0x50, 0x5C);
    const juce::Colour MAUVE_TAUPE = juce::Colour(0x96, 0x61, 0x6B);
    const juce::Colour ULTRA_RED = juce::Colour(0xF7, 0x6F, 0x8E);
    const juce::Colour BLANCHED_ALMOND = juce::Colour(0xFF, 0xEA, 0xD0);

    void setupLinkButton();
    void setupSlopeButtons();
    void setupFrequencySliders();

    void switchSlopeButtons(unsigned int newSlope);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CombinerAudioProcessorEditor)
};
