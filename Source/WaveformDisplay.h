#pragma once

#include <JuceHeader.h>

class WaveformDisplay  : public juce::Component, public ChangeListener
{
public:
    WaveformDisplay(AudioFormatManager& formatManagerToUse, AudioThumbnailCache& cacheToUse);
    ~WaveformDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void changeListenerCallback(ChangeBroadcaster* source) override;

    /**It is used for updating the playhead position*/
    void mouseDown(const MouseEvent& event) override;

    /**It is used to update the mouse position*/
    void mouseMove(const MouseEvent& event);

    /**set the play head position*/
    void setPlayHeadPosition(float relativePos);

    /**set the mouse callback function */
    void setMouseClickCallback(std::function<void(float)> callback);


    /**set is recording*/
    void setIsRecording(bool status);

    /**loads the file */
    void loadURL(URL audioURL);

    /**unloads the file*/
    void unloadURL();

private:
    AudioThumbnail audioThumb;
    bool fileLoaded = false;
    float playHeadPosition=0.0f;
    float mouseX = 0.0f;
    float mouseY = 0.0f;
    
    String getMouseX2TimeInString() const;

    bool isRecording = false;

    /**returns the relative position of the playhead*/
    float getPositionRelative();

    /**this call back is used to update the postion of playhead in audioTransport*/
    std::function<void(float)> mouseClickCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
