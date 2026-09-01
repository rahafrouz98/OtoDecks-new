/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 17 Jul 2026 7:44:36am
    Author:  hraha

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class DJAudioPlayer: public AudioSource {
    public:
        DJAudioPlayer(AudioFormatManager& formatManager);
        ~DJAudioPlayer();
        //=========================================================================

        /**implement the AudioSource*/
        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        /**Implement the AudioSource*/
        void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
        /**Implement the AudioSource*/
        void releaseResources() override;

        /**loads the audio track and returns true if the audio file is loaded successfully*/
        bool loadURL(URL audioURL);
        /**set gain of the AudioTransportSource*/
        void setGain(double gain);
        /**set speed of ResamplingAudioSource*/
        void setSpeed(double ratio);
        /**set position of AudioTransportSource*/
        void setPosition(double posInsecs);
        /**Start playing*/
        void start();
        /**Stop playing*/
        void stop();
        /** get the relative position of the play head*/
        float getPostionRelative() const;
        /**calcultaes the length of the AudioTransportSource and return it*/
        double calculateAudioLength() const;

        /**checks if transport source is playing*/
        bool isPlaying() const;

        /**get the current position of the play head in seconds*/
        double getCurrentPosition() const;

        /**returns the AudioFormatReader of the readerSource*/
        AudioFormatReader* getAudioFormatReader() const;

        /**adjust the postion of the playback */
        void setPostion(double targetTime);

        /**toggle the loopoingStatus*/
        void toggleLooping();
        
        /**sets the loopoingStatus*/
        void setLoopingStatus(bool status);

        /**the last buffer extracted from the resampleSource*/
        juce::AudioBuffer<float> latestBuffer;

        /**returns the latest buffer extracted from resampler*/
        const juce::AudioBuffer<float>& getLatestBuffer()const;

        /**unloads the file*/
        void DJAudioPlayer::unloadFile();
    private:
        bool isLooping;
        AudioFormatManager& formatManager;
        std::unique_ptr<AudioFormatReaderSource> readerSource;
        AudioTransportSource transportSource;
        ResamplingAudioSource resampleSource{ &transportSource, false, 2 };
};