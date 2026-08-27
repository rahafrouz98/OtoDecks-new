/*
  ==============================================================================

    OnOffButton.cpp
    Created: 20 Aug 2026 9:15:06am
    Author:  hraha

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OnOffButton.h"

//==============================================================================
OnOffButton::OnOffButton(const void* onImageData, int onImageDataSize, const void* offImageData, int offImageDataSize): status(true)
{
    setSize(300, 300);
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    onImage = ImageCache::getFromMemory(onImageData, onImageDataSize);
    if (offImageData != nullptr)
    {
        offImage = ImageCache::getFromMemory(offImageData, offImageDataSize);
    }

    button.setImages(false, true, true, 
        onImage, 0.8f, juce::Colours::transparentBlack,
        onImage, 0.9f, juce::Colours::transparentBlack,
        onImage, 1.0f, juce::Colours::transparentBlack);

    addAndMakeVisible(button);

    button.onClick = [this]() {
            //toggle the status
            status = !status;
            updateImages();
        };
        
}

OnOffButton::~OnOffButton()
{
}

void OnOffButton::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
}

void OnOffButton::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    button.setBounds(getLocalBounds());
}

void OnOffButton::addListener(juce::Button::Listener* listener)
{
    button.addListener(listener);
}

void OnOffButton::setButtonEnabled(bool enabled)
{
    button.setEnabled(enabled);
}

const juce::Button* OnOffButton::getButtonPointer()const
{
    return &button;
}

bool OnOffButton::getStatus() const
{
    return status;
}

void OnOffButton::setFirstMode(bool statusTarget)
{
    status = statusTarget;
    updateImages();
}

void OnOffButton::updateImages()
{
    if (!offImage.isValid())
    {
        return;
    }
    if (status)
    {
        //load on image
        button.setImages(false, true, true,
            onImage, 0.8f, juce::Colours::transparentBlack,
            onImage, 0.9f, juce::Colours::transparentBlack,
            onImage, 1.0f, juce::Colours::transparentBlack);
    }
    else
    {
        //load off image
        button.setImages(false, true, true,
            offImage, 0.8f, juce::Colours::transparentBlack,
            offImage, 0.9f, juce::Colours::transparentBlack,
            offImage, 1.0f, juce::Colours::transparentBlack);
    }
}