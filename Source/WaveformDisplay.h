/*
  ==============================================================================

    WaveformDisplay.h
    Created: 20 Jul 2026 7:56:25am
    Author:  hraha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class WaveformDisplay  : public juce::Component, public ChangeListener
{
public:
    WaveformDisplay(AudioFormatManager& formatManagerToUse, AudioThumbnailCache& cacheToUse);
    ~WaveformDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void loadURL(URL audioURL);
    void unloadURL();

    void changeListenerCallback(ChangeBroadcaster* source) override;

    /**It is used for updating the playhead position*/
    void mouseDown(const MouseEvent& event) override;
    /**It is used to update the mouse position*/
    void mouseMove(const MouseEvent& event);

    /**set the play head position*/
    void setPlayHeadPosition(float relativePos);

    /**set the mouse callback function */
    void setMouseClickCallback(std::function<void()> callback);

    /**float*/
    float getPositionRelative();

    /**set is recording*/
    void setIsRecording(bool status);
private:
    AudioThumbnail audioThumb;
    bool fileLoaded;
    float playHeadPosition;
    float mouseX;
    float mouseY;
    
    String getMouseX2TimeInString() const;

    bool isRecording = false;

    /**this call back is used to update the postion of playhead in audioTransport*/
    std::function<void()> mouseClickCallback;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
