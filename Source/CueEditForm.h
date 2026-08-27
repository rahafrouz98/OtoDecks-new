/*
  ==============================================================================

    CueEditForm.h
    Created: 21 Aug 2026 1:46:59pm
    Author:  hraha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <functional>
#include "ColourButtonLookAndFeel.h"
//==============================================================================
/*
*/
class CueEditForm  : public juce::Component, juce::Timer
{
public:
    /**creates a an edit form including a text editor to name the cue button and a group of colours to select the cueButton colour
    preSelectedColour -1 means no colour is selected yet
    */
    CueEditForm( juce::Colour preSelectedColour = juce::Colours::transparentBlack, juce::String preSelectedName = "");
    ~CueEditForm() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    /**set the focus on cancel button when form opens*/
    void timerCallback() override;

    enum CueColour {
        ffff0000,//red
        ff008000,//green
        ff0000ff,// blue
        ffffff00,//yellow
        ffffc0cb,//pink
        ffffa500,//orange
        ff800080,//purple
        ffadd8e6,//lightblue
        Undefined
    };

    /**converts CueColours to std::string*/
    static std::string convertCueColourToString(CueEditForm::CueColour colour);

    /**converts std::string to CueColours */
    static CueEditForm::CueColour convertStringToCueColour(std::string colour);

    /**set callback function for save button*/
    void setSaveButtonCallBack(std::function <void(juce::Colour, juce::String)> callback);

private:
    ColourButtonLookAndFeel colourButtonLookAndFeel;

    std::array<juce::TextButton, 8> colourButtons;
    juce::TextEditor textEditor;

    juce::TextButton saveButton{ "Save" };
    juce::TextButton cancelButton{ "Cancel" };

    juce::Colour selectedColour;
    juce::String selectedCueName;

    int colourSelectedIndex;

    /**this is a callback function to send the selectedCueName and selectedColour to the parent component(CueButton) 
    when saveButton is clicked*/
    std::function<void(juce::Colour, juce::String)> saveButtonCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CueEditForm)
};
