#include "SampleLoopButton.h"

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
    g.setColour(juce::Colours::lightgrey);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), getHeight() / 4.0f, 2.0f);
}

void SampleLoopButton::resized()
{
    auto area = getLocalBounds();
    
    auto playStopArea = area.removeFromLeft(static_cast<int>(getWidth() / 7.0f));

    playStopButton.setBounds(playStopArea.
        withSizeKeepingCentre(static_cast<int>(playStopArea.getHeight() * 0.4f), 
                              static_cast<int>(playStopArea.getHeight() * 0.4f)));

    auto textEditArea = area.removeFromLeft(getWidth() * 5 / 7.0f);
    textEditor.setBounds(textEditArea.
        withSizeKeepingCentre(textEditArea.getWidth() ,
                              static_cast<int>(textEditArea.getHeight() * 0.6f )));

    addRemoveButton.setBounds(
        area.withSizeKeepingCentre( static_cast<int>(area.getHeight()*0.4f), 
                                    static_cast<int>(area.getHeight()*0.4f)));
}

juce::URL SampleLoopButton::getURL()
{
    return url;
}

void SampleLoopButton::buttonClicked(Button*)
{

}

void SampleLoopButton::resetButtonData()
{
    url = juce::URL{};
    textEditor.setText("",false);
    playStopButton.setButtonEnabled(false);

}

void SampleLoopButton::setSampleDataAndButtonsStatus(juce::URL _url, 
                                                     String _name, 
                                                      bool updateAddRemoveMode)
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