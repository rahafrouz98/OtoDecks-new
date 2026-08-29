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
    /**enum to classify band categories*/
    enum class FrequencyBand{
        subBass,
        bass,
        lowMid,
        mid,
        highMid,
        treble,
        undefined
    };
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
    int getLiveBPM(FrequencyBand band);


private:

    static constexpr int fftOrder = 10;
    static constexpr int fftSize = 1 << fftOrder;
	static constexpr int bandsNumber = 6;

    juce::CriticalSection lock;

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

    /**keep the last read detected frame*/
    int lastReadFrame = 0;

	/**It is an array of a map for each frame. The first item of map is band category and the second is the total energy*/
	std::vector<std::map< std::string, float>> sixBandSpectrogram;


    /**Each item of this vector represent the booloean beat spectrum for each fram. the string is band category and bool is true if 
    beat is detected for at this time frame and for this band category*/
    std::vector< std::map <std::string, bool>> booleanBeatSpectrogram;

    /**the outer map first element is representing band category. The first element of inner map is representing band category and the second is 
    the BPM at that time frame for that band category*/
    std::map< std::string, std::map <int, int>> beatTimeBPM;

    /**Extract spectrogram and frameDuration */
	void setSixBandEnergySpectrogram();

    /**calculates the  booleanBeatSpectrogram vector from the sixBandSpectrogram.*/
    void setBooleanBeatSpectrogram();

    /**calculates the beatTimeBPM from the booleanBeatSpectrogram*/
    void setTimeBPMMap();

    /**Generate 6 band energy spectrum by grouping the energy of 1024 frequenct bins of the input spectrum. It 
    returns a map. the first element of map is band category and the second is the total energy for that band*/
    std::map< std::string, float> generateSixbandEnergySpectrum(const std::array<float, fftSize*2>& spectrum);


    /**calculates the variance of each band energy for the specific span of one second and  indicate if the next upcomming frame
    has beat on any of band categories. windoSpectrum is holding the six-band energy spectrum of last one second and nextFrameSixBandSpectrum is the 
     upcomming spectrum to detect the beat on it*/
    std::map<std::string, bool> detectBeatOnFrame(std::queue< std::map< std::string, float>> FIFOWindoSpectrum,
                                                  const std::map< std::string, float>& nextFrameSixBandSpectrum);

    /**takes a frequrncy and returns a string representing the band category*/
    std::string bandCategorizer(float frequency) const;

    /**Extract the map of time-BPM for specific bass categor. it returns a map. first item is timeFrame index and second is BPM */
    std::map<int, int> extractTimeBeatForThisBand(std::string band);


    /**converts the FrequencyBand Enum to std::string*/
    static std::string frequencyBandToString(FrequencyBand band);

    /**converts the std::string to FrequencyBand Enum*/
    FrequencyBand stringToFrequencyBand(std::string band);

    struct BandFrequncy
    {
        float min = 0;
        float max = 0;
    };

    BandFrequncy subBass{ 20.0f, 60.0f};
    BandFrequncy bass { 60.0f, 250.0f };
    BandFrequncy lowMid { 250.0f, 500.0f };
    BandFrequncy mid { 500.0f, 2000.0f };
    BandFrequncy highMid { 2000.0f, 6000.0f };
    BandFrequncy treble { 6000.0f, 20000.0f };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicAnalyzer)
};
