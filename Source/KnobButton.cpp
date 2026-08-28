/*
  ==============================================================================

    KnobButton.cpp
    Created: 19 Aug 2026 6:37:30am
    Author:  hraha

  ==============================================================================
*/

#include <JuceHeader.h>
#include "KnobButton.h"

//==============================================================================
KnobButton::KnobButton(juce::String title, double _min, double _max): min(_min), max(_max)
{
    setSize(300, 400);
   
    //////////////////////////////////////// knob////////////////////////////////////////////
    knob.setLookAndFeel(&knobLookandFeel);
    knob.setSliderStyle(juce::Slider::Rotary);
    knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(knob);
    knob.setRange(min, max);
    knob.setValue(0.0);
    knob.onValueChange = [this]() { textEditor.setText(String(knob.getValue(),3));};

    ///////////////////////////////////////text editor //////////////////////////////////////
    addAndMakeVisible(textEditor);
    textEditor.setText("0.000", false);
    textEditor.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::black);
    textEditor.setJustification(juce::Justification::centred);
    textEditor.onReturnKey = [this]() {
            if (isfinalInputValid())
            {
                knob.setValue(textEditor.getText().getDoubleValue());
            }
        };
    textEditor.onTextChange = [this]() {
            if (!isEditingInputValid())
            {
                textEditor.undo();
            }
        };

    //////////////////////////////////////group component ////////////////////////////////////
    addAndMakeVisible(groupComponent);
    groupComponent.setText(title);
    groupComponent.setTextLabelPosition(juce::Justification::centred);

}

KnobButton::~KnobButton()
{
    knob.setLookAndFeel(nullptr);
}

void KnobButton::paint (juce::Graphics& g)
{

}

void KnobButton::resized()
{

    auto rowH = getHeight() / 12;
    auto margin = 8;
    auto width = getWidth()- margin*2;

    //label.setBounds(margin, 0, width, rowH*3);
    groupComponent.setBounds(getLocalBounds());

    knob.setBounds(margin+(width/2)-(rowH*3), rowH*2.5, rowH * 6., rowH*6);

    textEditor.setBounds(margin, rowH*9, width, rowH*2.5);
    textEditor.setFont(14.0f);
}

void KnobButton::setRange(double _min, double _max)
{
    if (max > min && min >=0 )
    {
        min = _min;
        max = _max;
        knob.setRange(min, max);
    }
}

bool KnobButton::isfinalInputValid() const
{
    return std::regex_match(textEditor.getText().toStdString(), finalPattern);
}

bool KnobButton::isEditingInputValid() const
{
    return std::regex_match(textEditor.getText().toStdString(), editingPattern);
}
const juce::Slider* KnobButton::getSliderPointer()const
{
    return &knob;
}

void KnobButton::addListener(juce::Slider::Listener* listener)
{
    knob.addListener(listener);
}

void KnobButton::setKnobValue(double value)
{
    knob.setValue(value);
}