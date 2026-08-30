/*
  ==============================================================================

    Microphone.h
    Created: 23 Aug 2026 8:17:22pm
    Author:  hraha

  ==============================================================================
*/

#pragma once


#include "../JuceLibraryCode/JuceHeader.h"
#include "KnobButton.h"
#include "OnOffButton.h"
#include <random>


class Microphone: public juce::Component, public AudioSource, public juce::Slider::Listener
{
public:
    Microphone( juce::AudioDeviceManager& _deviceManager);
    ~Microphone() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    /**a callback for knob value change*/
    void sliderValueChanged( juce::Slider* slider )override;
    /**returns the status of microphone to indicate if it is active of not*/
    bool getStatus()const;

    /**is called by the AudioAppComponent::getNextAudioBlock and captures the original buffer before being cleared by audio mixer*/
    void setInputBuffer(const juce::AudioSourceChannelInfo& bufferToFill);

    /**returns a const reference to the processedBuffer. It must be called after getNextAudioBlock is called so updated buffer is given*/
    const juce::AudioBuffer<float>& getProceccedBuffer() const;

private:

    OnOffButton micButton{ BinaryData::mic_png, BinaryData::mic_pngSize, BinaryData::disabledmic_png, BinaryData::disabledmic_pngSize };

    juce::Slider slider{"MIC"};

    float level = 0.25f;

    juce::Random random;

    bool enabled = false;

    juce::AudioDeviceManager& deviceManager;

    //it is mix of inout and output channels of buffer before processing
    juce::AudioBuffer<float> capturedBuffer;

    //it is extracted buffer after processing the input 
    juce::AudioBuffer<float> processedBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Microphone)
};
