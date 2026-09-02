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

    static constexpr int fftOrder = 10;
    static constexpr int fftSize = 1 << fftOrder;
	static constexpr int bandsNumber = 4;

    juce::CriticalSection lock;

    std::array<float, fftSize*2> fftResult{ 0.0f };

    dsp::FFT fft{fftOrder};

    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReader> reader;
   
    float frameDuration = 0;
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

	/**It is an array of a map for each frame. The first item of map is band category
    and the second is the total energy*/
	std::vector<std::map< std::string, float>> fourBandSpectrogram;

    /**The first element is representing timeframe and second one is the BPM*/
    std::map <int, int> timeBPM;

    /**Extract spectrogram and frameDuration */
	void setFourBandEnergySpectrogram();


    /**calculates booleanBeatSpectrogram vector from the fourBandSpectrogram.*/
    std::vector<bool> extractBooleanBeatSpectrogram();

    /**calculates timeBPM from the booleanBeatSpectrogram*/
    void setTimeBPM();

    /**Generate 6 band energy spectrum by grouping the energy of 1024 frequenct bins 
    of the input spectrum. It  returns a map. the first element of map is band category 
    and the second is the total energy for that band*/
    std::map< std::string, float> generateFourbandEnergySpectrum( const std::array<float, 
                                                                  fftSize*2>& spectrum);

    /**returns true if beat is detected for the next frame. It takes an std::queue as the
    first argument representing the spectrums of the frames of the last 1 second. The second
    argument is the energy og next fram*/
    bool detectBeatOnFrame(std::queue< float> FIFOWindoAllSpectrum,float nextFrameEnergy);

    /**takes a frequrncy and returns a string representing the band sub category*/
    std::string bandCategorizer(float frequency) const;

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
