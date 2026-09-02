#include <JuceHeader.h>
#include "Microphone.h"

Microphone::Microphone(juce::AudioDeviceManager& _deviceManager): deviceManager(_deviceManager)
{
    ////////////////////////////////////////////// Microphone /////////////////////////////////
    addAndMakeVisible(micButton);
    micButton.setFirstMode(false);

    ////////////////////////////////////////////// slider /////////////////////////////////////
    addAndMakeVisible(slider);
    slider.addListener(this);
    slider.setRange(0.0, 1.0);
    slider.setValue(0.25f);
    slider.setNumDecimalPlacesToDisplay(4);
    slider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::black);
}

Microphone::~Microphone()
{
}

void Microphone::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));  
}

void Microphone::resized()
{
    auto area = getLocalBounds().withTrimmedLeft(static_cast<int>(getWidth() * 0.1f)).
                                 withTrimmedRight(static_cast<int>(getWidth() * 0.1f));

    auto sliderArea = area.removeFromLeft(static_cast<int>(area.getWidth() * 0.7f));
                        
    slider.setBounds(sliderArea.withSizeKeepingCentre(static_cast<int>(sliderArea.getWidth()) ,
                                                      static_cast<int>(sliderArea.getHeight() * 0.5f )));
    
    
    micButton.setBounds(area.withSizeKeepingCentre(static_cast<int>(area.getWidth()* 0.3f),
                                                   static_cast<int>(area.getHeight() * 0.3f)));
 
}


void Microphone::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{

}

void Microphone::releaseResources()
{
}

void Microphone::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!micButton.getStatus())
    {
        return;
    }
    juce::AudioIODevice* device = deviceManager.getCurrentAudioDevice();
    juce::BigInteger activeInputChannels = device->getActiveInputChannels();
    juce::BigInteger activeOutputChannels = device->getActiveOutputChannels();
    int maxInputChannels = activeInputChannels.getHighestBit();
    int maxOutputChannels = activeOutputChannels.getHighestBit();

    processedBuffer.setSize( bufferToFill.buffer->getNumChannels(), 
                             bufferToFill.buffer->getNumSamples(),
                             false, 
                             true, 
                             true);

    for (int channel = 0; channel < maxOutputChannels; ++channel)
    {
        if ((!activeOutputChannels[channel]) || maxInputChannels == 0)
        {
            bufferToFill.buffer->clear(channel,
                                       bufferToFill.startSample,
                                       bufferToFill.numSamples);
        }
        else
        {
            int mappedInputChannel = channel % maxInputChannels;

            if (activeInputChannels[mappedInputChannel])
            {
                const float* inBuffer = capturedBuffer.getReadPointer(mappedInputChannel, 
                                                                      bufferToFill.startSample);

                 float* outBuffer = bufferToFill.buffer->getWritePointer(channel,
                                                                         bufferToFill.startSample);

                for (int sample = 0; sample < bufferToFill.numSamples;++sample)
                {
                    outBuffer[sample] = inBuffer[sample] * level;
                }
            }
            processedBuffer.copyFrom(channel, 
                                     0, 
                                     *bufferToFill.buffer,
                                     channel, 
                                     bufferToFill.startSample,
                                     bufferToFill.numSamples);
        }
    }  
}

void Microphone::sliderValueChanged(Slider* slider)
{
    level = slider->getValue();
}

void Microphone::setInputBuffer(const AudioSourceChannelInfo& bufferToFill)
{
    capturedBuffer = *(bufferToFill.buffer);

}

bool Microphone::getStatus() const
{
    return micButton.getStatus();
}

const juce::AudioBuffer<float>& Microphone::getProceccedBuffer()const
{
    return processedBuffer;
}
