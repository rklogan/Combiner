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
    setResizable(true, true);
    setupLinkButton();
    setupSlopeButtons();
    setupFrequencySliders();
    lopassfilter.setFont(juce::Font(25.0f, juce::Font::bold));
    lopassfilter.setText("Low-Pass Filter", juce::dontSendNotification);
    lopassfilter.setColour(juce::Label::textColourId, juce::Colours::black);
    lopassfilter.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lopassfilter);
    hipassfilter.setFont(juce::Font(25.0f, juce::Font::bold));
    hipassfilter.setText("High-Pass Filter", juce::dontSendNotification);
    hipassfilter.setColour(juce::Label::textColourId, juce::Colours::black);
    hipassfilter.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(hipassfilter);
    setSize (600, 300);
}

CombinerAudioProcessorEditor::~CombinerAudioProcessorEditor()
{
}

//==============================================================================
void CombinerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void CombinerAudioProcessorEditor::resized()
{
    const int width = getLocalBounds().getWidth();
    const int height = getLocalBounds().getHeight();

    juce::Rectangle<int> topLeft = juce::Rectangle<int>(0, 0, width / 3, height/2);
    juce::Rectangle<int> btmLeft = juce::Rectangle<int>(0, height / 2, width / 3, height / 2);
    juce::Rectangle<int> topMid = juce::Rectangle<int>(width / 3, 0, width / 3, height / 2);
    juce::Rectangle<int> btmMid = juce::Rectangle<int>(width / 3, height / 2, width / 3, height / 2);
    juce::Rectangle<int> topRight = juce::Rectangle<int>(2 * width / 3, 0, width / 3, height / 2);
    juce::Rectangle<int> btmRight = juce::Rectangle<int>(2 * width / 3, height / 2, width / 3, height / 2);

    lpfFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, width / 3, 18);
    lpfFreqSlider.setBounds(btmLeft);
    hpfFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, width / 3, 18);
    hpfFreqSlider.setBounds(btmRight);
    
    for (auto* b : slopeButtons)
        b->setBounds(topMid.removeFromLeft(width / 9));

    linkButton.setBounds(btmMid);

    lopassfilter.setBounds(topLeft);
    hipassfilter.setBounds(topRight);
}

void CombinerAudioProcessorEditor::buttonStateChanged(juce::Button* button){}
void CombinerAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &linkButton)
    {
        bool newState = !linkButton.getToggleState();
        linkButton.setToggleState(newState, juce::dontSendNotification);
        audioProcessor.setLinked(newState);
        if (newState)
            linkButton.setButtonText(UNLINK_TEXT);
        else
            linkButton.setButtonText(LINK_TEXT);
    }
    else
    {
        int index{ 0 };
        for (; index < 3; ++index) 
            if (slopeButtons.getUnchecked(index) == button) break;
        
        unsigned int order = pow(2, index + 1);
        audioProcessor.setOrder(order, false, true);
    }
}

void CombinerAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {}
void CombinerAudioProcessorEditor::sliderDragStarted(juce::Slider* slider) {}
void CombinerAudioProcessorEditor::sliderDragEnded(juce::Slider* slider) {}

void CombinerAudioProcessorEditor::setupLinkButton()
{
    bool linked = audioProcessor.getLinked();
    if (linked)
        linkButton.setButtonText(UNLINK_TEXT);
    else
        linkButton.setButtonText(LINK_TEXT);
    linkButton.setToggleState(linked, juce::dontSendNotification);
    linkButton.addListener(this);
    addAndMakeVisible(linkButton);
}

void CombinerAudioProcessorEditor::setupSlopeButtons()
{
    for (unsigned int i{ 0 }; i < 3; ++i)
    {
        unsigned int slope = i == 2 ? 48 : (12 * (i + 1));

        auto* b = slopeButtons.add(new juce::TextButton(juce::String(slope) + " dB/8ve"));

        b->setRadioGroupId(1);
        b->setClickingTogglesState(true);
        b->addListener(this);

        switch (i)
        {
        case 0:
            b->setConnectedEdges(juce::Button::ConnectedOnRight);
            break;
        case 1:
            b->setConnectedEdges(juce::Button::ConnectedOnRight + juce::Button::ConnectedOnLeft);
            break;
        case 2:
            b->setConnectedEdges(juce::Button::ConnectedOnLeft);
            break;
        default:
            break;
        }

        addAndMakeVisible(b);
    }
    
    unsigned int order = audioProcessor.getOrder();
    switch (order)
    {
    case 2:
        slopeButtons.getUnchecked(0)->setToggleState(true, juce::dontSendNotification);
        break;
    case 4:
        slopeButtons.getUnchecked(1)->setToggleState(true, juce::dontSendNotification);
        break;
    case 8:
        slopeButtons.getUnchecked(2)->setToggleState(true, juce::dontSendNotification);
        break;
    default:
        break;
    }
}

void CombinerAudioProcessorEditor::setupFrequencySliders()
{
    lpfFreqSlider.setDoubleClickReturnValue(true, 750.0f);
    lpfFreqSlider.setRange(20.0, 20000.0);
    lpfFreqSlider.setNumDecimalPlacesToDisplay(1);
    lpfFreqSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    lpfFreqSlider.setTextValueSuffix(juce::String("Hz"));
    lpfFreqSlider.setValue(audioProcessor.getLowPassCutoff());
    lpfFreqSlider.setSkewFactor(0.25f);
    lpfFreqSlider.addListener(this);

    hpfFreqSlider.setDoubleClickReturnValue(true, 750.0f);
    hpfFreqSlider.setRange(20.0, 20000.0);
    hpfFreqSlider.setNumDecimalPlacesToDisplay(1);
    hpfFreqSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    hpfFreqSlider.setTextValueSuffix(juce::String("Hz"));
    hpfFreqSlider.setValue(audioProcessor.getHighPassCutoff());
    hpfFreqSlider.setSkewFactor(0.25f);
    hpfFreqSlider.addListener(this);

    addAndMakeVisible(lpfFreqSlider);
    addAndMakeVisible(hpfFreqSlider);
}
