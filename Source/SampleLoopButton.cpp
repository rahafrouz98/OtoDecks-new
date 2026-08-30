/*
  ==============================================================================

    SampleLoopButton.cpp
    Created: 25 Aug 2026 11:11:38pm
    Author:  hraha

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SampleLoopButton.h"

//==============================================================================
SampleLoopButton::SampleLoopButton()
{
    setSize(45, 146);

    //////////////////////////// play and stop button ///////////////////////////////
    addAndMakeVisible(playStopButton);
    playStopButton.setButtonEnabled(false);
    playStopButton.addListener(this);


    //////////////////////////// add and remove button /////////////////////////////
    addAndMakeVisible(addRemoveButton);
    addRemoveButton.setButtonEnabled(false);
    addRemoveButton.addListener(this);

    //////////////////////////// text editor /////////////////////////////
    addAndMakeVisible(textEditor);
    textEditor.onTextChange = [this]() {textChangeCallback(textEditor.getText());};

}

SampleLoopButton::~SampleLoopButton()
{
}

void SampleLoopButton::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour(juce::Colours::lightgrey);

    g.drawRoundedRectangle(getLocalBounds().toFloat(), getHeight() / 4.0f, 2.0f);
}

void SampleLoopButton::resized()
{
    auto area = getLocalBounds();
    
    auto playStopArea = area.removeFromLeft(getWidth() / 7.0f);

    playStopButton.setBounds(playStopArea.withSizeKeepingCentre(playStopArea.getHeight() * 0.4f, 
                                                                playStopArea.getHeight() * 0.4f));

    auto textEditArea = area.removeFromLeft(getWidth() * 5 / 7.0f);
    textEditor.setBounds(textEditArea.withSizeKeepingCentre(textEditArea.getWidth() ,
                                                             textEditArea.getHeight() * 0.6f ));

    addRemoveButton.setBounds(area.withSizeKeepingCentre(area.getHeight()*0.4,area.getHeight()*0.4));
}

juce::URL SampleLoopButton::getURL()
{
    return url;
}

void SampleLoopButton::buttonClicked(Button* button)
{

}

void SampleLoopButton::resetButtonData()
{
    DBG("REdet");
    url = juce::URL{};
    textEditor.setText("",false);
    playStopButton.setButtonEnabled(false);

}

void SampleLoopButton::setSampleDataAndButtonsStatus(juce::URL _url, String _name, bool updateAddRemoveMode)
{
    url = _url;
    textEditor.setText(_name, false);

    if (_url != juce::URL{})
    {
        playStopButton.setButtonEnabled(true);
        addRemoveButton.setButtonEnabled(true);
        if (updateAddRemoveMode)
        {
            addRemoveButton.setFirstMode(false);
        }
    }
}

void SampleLoopButton::addListener(juce::Button::Listener* listener)
{
    addRemoveButton.addListener(listener);
    playStopButton.addListener(listener);
}


const juce::Button* SampleLoopButton::getPlayStopButtonPointer()const
{
    return playStopButton.getButtonPointer();
}

const juce::Button* SampleLoopButton::getAddRemoveButtonPointer()const
{
    return addRemoveButton.getButtonPointer();
}
void SampleLoopButton::setTextChangeCallBack(std::function<void(juce::String)> callback)
{
    textChangeCallback = callback;
}

void SampleLoopButton::setAddRemoveButtonEnabled(bool status)
{
    addRemoveButton.setButtonEnabled(status);
}

bool SampleLoopButton::getPlayStopButtonStatus()const
{
    return playStopButton.getStatus();
}

bool SampleLoopButton::getAddRemoveButtonStatus()const
{
    return addRemoveButton.getStatus();
}

int SampleLoopButton::getID()
{
    return buttonID;
}

void SampleLoopButton::setID(int id)
{
    buttonID = id;
}