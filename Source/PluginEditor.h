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
class CombinerAudioProcessorEditor  : 
    public juce::AudioProcessorEditor,
    public juce::AudioProcessorValueTreeState::Listener,
    public juce::Button::Listener
{
public:
    CombinerAudioProcessorEditor (CombinerAudioProcessor&);
    ~CombinerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //========================== EVENT LISTENERS ===================================
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    void buttonStateChanged(juce::Button* button) override;
    void buttonClicked(juce::Button* button);

private:
    CombinerAudioProcessor& audioProcessor;

    // UI Elements
    juce::TextButton linkButton;
    juce::OwnedArray<juce::TextButton> slopeButtons;
    juce::Slider lpfFreqSlider, hpfFreqSlider;
    juce::Label lopassfilter, hipassfilter, title;

    // UI Element Listeners
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::ButtonAttachment> linkButtonAttachment;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> lpfSliderAttachment, hpfSliderAttachment;

    // Strings
    const juce::String LINK_TEXT = juce::String("<- LINK ->");
    const juce::String UNLINK_TEXT = juce::String("<- UNLINK ->");

    // Colours
    const juce::Colour HONEYDEW = juce::Colour(0xD8, 0xF1, 0xD0);
    const juce::Colour POWDER_BLUE = juce::Colour(0xA8, 0xDA, 0xDC);
    const juce::Colour CALEDON_BLUE = juce::Colour(0x45, 0x7B, 0x9D);
    const juce::Colour PRUSSIAN_BLUE = juce::Colour(0x1D, 0x35, 0x57);

    /**
    * Helper function to create the link/unlink button
    */
    void setupLinkButton();

    /**
    * Helper function to create the slope button
    */
    void setupSlopeButtons();

    /**
    * Helper function to create the frequency sliders
    */
    void setupFrequencySliders();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CombinerAudioProcessorEditor)
};

