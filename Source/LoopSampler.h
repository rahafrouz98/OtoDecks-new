/*
  ==============================================================================

    LoopSampler.h
    Created: 23 Aug 2026 7:52:08am
    Author:  hraha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "OnOffButton.h"
#include "DJAudioPlayer.h"
#include "Microphone.h"
#include "WaveformDisplay.h"
#include "SampleLoopButton.h"
#include "Utilities.h"


class LoopSampler : public juce::Component, public::juce::Button::Listener, public::juce::AudioSource, public juce::Timer
{
public:
    LoopSampler(DJAudioPlayer* _player, DJAudioPlayer& _leftPlayer,
        DJAudioPlayer& _rightPlayer, Microphone& _microphone, AudioFormatManager& _formatManagerToUse,
        AudioThumbnailCache& _cacheToUse);
    ~LoopSampler() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void buttonClicked(Button*) override;

    void releaseResources()override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate)override;

    void timerCallback()override;

    /**called repeaditly at the end of the mainComponent::getNextAudioBlock to fetch the blocks of audio samples from DJAudioPlayer AudioSource and
    the Microphon AudioSource */
    void processSignals();

    /**set the number of output channels*/
    void setNumberOfChannels(int numChannels);



private:

    std::array<SampleLoopButton, 8> sampleButtons;
    //first item is representing url and second item is the displayed name.
    std::array<Utilities::SampleStruct, 8> samplesRecord;

    juce::URL sampledURL = juce::URL{};

    OnOffButton leftDeckImageButton{ BinaryData::left_png, BinaryData::left_pngSize,BinaryData::disabledleft_png, BinaryData::disabledleft_pngSize};
    OnOffButton startStopRecordImageButton{ BinaryData::startrecord_png, BinaryData::startrecord_pngSize, BinaryData::stoprecording_png, BinaryData::stoprecording_pngSize};
    OnOffButton deleteImageButton{ BinaryData::delete_png, BinaryData::delete_pngSize };
    OnOffButton playSampleImageButton{ BinaryData::play_png, BinaryData::play_pngSize, BinaryData::pause_png, BinaryData::pause_pngSize };
    OnOffButton rightDeckImageButton{ BinaryData::right_png, BinaryData::right_pngSize, BinaryData::disabledright_png, BinaryData::disabledright_pngSize};

    DJAudioPlayer& leftPlayer;
    DJAudioPlayer& rightPlayer;
    Microphone& microphone;

    DJAudioPlayer* player;

    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatWriter> writer;

    WaveformDisplay waveformDisplay;

    //for wavFormat
    int possibleBitsPerSample = 16;
    //for wavFormat
    double possibleSampleRate = 44100;
    int numberOfChannels = 0;
    int bitsPerSamples = 0;

    juce::AudioBuffer<float> samplerBuffer;


    /** Select a destination to save the loop sample in WAV format  in a directory called samples 
    in the same directory as app gets created*/
    juce::File selectSampleAudioFile();
    

    /** saves the sampel to the local memory*/
    void startRecording();

    /**writes the extracted buffer from processSignals on to the file*/
    void writeOnFile(juce::AudioBuffer<float> buffer, int startSample, int numSamples);

    /**removes sampled file from player, reset sampledURL, and its waveform and updates buttons*/
    void removeSampleFromRecordingSection();

    /**loops through sample buttons and enable the addREmove button for them if they do not have loaded URL*/
    void enableAddRemoveForEmptysampleButtons();

    /**loops through sample buttons and disable the addREmove button for them if they do not have loaded URL*/
    void disableAddRemoveForEmptysampleButtons();

    /**delete the loop sample from local memory*/
    void deleteLocalFile(juce::URL url);

    /**writes the loop samples data as a json file in a directory called samples in the same directory as EXE is located*/
    void writeLoopSamplesData();

    /**loads loop samples data in JSON format from a directory called samples located in the same directory as EXE file is located
    and save it in the samplesRecord array*/
    void loadLoopSamplesData();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoopSampler)
};
