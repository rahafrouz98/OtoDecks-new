
#pragma once

#include <JuceHeader.h>
#include "OnOffButton.h"

class SampleLoopButton  : public juce::Component, public juce::Button::Listener
{
public:
    SampleLoopButton();
    ~SampleLoopButton() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    /** implement Button::Listener */
    void buttonClicked(Button*) override;

    /**set id */
    void setID(int id);

    /**loads sample data and updates the status of buttons. updateAddRemoveMode=true is used 
    when we need to change the mode without onClickcallback. If it is false the mode will be 
    toggled by callback */
    void setSampleDataAndButtonsStatus(juce::URL _url, String _name, bool updateAddRemoveMode = false);

    /**remove data from the button and disable play button*/
    void resetButtonData();

    /**adds Listener to the button*/
    void addListener(juce::Button::Listener* listener);

    /**sets a callback for the text editor when its text gets changed*/
    void SampleLoopButton::setTextChangeCallBack(std::function<void(juce::String)> callback);

    /**set addButton enable status*/
    void setAddRemoveButtonEnabled(bool status);

    /**get playStopStatus */
    bool getPlayStopButtonStatus()const;

    /**get addRemoveStatus */
    bool getAddRemoveButtonStatus()const;

    /**returns a const pointer to the playStop button*/
    const juce::Button* getPlayStopButtonPointer()const;

    /**returns a const pointer to the addremove button*/
    const juce::Button* getAddRemoveButtonPointer()const;

    /**get the url */
    juce::URL getURL();

    /**get buttonID*/
    int getID();
private:

    OnOffButton playStopButton{ BinaryData::play_png, 
                                BinaryData::play_pngSize, 
                                BinaryData::pause_png,
                                BinaryData::pause_pngSize };

    OnOffButton addRemoveButton{ BinaryData::add_png, 
                                 BinaryData::add_pngSize,
                                 BinaryData::remove_png, 
                                 BinaryData::remove_pngSize };

    juce::TextEditor textEditor;

    int buttonID;

    bool isPlaying = false;

    bool isLoaded = true;

    juce::URL url;

    /**It is a callback for the text editor when its text gets changed*/
    std::function<void(juce::String)> textChangeCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleLoopButton)
};
