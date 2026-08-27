/*
  ==============================================================================

    musicAnalyzer.h
    Created: 13 Aug 2026 10:05:08am
    Author:  hraha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <cmath>

//==============================================================================
/*
*/
class MusicAnalyzer  : public juce::AudioAppComponent, public juce::Thread, public Timer
{
public:
    MusicAnalyzer();
    ~MusicAnalyzer() override;
    void paint (juce::Graphics&) override;
    void resized() override;

    /**loads audio data and launches analyzers*/
    void loadAudioData(File file);

    void releaseResources() override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void run()override;
    void timerCallback() override;
    /**update time and repaint the specrum graph*/
    void setLiveTime(float time);

    /**get the BPM of the corresponding time*/
    int getLiveBPM( );

private:

    static constexpr int fftOrder = 10;
    static constexpr int fftSize = 1 << fftOrder;
	static constexpr int bandsNumber = 32;

    std::array<float, fftSize*2> fftResult{ 0.0f };

    dsp::FFT fft{fftOrder};

    //a separate copy of source reader fro the file is created so two threads (one here and one in DJAudio player)can work at the same time 
    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReader> reader;
   

    float frameDuration = 0;
    float liveTime=0.0f;
    
    //atomic double used to share the variables between two thread
    std::atomic <double> atomicSpectrogramPercentage = 0.0;
    std::atomic <double> atomicBPMPercentage = 0.0;

    double spectrogramPercentage = 0;
    double BPMPercentage = 0;

    ProgressBar SpectrogramProgressBar{ spectrogramPercentage };
    ProgressBar BPMProgressBar{ BPMPercentage };
    Label specProgLabel;
    Label BPMProgLabel;

    /**Extract spectrogram and frame duration */
	void analyzer();

    /**Generate 32 band energy spectrum by grouping the average energy of 1024 frequencies of the input spectrum */
    std::array<float, bandsNumber>generateEnergy32_bandSpectrum(const std::array<float, fftSize*2>& spectrum);

	//the items in the vector is the 32-bandspectrum of one frame, the key is the frame number
	std::vector<std::array<float, bandsNumber>> energy32_bandSpectrogram;

    /**calculates the BPM sets it*/
    void setTimeBPMVector();

    //returns a vector of boolean. Each item representing a timeframe as true(beat detected) and false. 
    std::vector<bool>extractBooleanBeetVector();

    //this vector has pairs, and each pair keep the timeframe index of each beat at the first element and the corresponding BPM at the second element
    std::map<int, int> beatTimeBPM;

    //keep the last read detected frame
    int lastReadFrame = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicAnalyzer)
};
