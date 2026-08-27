/*
  ==============================================================================

    Microphone.cpp
    Created: 23 Aug 2026 8:17:22pm
    Author:  hraha

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Microphone.h"

//==============================================================================
Microphone::Microphone(juce::AudioDeviceManager& _deviceManager): deviceManager(_deviceManager)
{
    ////////////////////////////////////////////// Microphone /////////////////////////////////////////////////
    addAndMakeVisible(micButton);
    micButton.setFirstMode(false);

    ////////////////////////////////////////////// Knob ///////////////////////////////////////////////////////
    addAndMakeVisible(knob);
    knob.addListener(this);
    knob.setKnobValue(0.25f);
    
}

Microphone::~Microphone()
{
}

void Microphone::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

}

void Microphone::resized()
{
    auto area = getLocalBounds();

    auto knobArea = area.removeFromRight(static_cast<int>(area.getWidth() / 2.0f));
                        
    knob.setBounds( knobArea.withSizeKeepingCentre(static_cast<int>(getWidth()  / 3.0f) ,
                                                   static_cast<int>(getHeight() / 2.0f )));
    
    auto micArea = area.withTrimmedLeft(static_cast<int>(area.getWidth() / 4.0f));
    micButton.setBounds(micArea.withSizeKeepingCentre(static_cast<int>(area.getWidth() / 2.0f),
                                                   static_cast<int>(area.getHeight()/ 2.0f)));
    (knob.getHeight());
    (knob.getWidth());
    (getHeight());
    (getWidth());
}


void Microphone::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{

}

void Microphone::releaseResources()
{
}

//code is inspired from https://juce.com/tutorials/tutorial_processing_audio_input/ 

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

    processedBuffer.setSize(bufferToFill.buffer->getNumChannels(), bufferToFill.buffer->getNumSamples(), false, true, true);

    for (int channel = 0; channel < maxOutputChannels; ++channel)
    {
        if ((!activeOutputChannels[channel]) || maxInputChannels == 0)
        {
            bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
        }
        else
        {
            int mappedInputChannel = channel % maxInputChannels;

            if (activeInputChannels[channel])
            {
                const float* inBuffer = capturedBuffer.getReadPointer(mappedInputChannel, bufferToFill.startSample);
                 float* outBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

                for (int sample = 0; sample < bufferToFill.numSamples;++sample)
                {
                    float noise = (random.nextFloat() * 2.0f) - 1.0f;
                    outBuffer[sample] = inBuffer[sample] * level;
                }
            }
            processedBuffer.copyFrom(channel, 0, *bufferToFill.buffer, channel, 
                                                                 bufferToFill.startSample,bufferToFill.numSamples);
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
