/*
  ==============================================================================

    CueButton.h
    Created: 21 Aug 2026 8:07:05am
    Author:  hraha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CueEditForm.h"
#include <functional>

//==============================================================================
/*
*/
class CueButton  : public juce::Component
{
public:
    CueButton();
    ~CueButton() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    /**it is a call back to receive data from CueEditCutton (grandchild) when its save button is clicked  update the mainButton 
    and send data to DeckGui (grandparent). First Argument is colour and second represents name*/
    void setCueButtonEditedCallback(std::function<void(std::string, std::string)> callback);

    /**It is a call back to send notification to the DeckGUI when its addButton is clicked. */
    void setCueButtonAddCallback(std::function<void()> callback);

    /**It is a call back to send notification to the DeckGUI when its removeButton is clicked. */
    void setCueButtonRemoveCallback(std::function<void()> callback);

    /**set markedTime*/
    void setMarkedTime(double time);

    /**get marked time. The default is -1 indicating that no time is marked.*/
    double getMarkedTime() const;

    /***set the colour of mainButton */
    void setCueButtonColour(juce::Colour colour);

    /**manage the enable status of buttons. 
    -false disables all buttons.
    -true disables add and enables edit and remove button if component has markedTime not equal to -1
     and viceversa if markedTime is equal to 1*/
    void setEnableButtons(bool statusTarget);

    /**returns a const pointer to the button*/
    const juce::Button* getButtonPointer()const;

    /**adds Listener to the button*/
    void addListener(juce::Button::Listener* listener);

    /** this function toggles between enabled status of removeButton and editButton against addButton
        true makes addButton be disabled and editButton and addButton enabled and vice versa
    */
    void toggleEnablingStatusOfChildButtons(bool targetStatus);

    /**set the name of the button to display*/
    void setCueButtonName(juce::String name);
private:

    juce::Image editImage;
    juce::Image removeImage;
    juce::Image addImage;
        

    juce::ImageButton removeButton;
    juce::ImageButton editButton;
    juce::ImageButton addButton;
    juce::Label nameLabel;
    juce::TextButton mainButton;

    double markedTime = -1.0;

    bool status = true;


    /**this function is executed when save button in an CueEditForm is clicked*/
    std::function<void( std::string, std::string)> cueButtonEditedCallback;

    /**this function is executed when add button is clicked*/
    std::function<void()> cueButtonAddCallback;

    /**this function is executed when remove button is clicked*/
    std::function<void()> cueButtonRemoveCallback;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CueButton)
};
