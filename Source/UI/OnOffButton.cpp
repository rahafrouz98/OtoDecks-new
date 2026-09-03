#include "OnOffButton.h"

OnOffButton::OnOffButton(const void* onImageData, 
                         int onImageDataSize, 
                         const void* offImageData, 
                         int offImageDataSize): status(true)
{
    setSize(300, 300);

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

            status = !status;
            updateImages();
        };
}

OnOffButton::~OnOffButton()
{
}

void OnOffButton::paint (juce::Graphics&)
{ 
}

void OnOffButton::resized()
{
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
    /**this condition is for the case that instance is using just one image
    for the first mode*/
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