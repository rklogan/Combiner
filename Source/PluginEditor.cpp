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

void CombinerAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) 
{
    if (slider == &lpfFreqSlider || slider == &hpfFreqSlider)
    {
        bool linked = audioProcessor.getLinked();
        double newVal = slider->getValue();

        if (linked) {
            audioProcessor.setBothCutoffFrequencies(newVal, false, true);
            lpfFreqSlider.setValue(newVal);
            hpfFreqSlider.setValue(newVal);
        }
        else if (slider == &lpfFreqSlider)
            audioProcessor.setLowPassCutoff(newVal, false, true);
        else if (slider == &hpfFreqSlider)
            audioProcessor.setHighPassCutoff(newVal, false, true);
    }
}

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
