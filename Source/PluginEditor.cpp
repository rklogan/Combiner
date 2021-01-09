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

    title.setFont(juce::Font(30.0f, juce::Font::bold));
    title.setText("COMBINER", juce::dontSendNotification);
    title.setColour(juce::Label::textColourId, POWDER_BLUE);
    title.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(title);

    lopassfilter.setFont(juce::Font(25.0f, juce::Font::bold));
    lopassfilter.setText("Low-Pass Filter", juce::dontSendNotification);
    lopassfilter.setColour(juce::Label::textColourId, POWDER_BLUE);
    lopassfilter.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lopassfilter);

    hipassfilter.setFont(juce::Font(25.0f, juce::Font::bold));
    hipassfilter.setText("High-Pass Filter", juce::dontSendNotification);
    hipassfilter.setColour(juce::Label::textColourId, POWDER_BLUE);
    hipassfilter.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(hipassfilter);

    getLookAndFeel().setColour(juce::Slider::thumbColourId, HONEYDEW);
    getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, CALEDON_BLUE);
    getLookAndFeel().setColour(juce::Slider::rotarySliderOutlineColourId, POWDER_BLUE);
    getLookAndFeel().setColour(juce::Slider::textBoxTextColourId, POWDER_BLUE);
    getLookAndFeel().setColour(juce::Slider::textBoxOutlineColourId, CALEDON_BLUE);

    getLookAndFeel().setColour(juce::TextButton::buttonColourId, POWDER_BLUE);
    getLookAndFeel().setColour(juce::TextButton::buttonOnColourId, CALEDON_BLUE);
    getLookAndFeel().setColour(juce::TextButton::textColourOffId, CALEDON_BLUE);
    getLookAndFeel().setColour(juce::TextButton::textColourOnId, POWDER_BLUE);

    linkButtonAttachment = new juce::AudioProcessorValueTreeState::ButtonAttachment(
        audioProcessor.parameters, LINKED_ID, linkButton
    );
    audioProcessor.parameters.addParameterListener(LINKED_ID, this);
    audioProcessor.parameters.addParameterListener(SLOPE_ID, this);

    lpfSliderAttachment = new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.parameters, LOPASS_FREQ_ID, lpfFreqSlider
    );
    hpfSliderAttachment = new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.parameters, HIPASS_FREQ_ID, hpfFreqSlider
    );
    audioProcessor.parameters.addParameterListener(LOPASS_FREQ_ID, this);
    audioProcessor.parameters.addParameterListener(HIPASS_FREQ_ID, this);
    
    setSize (600, 300);
}

CombinerAudioProcessorEditor::~CombinerAudioProcessorEditor()
{
}

//==============================================================================
void CombinerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(PRUSSIAN_BLUE);
}

void CombinerAudioProcessorEditor::resized()
{
    const int width = getLocalBounds().getWidth();
    const int height = getLocalBounds().getHeight();
    const int oneThirdWidth = width / 3;
    const int oneNinthWidth = oneThirdWidth / 3;
    const int twoThirdsWidth = oneThirdWidth * 2;
    const int oneThirdHeight = height / 3;
    const int twoThirdsHeight = 2 * oneThirdHeight;
    const int oneSixthHeight = oneThirdHeight / 2;

    juce::Rectangle<int> titleArea = juce::Rectangle<int>(0, 0, width, oneSixthHeight);
    title.setBounds(titleArea);

    juce::Rectangle<int> leftLabelArea = juce::Rectangle<int>(0, oneSixthHeight, oneThirdWidth, oneSixthHeight);
    lopassfilter.setBounds(leftLabelArea);

    juce::Rectangle<int> slopeButtonArea = juce::Rectangle<int>(oneThirdWidth, oneSixthHeight, oneThirdWidth, oneSixthHeight + oneThirdHeight);
    for (auto* b : slopeButtons)
        b->setBounds(slopeButtonArea.removeFromLeft(oneNinthWidth));

    juce::Rectangle<int> rightLabelArea = juce::Rectangle<int>(twoThirdsWidth, oneSixthHeight, oneThirdWidth, oneSixthHeight);
    hipassfilter.setBounds(rightLabelArea);

    juce::Rectangle<int> lpfSliderArea = juce::Rectangle<int>(0, oneThirdHeight, oneThirdWidth, twoThirdsHeight);
    lpfFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, oneThirdWidth, 18);
    lpfFreqSlider.setBounds(lpfSliderArea);

    juce::Rectangle<int> hpfSliderArea = juce::Rectangle<int>(twoThirdsWidth, oneThirdHeight, oneThirdWidth, twoThirdsHeight);
    hpfFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, oneThirdWidth, 18);
    hpfFreqSlider.setBounds(hpfSliderArea);

    juce::Rectangle<int> linkButtonArea = juce::Rectangle<int>(oneThirdWidth, twoThirdsHeight, oneThirdWidth, oneThirdHeight);
    linkButton.setBounds(linkButtonArea);
}

void CombinerAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == LINKED_ID)
    {
        bool newState = newValue < 0.5f;
        if (!newState)
            linkButton.setButtonText(UNLINK_TEXT);
        else
            linkButton.setButtonText(LINK_TEXT);
    }
    else if (parameterID == SLOPE_ID)
    {
        unsigned int idx = round(newValue);
        for (unsigned int i{ 0 }; i < 3; ++i)
            slopeButtons[i]->setToggleState(i == idx, juce::dontSendNotification);
        audioProcessor.resetAndPrepare();
    }
    else if (parameterID == LOPASS_FREQ_ID || parameterID == HIPASS_FREQ_ID)
    {
        lopassfilter.setText("here", juce::dontSendNotification);
        if (*(audioProcessor.parameters.getRawParameterValue(LINKED_ID)) > 0.5f)
        {
            if (parameterID == LOPASS_FREQ_ID) {
                audioProcessor.parameters.getRawParameterValue(HIPASS_FREQ_ID)->store(newValue);
                hpfFreqSlider.setValue(newValue, juce::dontSendNotification);
            }
            else {
                audioProcessor.parameters.getRawParameterValue(LOPASS_FREQ_ID)->store(newValue);
                lpfFreqSlider.setValue(newValue, juce::dontSendNotification);
            }
        }
        audioProcessor.updateFrequencies(false, true);
    }
}

void CombinerAudioProcessorEditor::buttonStateChanged(juce::Button* button){}
void CombinerAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &linkButton)
    {
        bool newState = !linkButton.getToggleState();// ? 1.0f : 0.0f;
        linkButton.setToggleState(newState, juce::dontSendNotification);
        if (!newState)
            linkButton.setButtonText(UNLINK_TEXT);
        else
            linkButton.setButtonText(LINK_TEXT);
    }
    else
    {
        int index{ 0 };
        for (; index < 3; ++index)
            if (slopeButtons.getUnchecked(index) == button) break;
        audioProcessor.parameters.getRawParameterValue(SLOPE_ID)->store(index);
        audioProcessor.resetAndPrepare(); //TODO just prepare?
    }
}

void CombinerAudioProcessorEditor::setupLinkButton()
{
    bool linked = *(audioProcessor.parameters.getRawParameterValue(LINKED_ID)) < 0.5f;
    if (!linked)
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

    unsigned int idx = round(audioProcessor.parameters.getRawParameterValue(SLOPE_ID)->load());
    for (unsigned int i{ 0 }; i < 3; ++i)
        slopeButtons[i]->setToggleState(i == idx, juce::dontSendNotification);
}

void CombinerAudioProcessorEditor::setupFrequencySliders()
{
    lpfFreqSlider.setDoubleClickReturnValue(true, 750.0f);
    lpfFreqSlider.setRange(20.0, 20000.0);
    lpfFreqSlider.setNumDecimalPlacesToDisplay(1);
    lpfFreqSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    lpfFreqSlider.setTextValueSuffix(juce::String("Hz"));
    lpfFreqSlider.setValue(audioProcessor.parameters.getRawParameterValue(LOPASS_FREQ_ID)->load());
    lpfFreqSlider.setSkewFactor(0.25f);

    hpfFreqSlider.setDoubleClickReturnValue(true, 750.0f);
    hpfFreqSlider.setRange(20.0, 20000.0);
    hpfFreqSlider.setNumDecimalPlacesToDisplay(1);
    hpfFreqSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    hpfFreqSlider.setTextValueSuffix(juce::String("Hz"));
    hpfFreqSlider.setValue(audioProcessor.parameters.getRawParameterValue(HIPASS_FREQ_ID)->load());
    hpfFreqSlider.setSkewFactor(0.25f);

    addAndMakeVisible(lpfFreqSlider);
    addAndMakeVisible(hpfFreqSlider);

    audioProcessor.updateFrequencies(true, true);
}