/*
  ==============================================================================

    SampleLoopButton.h
    Created: 25 Aug 2026 11:11:38pm
    Author:  hraha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "OnOffButton.h"

//==============================================================================
/*
*/
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

    /**loads sample data to the button*/
    void setSample(juce::URL _url, String _name);

    /**remove data from the button*/
    void resetButtonData();

    /**adds Listener to the button*/
    void addListener(juce::Button::Listener* listener);

    /**sets a callback for the text editor when its text gets changed*/
    void SampleLoopButton::setTextChangeCallBack(std::function<void(juce::String)> callback);

    /**set addButton enable status*/
    void setAddButtonEnabled(bool status);

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

    OnOffButton playStopButton{ BinaryData::play_png, BinaryData::play_pngSize, BinaryData::pause_png, BinaryData::pause_pngSize };
    OnOffButton addRemoveButton{ BinaryData::add_png, BinaryData::add_pngSize, BinaryData::remove_png, BinaryData::remove_pngSize };

    juce::TextEditor textEditor;

    juce::GroupComponent groupComponent;

    int buttonID;

    bool isPlaying = false;

    bool isLoaded = true;

    juce::URL url;

    /**It is a callback for the text editor when its text gets changed*/
    std::function<void(juce::String)> textChangeCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleLoopButton)
};
