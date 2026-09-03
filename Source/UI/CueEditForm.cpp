#include "CueEditForm.h"

CueEditForm::CueEditForm(juce::Colour preSelectedColour, juce::String preSelectedName ): 
                           selectedColour(preSelectedColour), selectedCueName(preSelectedName)
{
    setSize(300, 200);
    
    //////////////////////////////////////////////// Text Editor /////////////////////////////////////
    addAndMakeVisible(textEditor);
    textEditor.setMultiLine(false);
    textEditor.setTextToShowWhenEmpty("Type the cue button name here.", 
                                       juce::Colours::lightgrey.withAlpha(0.5f));
    textEditor.clear();
    textEditor.setJustification(juce::Justification::centredLeft);
    textEditor.setText(preSelectedName, false);

    ////////////////////////////////////////////// Colour buttons //////////////////////////////////
    for (int i = 0; i < colourButtons.size(); ++i)
    {
        juce::Colour colour = convertCueColourToJuceColour(static_cast<CueColour>(i));
        colourButtons[i].setColour(juce::TextButton::buttonColourId, colour);
        addAndMakeVisible(colourButtons[i]);

        if (colour == preSelectedColour)
        {
            //it is used to highlight the button when it is selected
            colourButtons[i].getProperties().set("isSelected", "true");
            colourSelectedIndex = i;
        }
   
        /**changes the data for selected colour*/
        colourButtons[i].onClick = [this,colour, i]() 
            {
                selectedColour = colour; 
               
                if (colourSelectedIndex >= 0)
                {
                    colourButtons[colourSelectedIndex].getProperties().set("isSelected", "false");
                }
                /** isSelected is to mark the button so when it is true a white boarder is drawn around it*/
                colourButtons[i].getProperties().set("isSelected", "true");
                colourSelectedIndex = i;
            };

        /* it draws the button with  a white boarder around the button when its property called 
        "isSelected" is true*/
        colourButtons[i].setLookAndFeel(&colourButtonLookAndFeel);

        /**updates the selectedCueName as the user is typing inside the text editor*/
        textEditor.onTextChange = [this]() { selectedCueName = textEditor.getText(); };
    }

    ///////////////////////////////////////////// Save   ////////////////////////////////////
    addAndMakeVisible(saveButton);

    ////////////////////////////////////////////  Cancel ////////////////////////////////////////
    addAndMakeVisible(cancelButton);
    /**it moves the focus from tect editor to cancel button so the text editor display a message
    when it is empty (if focus is on the text editor the message does not display)*/
    cancelButton.setWantsKeyboardFocus(true);

    /**closed the box when the cancel is clicked*/
    cancelButton.onClick = [this]() {
            /**finds the parent that component */
            juce::CallOutBox* parentCalloutBox = this->findParentComponentOfClass<juce::CallOutBox>();
            if (parentCalloutBox != nullptr)
            {
                /** Sends a message to the parent of this component to close this component asynchronously */
                parentCalloutBox->dismiss();
            }
        };

    startTimer(100);
}

CueEditForm::~CueEditForm()
{
    stopTimer();
}

void CueEditForm::paint (juce::Graphics&)
{ 
}

void CueEditForm::resized()
{

    auto area = getLocalBounds();

    /////////////////////////////////////////////////text editor /////////////////////////////////////
    auto textEditArea = area.removeFromTop(static_cast<int>(getHeight() / 5));
    textEditor.setBounds(textEditArea.withSizeKeepingCentre(static_cast<int>(textEditArea.getWidth() ), 
                                                            static_cast<int>(textEditArea.getHeight() * 0.9f)));

    /////////////////////////////////////////////// colour buttons ////////////////////////////////////
    // it is a two row table of colour buttons
    auto coloursArea = area.removeFromTop(getHeight() *2 / 3);
    int cueButtonHeight = static_cast<int>(coloursArea.getHeight() / 2);
    int cueButtonWidth = static_cast<int>(coloursArea.getWidth() / 4);

    for (int row = 0; row < 2; ++row)
    {
        auto rowArea = coloursArea.removeFromBottom(cueButtonHeight);
        for (int col = 0; col < 4; ++col)
        {
            int index = row * 4 + col;
            if (index >= colourButtons.size())
            {
                break;
            }
            auto cueButtonArea = rowArea.removeFromLeft(cueButtonWidth);
            colourButtons[index].setBounds(cueButtonArea);

            if (index == colourSelectedIndex)
            {

            }
        }
    }

    //////////////////////////////////////////////// Save Button ////////////////////////////////////////
    auto saveArea = area.removeFromLeft(getWidth() / 2);
    saveButton.setBounds(saveArea.withSizeKeepingCentre(static_cast<int>(saveArea.getWidth()),
                                                        static_cast<int>(saveArea.getHeight() * 0.9f)));

    saveButton.onClick = [this]() {saveButtonCallback(selectedColour, selectedCueName); };

    ////////////////////////////////////////////// cancel button //////////////////////////////////////////
    cancelButton.setBounds(area.withSizeKeepingCentre(static_cast<int>(area.getWidth()),
                                                      static_cast<int>(area.getHeight() * 0.9f)));  
}

juce::Colour CueEditForm::convertCueColourToJuceColour(CueEditForm::CueColour colour)
{
    switch (colour)
    {
    case Red: 
        return juce::Colours::red;
    case Green: 
        return juce::Colours::green;
    case Blue:
        return juce::Colours::blue;
    case Yellow: 
        return juce::Colours::yellow;
    case Pink: 
        return juce::Colours::pink;
    case Orange: 
        return juce::Colours::orange;
    case Purple: 
        return juce::Colours::purple;
    case Lightblue: 
        return juce::Colours::lightblue;
    default:
        return juce::Colours::transparentBlack;
    }
}

CueEditForm::CueColour CueEditForm::convertJuceColourToCueColour(juce::Colour colour)
{
    if (colour == juce::Colours::red) 
        return Red;
    if (colour == juce::Colours::red)
        return Green;
    if (colour == juce::Colours::blue) 
        return Blue;
    if (colour == juce::Colours::yellow) 
        return Yellow;
    if (colour == juce::Colours::pink)
        return Pink;
    if (colour == juce::Colours::orange) 
        return Orange;
    if (colour == juce::Colours::purple)
        return Purple;
    if (colour == juce::Colours::lightblue) 
        return Lightblue;
    else
        return Undefined;
}

void CueEditForm::timerCallback()
{
    //set foucus on cancel button
    /**timer is used to check in inervals and make sure the cancelButton is available
      whith out time app would run into running time error*/
    if (cancelButton.isShowing())
    {
        cancelButton.grabKeyboardFocus();
        if (cancelButton.hasKeyboardFocus(true))
        {
            stopTimer();
        }
    }
}

void CueEditForm::setSaveButtonCallBack(std::function <void(juce::Colour, juce::String)> callback)
{
    saveButtonCallback = callback;
}



