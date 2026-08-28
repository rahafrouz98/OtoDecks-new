/*
  ==============================================================================

    PlayStopButton.h
    Created: 20 Aug 2026 9:15:06am
    Author:  hraha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class OnOffButton : public juce::Component
{
public:
    /**it is an on and off image button
    1-onImageData and offImageData are BinaryData::filename_PNG
    2-onImageDataSize and offImageDataSize are BinaryData::filename_PNGSize
    with default value on third and forth argument the button will just have one image
    */
    OnOffButton(const void* onImageData, int onImageDataSize, const void* offImageData = nullptr, int offImageDataSize = 0);
    ~OnOffButton() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    /**adds Listener to the button*/
    void addListener(juce::Button::Listener* listener);

    /**sets the enable status of the button*/
    void setButtonEnabled(bool enabled);

    /**returns a const pointer to the button*/
    const juce::Button* getButtonPointer()const;

    /**returns true if it is on first mode */
    bool getStatus() const;

    /**true sets the button on first mode and update picture*/
    void setFirstMode(bool status);
private:

    juce::ImageButton button;
    juce::Image onImage;
    juce::Image offImage;

    /**true displays on image  and false displays off image*/
    bool status;
    /**this is used in onClick Call back to check if the loaded image and status are match*/
    bool isOnImageLoaded;

    /**update images of the button based on the status value and updates isOnImageLoaded respectively*/
    void updateImages( );



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OnOffButton)
};
