#pragma once

#include <JuceHeader.h>

class MusicAnalyzer  : public juce::Component, public juce::Thread, public Timer
{
public:
    MusicAnalyzer();
    ~MusicAnalyzer() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    /**loads audio data and launches analyzer thread*/
    void analyzeAudio(File file);
    
    /**starts the worker analyser Thread*/
    void run()override;

    /**calls periodicly when worker thread is running to transfer the 
    progress percentage of progress bars from atomic variables to double variables */
    void timerCallback() override;

    /**update time and repaint the specrum graph*/
    void setLiveTime(float time);

    /**get the BPM of the corresponding time*/
    int getLiveBPM();


private:

    static constexpr int firstFFTOrder = 10;
    static constexpr int firstFFTSize = 1 << firstFFTOrder;
    static constexpr int secondFFTOrder = 9;
    static constexpr int secondFFTSize = 1 << secondFFTOrder;
	static constexpr int bandsNumber = 4;

    juce::CriticalSection lock;

    /**it is used to extract the frequency spectrum of audio samples*/
    dsp::FFT firstFFT{ firstFFTOrder };

    /**It is used to extract the energy-frequency spectrum from a time slice(window) of specrogram*/
    dsp::FFT secondFFT{ secondFFTOrder };

    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReader> reader;
   
    /**frameDuration is equal to firstFFTSize / sampleRate*/
    float frameDuration = 0;

    /**windowDuration is equal to frameDuration * secondFFTSize */
    float windowDuration = 0;

    float liveTime=0.0f;
    
    //atomic double used to share the variables between two thread 
    // (MusicAnalyser thread and GUI thread) without data race
    std::atomic <double> atomicSpectrogramPercentage = 0.0;
    std::atomic <double> atomicBPMPercentage = 0.0;

    //These variabels are updated at timer callback to be match with atomic
    // double variables and used as arguments to create
    double spectrogramPercentage = 0;
    double BPMPercentage = 0;

    ProgressBar SpectrogramProgressBar{ spectrogramPercentage };
    ProgressBar BPMProgressBar{ BPMPercentage };
    
    Label specProgLabel;
    Label BPMProgLabel;

	/**An array of a map for each frame. The first item of map is band category
    and the second is the total energy*/
	std::vector<std::map< std::string, float>> fourBandEnergySpectrogram;

    /**Each item represents the BPM of the corresponding window index*/
    std::vector<double> BPMVector;

    /**Extracts spectrogram and frameDuration */
	void setFourBandEnergySpectrogram();

    /**Itterates through the energy spectrogram, passes each window of frames to tempoSpectrum()
    to extract the BPM for that window, and stores the result in the BPMVector*/
    void setBPMVector();

    /**Generate 4 band energy spectrum by grouping the energy of 1024 (firstFFTSize) frequenct bins 
    of the input spectrum. It  returns a map. the first element of map is band category 
    and the second is the total energy for that band*/
    std::map< std::string, float> generateFourbandEnergySpectrum(const std::array<float, firstFFTSize * 2>& spectrum);

    /**takes a frequrncy and returns a string representing the band sub category*/
    std::string bandCategorizer(float frequency) const;

    /**Takes an array of energy of sequencial frames, applies fft to generate a spectrum for that window, finds a 
    frequency bin with the heigest energy between 1hz to 4 hz (60 BPM to 240BPM),
    and returns the corresponding BPM of that frequency bin. */
    double tempoSpectrum(std::array<float, secondFFTSize * 2 > fftResult);


    struct BandFrequncy
    {
        float min = 0;
        float max = 0;
    };

    BandFrequncy low{ 20.0f, 250.0f};
    BandFrequncy mid { 250.0f, 8000.0f };
    BandFrequncy high { 8000.0f, 20000. };
    BandFrequncy all{ 20.0f, 20000.0f };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicAnalyzer)
};
