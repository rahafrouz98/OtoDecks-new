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
    
    auto playStopArea = area.removeFromLeft(getWidth() / 5.0f);

    playStopButton.setBounds(playStopArea.withSizeKeepingCentre(playStopArea.getHeight() * 0.4f, 
                                                                playStopArea.getHeight() * 0.4f));

    auto textEditArea = area.removeFromBottom(getHeight()* 2/ 3.0f);
    textEditor.setBounds(textEditArea.withSizeKeepingCentre(textEditArea.getWidth() * 0.8f ,
                                                             textEditArea.getHeight() * 0.8f ));

    addRemoveButton.setBounds(area.removeFromRight(getWidth() / 4.0f).withSizeKeepingCentre(area.getHeight()*0.8,
                                                                 area.getHeight()*0.8));
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
    url = juce::URL{};
    textEditor.setText("");
    playStopButton.setButtonEnabled(false);

}

void SampleLoopButton::setSample(juce::URL _url, String _name)
{
    url = _url;
    textEditor.setText(_name);
    playStopButton.setButtonEnabled(true);
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

void SampleLoopButton::setAddButtonEnabled(bool status)
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
