#pragma once

#include <JuceHeader.h>
#include "CueEditForm.h"
#include <functional>

class CueButton  : public juce::Component
{
public:
    CueButton();
    ~CueButton() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    /**Set a callback to receive data from CueEditButton when its save button is clicked.
    it updates the mainButton and send data to DeckGui . It takes a calback function as the argument and
    the callback takes two arguments.First Argument is colour and second represents name*/
    void setCueButtonEditedCallback(std::function<void(juce::Colour, juce::String)> callback);

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
    -true disables add and enables edit and remove button if component has markedTime greater than -1
     and viceversa if markedTime is equal to -1*/
    void setEnableButtons(bool statusTarget);

    /**returns a const pointer to the button*/
    const juce::Button* getButtonPointer()const;

    /**It is an adds Listener to transfer the listener to the  mainButton */
    void addListener(juce::Button::Listener* listener);

    /** this function toggles between enabled status of removeButton and editButton against addButton.
        -True makes addButton disabled and editButton and addButton enabled 
        -False makes addButton enabled and editButton and addButton disabled 
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
    std::function<void( juce::Colour, juce::String)> cueButtonEditedCallback;

    /**this function is executed when add button is clicked*/
    std::function<void()> cueButtonAddCallback;

    /**this function is executed when remove button is clicked*/
    std::function<void()> cueButtonRemoveCallback;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CueButton)
};
