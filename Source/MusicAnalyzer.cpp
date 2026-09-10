#include "musicAnalyzer.h"

MusicAnalyzer::MusicAnalyzer(): Thread("Music Analyzer Thread")
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    formatManager.registerBasicFormats();

    //////////////////////////////////Progress Bars and labels////////////////////////////
    addAndMakeVisible(specProgLabel);
    specProgLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour(88, 211, 255));
    specProgLabel.setJustificationType(juce::Justification::centredBottom);
    specProgLabel.setText("Spectrogram Analysis:", dontSendNotification);

    addAndMakeVisible(SpectrogramProgressBar);
    SpectrogramProgressBar.setColour(juce::ProgressBar::ColourIds::foregroundColourId,
                                     juce::Colour(88, 211, 255));

    addAndMakeVisible(BPMProgLabel);
    BPMProgLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour(88, 211, 255));
    BPMProgLabel.setJustificationType(juce::Justification::centredBottom);
    BPMProgLabel.setText("BPM Analysis:", dontSendNotification);

    addAndMakeVisible(BPMProgressBar);
    BPMProgressBar.setColour(juce::ProgressBar::ColourIds::foregroundColourId,
                             juce::Colour(88, 211, 255));
}
MusicAnalyzer::~MusicAnalyzer()
{
	stopThread(500);
    stopTimer();
}

void MusicAnalyzer::paint(juce::Graphics& g)
{ 
    g.fillAll(juce::Colours::black);

    auto area = getLocalBounds();

    int liveFrameIndex = 0;
            
    juce::ScopedTryLock  scopeLock(lock);

    if (scopeLock.isLocked())
    {

        if (frameDuration > 0)
        {
            liveFrameIndex = static_cast<int>(liveTime / frameDuration);
        }

        /**because vector.size() is unsigned liveFrameIndex < fourBandSpectrogram.size() - 1
        condition is not enough as it underflows*/
        if (fourBandEnergySpectrogram.size() > 0 &&
            liveFrameIndex < fourBandEnergySpectrogram.size() - 1 &&
            !isThreadRunning())
        {
            int spectrogramSize = fourBandEnergySpectrogram.size();

            auto barGraphArea = area.removeFromTop(getHeight() * 5 / 6);

            for (const auto& [band, energy] : fourBandEnergySpectrogram[liveFrameIndex])
            {
                if (band == "low")
                {
                    g.setColour(juce::Colours::red);
                }
                else if (band == "mid")
                {
                    g.setColour(juce::Colours::green);
                }
                else if (band == "high")
                {
                    g.setColour(juce::Colours::blue);
                }
                else
                {
                    g.setColour(juce::Colours::white);
                }

                int barWidth = getWidth() / bandsNumber;
                int barHeight = static_cast<int>(energy / 5.0f);
                g.fillRect(barGraphArea.removeFromLeft(barWidth).removeFromBottom(barHeight));

                g.setColour(juce::Colours::white);
                g.drawText(band, area.removeFromLeft(barWidth), juce::Justification::centred);
            }
        }
        else
        {
            g.setColour(juce::Colours::black);
            g.fillRect(area);
        }
    }
    g.setColour(juce::Colours::grey);
    g.drawRect(area, 1);  
}

void MusicAnalyzer::resized()
{

    specProgLabel.setBounds( getWidth() / 2 - getWidth() / 4, 
                             getHeight() / 3 - getHeight() / 10,
                             getWidth() / 2, getHeight() / 10);

    SpectrogramProgressBar.setBounds( getWidth() / 4, 
                                      getHeight() / 3, 
                                      getWidth() / 2, 
                                      getHeight() / 10);

    BPMProgLabel.setBounds( getWidth() / 2 - getWidth() / 4,
                            getHeight() * 2 / 3 - getHeight() / 10, 
                            getWidth() / 2,
                            getHeight() / 10);

    BPMProgressBar.setBounds( getWidth() / 4, 
                              getHeight() * 2 / 3, 
                              getWidth() / 2,
                              getHeight() / 10 );
}

void MusicAnalyzer::timerCallback()
{
    spectrogramPercentage = static_cast<double>(atomicSpectrogramPercentage);
    BPMPercentage = static_cast<double>(atomicBPMPercentage);
}

void MusicAnalyzer::analyzeAudio(File file)
{
    stopThread(500);
    atomicBPMPercentage = 0;
    atomicSpectrogramPercentage = 0;
    reader.reset(formatManager.createReaderFor(file));
    SpectrogramProgressBar.setVisible(true);
    BPMProgressBar.setVisible(true);
    specProgLabel.setVisible(true);
    BPMProgLabel.setVisible(true);
    startThread();
}

void MusicAnalyzer::run()
{
    startTimer(20);
    setFourBandEnergySpectrogram();
    setBPMVector();
    stopTimer();
    
    //sends the lambda function to the message thread queue to hide the progress bar
    //the condition prevents sending the message if in the middle of analysing another analyzeAudio()
    //is called. Otherwise, the progress bars are hidden for the second analysis
    if (atomicSpectrogramPercentage >= 0.99 && atomicBPMPercentage >= 0.99)
    {
        MessageManager::callAsync([this](){
            SpectrogramProgressBar.setVisible(false);
            BPMProgressBar.setVisible(false);
            specProgLabel.setVisible(false);
            BPMProgLabel.setVisible(false);
         });
    }
}

void MusicAnalyzer::setFourBandEnergySpectrogram()
{
    //this lock solves the issue of competition between paint() and FourBandSpectrogram.clear()
    {
        juce::ScopedLock scopeLock(lock);
        fourBandEnergySpectrogram.clear();
    }


	juce::AudioBuffer<float> audioBuffer(1, firstFFTSize);

    if (reader == nullptr)
    {
        return;
    }

	double sampleRate = reader->sampleRate;

	frameDuration = static_cast<float>(firstFFTSize / sampleRate);

    int64 readerStartSample = 0;
    int bufferSize;
    int64 numberOfAudioSamples = reader->lengthInSamples;

	while (readerStartSample < numberOfAudioSamples)
	{
        if (threadShouldExit())
        {
            audioBuffer.clear();
            return;
        }

        /**it makes all frames have a equal number of samples except the last frame 
        that might be less*/
        if (numberOfAudioSamples - readerStartSample >= firstFFTSize)
        {
			bufferSize = firstFFTSize;
        }
        else
        {
			bufferSize = static_cast<int>(numberOfAudioSamples - readerStartSample);
        }

        //read the next batch of samples and keep it in audioBuffer
        reader->read(&audioBuffer, 0, bufferSize, readerStartSample, true, false);

        std::array<float, firstFFTSize * 2> fftResult{ 0.0f };
        
        //transfer buffer to fftResult
        auto it = audioBuffer.getReadPointer(0);
        std::copy(it, it+bufferSize, fftResult.begin());
       
        //transform the data in the fftResult into spectrum of frequencies
		firstFFT.performFrequencyOnlyForwardTransform(fftResult.data(), true);

		//convert the fftResault to a four-band energy spectrum for this frame
        std::map< std::string, float>  bandSpectrum = generateFourbandEnergySpectrum(fftResult);

        fourBandEnergySpectrogram.push_back( bandSpectrum );
        
        atomicSpectrogramPercentage = readerStartSample / double(numberOfAudioSamples);
       
        readerStartSample += bufferSize;

		//reset the audioBuffer for the next frame
		audioBuffer.clear();
	}
}

std::map< std::string, float> MusicAnalyzer::generateFourbandEnergySpectrum(
                                                const std::array<float, firstFFTSize*2>& spectrum)
{
	/**The first element of the spectrum is the DC component and to exclude it the binPointer
    starts from 1*/
    int binPointer = 1;
   
    std::map< std::string, float> bandSpectrum;

    float frequrncyResulotion = 1.0f / frameDuration;


    // (firstFFTSize/2)-1 only includes the positive frequencies excluding Nyquist frequency.
    while (binPointer < firstFFTSize /2-1)
    {
        if (threadShouldExit())
        {
            return bandSpectrum;
        }
        

        float frequency = binPointer * frequrncyResulotion;

        std::string bandCategory = bandCategorizer(frequency);

		
        //in this case the frequncy is either below or above of hearing capability
        if (bandCategory == "")
        {
            binPointer++;
            continue;
        }

        //accumulate the total energy of categorized bins in each band
        bandSpectrum[bandCategory] += spectrum[binPointer];

        //accumulate the total energy of all bins in human earing range
        if (frequency >= all.min && frequency < all.max)
        {
            bandSpectrum["all"] += spectrum[binPointer];
        }

        binPointer++;
    }

    return bandSpectrum;
}

std::string MusicAnalyzer::bandCategorizer(float frequency) const
{
    if (frequency >= low.min && frequency < low.max)
    {
        return "low";
    }
    if (frequency >= mid.min && frequency < mid.max)
    {
        return "mid";
    }
    if (frequency >= high.min && frequency < high.max)
    {
        return "high";
    }
    if (frequency >= all.min && frequency < all.max)
    {
        return "all";
    }
    //if frequency is higher or lower is not detectable by human
    return "";
}

double MusicAnalyzer::tempoSpectrum(std::array<float, secondFFTSize * 2 > fftResult)
{
    secondFFT.performFrequencyOnlyForwardTransform(fftResult.data(), true);

    int binPointer = 1;

    double frequencyResolution = (1.0 / frameDuration) / (secondFFTSize);

    int minBinIndex = static_cast<int>(1 / frequencyResolution);
    int maxBinIndex = static_cast<int>(4 / frequencyResolution);

    double tempoEnergy = 0;
    double BPM = 0;

    // (secondFFTSize/2)-1 only includes the positive frequencies excluding Nyquist frequency.
    for (int bin = minBinIndex; bin <= maxBinIndex && bin < (secondFFTSize / 2); bin++)
    {
        if (threadShouldExit())
        {
            return 0;
        }

        if (fftResult[bin] > tempoEnergy)
        {
            BPM = bin * frequencyResolution * 60;
            tempoEnergy = fftResult[bin];
        }
    }

    return BPM;

}

void MusicAnalyzer::setBPMVector()
{
    {
        juce::ScopedLock scopeLock(lock);
        BPMVector.clear();
    }

    //takes 512 (secondFFTSize/2) frames of first spectrogram as samples for the second spectrugram
    windowDuration = static_cast<float>(secondFFTSize * frameDuration);

    int frameIndex = 0;
    int totalFrames = fourBandEnergySpectrogram.size();
    int bufferSize = 0;

    while (frameIndex < totalFrames)
    {
        if (threadShouldExit())
        {
            return;
        }

        /**it makes all windows to have a equal number of frames except the last frame
        that might be less*/
        if (totalFrames - frameIndex >= secondFFTSize)
        {
            bufferSize = secondFFTSize;
        }
        else
        {
            bufferSize = static_cast<int>(totalFrames - frameIndex);
        }

        std::array<float, secondFFTSize * 2> fftResult{ 0.0f };
        for (int i = 0; i < bufferSize; i++)
        {
            fftResult[i] = fourBandEnergySpectrogram[i + frameIndex]["all"];
        }

        BPMVector.push_back(tempoSpectrum(fftResult));

        frameIndex += bufferSize;

        atomicBPMPercentage = frameIndex / static_cast<double>(totalFrames);
        DBG(atomicBPMPercentage);
    }
}


void MusicAnalyzer::setLiveTime(float time)
{
    if (!isThreadRunning())
    {
       liveTime = time;
	   repaint();
    }
}

int MusicAnalyzer::getLiveBPM()
{
    if (isThreadRunning() || frameDuration == 0 )
    {
        return 0;
    }
    int currentFrame = int(liveTime / frameDuration);
  
    juce::ScopedLock scopeLock(lock);

    int currentWindowIndex = liveTime / windowDuration;
      
    if (currentWindowIndex < BPMVector.size())
    {
        return BPMVector[currentWindowIndex];
    }
    return 0;
}