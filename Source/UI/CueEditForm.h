#pragma once

#include <JuceHeader.h>
#include <functional>
#include "ColourButtonLookAndFeel.h"

class CueEditForm  : public juce::Component, juce::Timer
{
public:
    /**creates a an edit form including a text editor to name the cue button 
    and a group of colours to select the cueButton colour.
    */
    CueEditForm( juce::Colour preSelectedColour = juce::Colours::transparentBlack, 
                 juce::String preSelectedName = "");
    ~CueEditForm() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    /**set the focus on cancel button when form opens*/
    void timerCallback() override;

    enum CueColour {
        Red,
        Green,
        Blue,
        Yellow,
        Pink,
        Orange,
        Purple,
        Lightblue,
        Undefined
    };

    /**converts CueColours to juce::Colour*/
    static juce::Colour convertCueColourToJuceColour(CueEditForm::CueColour colour);

    /**converts juce::Colour to CueColours */
    static CueEditForm::CueColour convertJuceColourToCueColour(juce::Colour);

    /**set callback function for save button. it takes a call back function as the argument. The
    callback function takes two arguments to represent colour and name and this data is 
    transfered to the parent through the callback*/
    void setSaveButtonCallBack(std::function <void(juce::Colour, juce::String)> callback);

private:
    ColourButtonLookAndFeel colourButtonLookAndFeel;

    std::array<juce::TextButton, 8> colourButtons;
    juce::TextEditor textEditor;

    juce::TextButton saveButton{ "Save" };
    juce::TextButton cancelButton{ "Cancel" };

    /**it is to save the colour that is selected at the time of clicking on save 
    button to send it to the parent by callback*/
    juce::Colour selectedColour;

    /**it is to save the name that is selected at the time of clicking on save
    button to send it to the parent by callback*/
    juce::String selectedCueName;

    /**it is used to hold the track of last selected colourButton (before selecting a new one) index 
    and is used to update the property called isSelected for this button as false */
    int colourSelectedIndex ;

    /**this is a callback function to send the selectedCueName and selectedColour to the parent
    component (CueButton) when saveButton is clicked*/
    std::function<void(juce::Colour, juce::String)> saveButtonCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CueEditForm)
};
