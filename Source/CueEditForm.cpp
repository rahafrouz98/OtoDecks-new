/*
  ==============================================================================

    CueEditForm.cpp
    Created: 21 Aug 2026 1:46:59pm
    Author:  hraha

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CueEditForm.h"

//==============================================================================
CueEditForm::CueEditForm(juce::Colour preSelectedColour, juce::String preSelectedName ): selectedColour(preSelectedColour), selectedCueName(preSelectedName)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setSize(300, 200);
    
    //////////////////////////////////////////////// Text Editor /////////////////////////////////////
    addAndMakeVisible(textEditor);
    textEditor.setMultiLine(false);
    textEditor.setTextToShowWhenEmpty("Type the cue button name here.", juce::Colours::lightgrey.withAlpha(0.5f));
    textEditor.clear();
    textEditor.setJustification(juce::Justification::centredLeft);
    textEditor.setText(preSelectedName, dontSendNotification);

    ////////////////////////////////////////////// Colour buttons //////////////////////////////////
    for (int i = 0; i < colourButtons.size(); ++i)
    {
        juce::Colour colour = juce::Colour::fromString( "#"+CueEditForm::convertCueColourToString(static_cast<CueEditForm::CueColour>(i)));
        colourButtons[i].setColour(juce::TextButton::buttonColourId, colour);
        addAndMakeVisible(colourButtons[i]);
        if (colour == preSelectedColour)
        {
            //it is used to highlight the button when it is selected
            colourButtons[i].getProperties().set("isSelected", "true");
            colourSelectedIndex = i;
        }
   

        colourButtons[i].onClick = [this,colour, i]() {
                selectedColour = colour; 
                if (colourSelectedIndex >= 0)
                {
                    colourButtons[colourSelectedIndex].getProperties().set("isSelected", "false");
                }
                colourButtons[i].getProperties().set("isSelected", "true");
                colourSelectedIndex = i;
            };

        colourButtons[i].setLookAndFeel(&colourButtonLookAndFeel);

        textEditor.onTextChange = [this]() { selectedCueName = textEditor.getText(); };

    }

    ///////////////////////////////////////////// Save   ////////////////////////////////////
    addAndMakeVisible(saveButton);

    ////////////////////////////////////////////  Cancel ////////////////////////////////////////
    addAndMakeVisible(cancelButton);
    cancelButton.setWantsKeyboardFocus(true);
    cancelButton.onClick = [this]() {
            juce::CallOutBox* parentCalloutBox = this->findParentComponentOfClass<juce::CallOutBox>();
            if (parentCalloutBox != nullptr)
            {
                parentCalloutBox->dismiss();
            }
        };

    startTimer(100);
}

CueEditForm::~CueEditForm()
{
    stopTimer();
}

void CueEditForm::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
}

void CueEditForm::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    auto area = getLocalBounds();

    ///////////////////////////////////////////////////////////////text editor //////////////////////////////////////
    auto textEditArea = area.removeFromTop(static_cast<int>(getHeight() / 5));
    textEditor.setBounds(textEditArea.withSizeKeepingCentre(static_cast<int>(textEditArea.getWidth() ), 
                                                            static_cast<int>(textEditArea.getHeight() * 0.9f)));

    ////////////////////////////////////////////////////////////// colour buttons //////////////////////////////////////
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

    ///////////////////////////////////////////////////////////////// Save Button ////////////////////////////////////////////
    auto saveArea = area.removeFromLeft(getWidth() / 2);
    saveButton.setBounds(saveArea.withSizeKeepingCentre(static_cast<int>(saveArea.getWidth()),
                                                        static_cast<int>(saveArea.getHeight() * 0.9f)));

    saveButton.onClick = [this]() {saveButtonCallback(selectedColour, selectedCueName); };

    //////////////////////////////////////////////////////////////// cancel button //////////////////////////////////////////
    cancelButton.setBounds(area.withSizeKeepingCentre(static_cast<int>(area.getWidth()),
                                                      static_cast<int>(area.getHeight() * 0.9f)));  
}

std::string CueEditForm::convertCueColourToString(CueEditForm::CueColour colour)
{
    switch (colour)
    {
    case ffff0000: // red
        return "ffffa500";
    case ff008000: // green
        return "ff008000";
    case ff0000ff: //blue
        return "ff0000ff";
    case ffffff00: // yellow
        return "ffffff00";
    case ffffc0cb: // pink
        return "ffffc0cb";
    case ffffa500: // orange
        return "ffffa500";
    case ff800080: // purple
        return "ff800080";
    case ffadd8e6: // lightblue
        return "ffadd8e6";
    default:
        return "black";
    }
}

CueEditForm::CueColour CueEditForm::convertStringToCueColour(std::string colour)
{
    if (colour == "ffff0000") //red
        return ffff0000;
    if (colour == "ff008000") //green
        return ff008000;
    if (colour == "ff0000ff") //blue
        return ff0000ff;
    if (colour == "ffffff00") //yellow
        return ffffff00;
    if (colour == "ffffc0cb") //pink
        return ffffc0cb;
    if (colour == "ffffa500") //orange
        return ffffa500;
    if (colour == "ff800080") //purple
        return ff800080;
    if (colour == "ffadd8e6") //lightblue
        return ffadd8e6;
    else
        return Undefined;
}

void CueEditForm::timerCallback()
{
    //set foucus on cancel button
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



