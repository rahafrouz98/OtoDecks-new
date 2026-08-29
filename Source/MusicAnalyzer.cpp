/*
  ==============================================================================

    musicAnalyzer.cpp
    Created: 13 Aug 2026 10:05:08am
    Author:  hraha

  ==============================================================================
*/

#include <JuceHeader.h>
#include "musicAnalyzer.h"

//==============================================================================
MusicAnalyzer::MusicAnalyzer(): Thread("Music Analyzer Thread")
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    formatManager.registerBasicFormats();

    //////////////////////////////////////////////Progress Bars and labels/////////////////////////////////////
    addAndMakeVisible(specProgLabel);
    specProgLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour(88, 211, 255));
    specProgLabel.setJustificationType(juce::Justification::centredBottom);
    specProgLabel.setText("Spectrogram Analysis:", dontSendNotification);

    addAndMakeVisible(SpectrogramProgressBar);
    SpectrogramProgressBar.setColour(juce::ProgressBar::ColourIds::foregroundColourId, juce::Colour(88, 211, 255));

    addAndMakeVisible(BPMProgLabel);
    BPMProgLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colour(88, 211, 255));
    BPMProgLabel.setJustificationType(juce::Justification::centredBottom);
    BPMProgLabel.setText("BPM Analysis:", dontSendNotification);

    addAndMakeVisible(BPMProgressBar);
    BPMProgressBar.setColour(juce::ProgressBar::ColourIds::foregroundColourId, juce::Colour(88, 211, 255));
}

MusicAnalyzer::~MusicAnalyzer()
{
	stopThread(500);
    stopTimer();
}

void MusicAnalyzer::paint(juce::Graphics& g)
{
    

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

    g.fillAll(juce::Colours::black);

    juce::ScopedLock scopeLock(lock);
    if (sixBandSpectrogram.size() != 0 && !isThreadRunning())
    {

        int lastFrame = static_cast<int>(liveTime / frameDuration);

        int spectrogramSize = static_cast<int> (sixBandSpectrogram.size());
        
        for (int frameIndex = 1; frameIndex < sixBandSpectrogram.size(); ++frameIndex)
        {

            for (const auto& [band, energy] : sixBandSpectrogram[frameIndex])
            {
                juce::uint8 blue = static_cast<juce::uint8>(energy * 100);
                juce::uint8 green = static_cast<juce::uint8>(std::pow(energy, 3.0f));
                juce::uint8 red = static_cast<juce::uint8>(std::pow(energy, 5.0f));
                g.setColour(juce::Colour(red, green, blue));

                int barWidth = static_cast<int>( getWidth() / bandsNumber );

                g.fillRect( barWidth * static_cast<int>(stringToFrequencyBand(band)), 
                            getHeight(),
                            barWidth,
                            static_cast<int>(energy * 100 ));

            }
        }


       
        
    }
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);

     
}

void MusicAnalyzer::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    specProgLabel.setBounds(getWidth() / 2- getWidth() / 4, getHeight() / 3 - getHeight() / 10, getWidth() / 2, getHeight() / 10);
    SpectrogramProgressBar.setBounds(getWidth() / 4, getHeight() / 3, getWidth() / 2, getHeight() / 10);

    BPMProgLabel.setBounds(getWidth() / 2 - getWidth() / 4, getHeight() * 2 / 3- getHeight() / 10, getWidth() / 2, getHeight() / 10);
    BPMProgressBar.setBounds(getWidth() / 4, getHeight() *2 / 3, getWidth() / 2, getHeight() / 10);

}
void MusicAnalyzer::releaseResources()
{
}
void MusicAnalyzer::prepareToPlay(int , double )
{
}
void MusicAnalyzer::getNextAudioBlock(const AudioSourceChannelInfo&)
{
}
void MusicAnalyzer::run()
{
    startTimer(20);
    setSixBandEnergySpectrogram();
    setBooleanBeatSpectrogram();
    setTimeBPMMap();
    signalThreadShouldExit();
    stopTimer();
    //sends the lambda function to the message thread queue to hide the progress bar
    MessageManager::callAsync([this](){
        SpectrogramProgressBar.setVisible(false);
        BPMProgressBar.setVisible(false);
        specProgLabel.setVisible(false);
        BPMProgLabel.setVisible(false);
     });
  
}
void MusicAnalyzer::loadAudioData(File file)
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
void MusicAnalyzer::setSixBandEnergySpectrogram()
{
    //reset the spectrogram vector for new audio
    sixBandSpectrogram.clear();
    frameDuration = 0;

	juce::AudioBuffer<float> audioBuffer(1, fftSize);

    if (reader == nullptr)
    {
        return;
    }

	double sampleRate = reader->sampleRate;

	frameDuration = static_cast<float>(fftSize / sampleRate);

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

        //it makes all frames have a equal number of samples except the last frame that might be less
        if (numberOfAudioSamples - readerStartSample >= fftSize)
        {
			bufferSize = fftSize;
        }
        else
        {
			bufferSize = static_cast<int>(numberOfAudioSamples - readerStartSample);
        }

        //read the next batch of samples and keep it in audioBuffer
        reader->read(&audioBuffer, 0, bufferSize, readerStartSample, true, false);

		//Reset fftResult to zero
        std::fill(fftResult.begin(), fftResult.end(), 0.0f);


		//transfer buffer to fftResult
        auto it = audioBuffer.getReadPointer(0);
        std::copy(it, it+bufferSize, fftResult.begin());
       
        //transform the data in the fftResult into spectrum of frequencies
		fft.performFrequencyOnlyForwardTransform(fftResult.data(), true);

		//convert the fftResault to a six-band energy spectrum for this frame
        std::map< std::string, float>  bandSpectrum = generateSixbandEnergySpectrum(fftResult);

        juce::ScopedLock scopeLock(lock);
        sixBandSpectrogram.push_back( bandSpectrum );
        
        atomicSpectrogramPercentage = readerStartSample / double(numberOfAudioSamples);
       
        readerStartSample += bufferSize;

		//reset the audioBuffer for the next frame
		audioBuffer.clear();

	}

}

std::map< std::string, float> MusicAnalyzer::generateSixbandEnergySpectrum(const std::array<float, fftSize*2>& spectrum)
{
	//The first element of the spectrum is the DC component and to exclude it the binPointer starts from 1 
    int binPointer = 1;
   
    std::map< std::string, float> bandSpectrum;

    float frequrncyResulotion = 1.0f / frameDuration;


    // (fftSize/2)-1 only includes the positivefrequencies excluding Nyquist frequency.
    while (binPointer < fftSize/2-1)
    {
        if (threadShouldExit())
        {
            return bandSpectrum;
        }
        std::string bandCategory = bandCategorizer(binPointer * frequrncyResulotion);   
		
        //in this case the frequncy is either below or above of hearing capability
        if (bandCategory == "")
        {
            binPointer++;
            continue;
        }

        //accumulate the total energy of all bins in each band
        bandSpectrum[bandCategory] += spectrum[binPointer];

        binPointer++;
    }

    return bandSpectrum;
}

std::string MusicAnalyzer::bandCategorizer(float frequency) const
{
    if (frequency >= subBass.min && frequency < subBass.max)
    {
        return "subBass";
    }
    if (frequency >= bass.min && frequency < bass.max)
    {
        return "bass";
    }
    if (frequency >= lowMid.min && frequency < lowMid.max)
    {
        return "lowMid";
    }
    if (frequency >= mid.min && frequency < mid.max)
    {
        return "mid";
    }
    if (frequency >= highMid.min && frequency < highMid.max)
    {
        return "highMid";
    }
    if (frequency >= treble.min && frequency < treble.max)
    {
        return "treble";
    }
    //if frequency is higher or lower is not detectable by human
    return "";
}

void MusicAnalyzer::setLiveTime(float time)
{
    if (!isThreadRunning())
    {
       liveTime = time;
	   repaint();
    }
}

void MusicAnalyzer::setTimeBPMMap()
{
    //reset the beatTime_BPM vector for new audio
    beatTimeBPM.clear();
    if (frameDuration == 0)
    {
        return;
    }
    
    //extract the time-beat map for all band categories and add it to beatTimeBPM
    beatTimeBPM["subBass"] = extractTimeBeatForThisBand("subBass");

    beatTimeBPM["bass"] = extractTimeBeatForThisBand("bass");

    beatTimeBPM["lowMid"] = extractTimeBeatForThisBand("lowMid");

    beatTimeBPM["mid"] = extractTimeBeatForThisBand("mid");

    beatTimeBPM["highMid"] = extractTimeBeatForThisBand("highMid");

    beatTimeBPM["treble"] = extractTimeBeatForThisBand("treble");

}

std::map<int, int>MusicAnalyzer::extractTimeBeatForThisBand(std::string band)
{
    std::map<int, int> bandTimeBeat;
    //run a separate loop for detecting the first beat so the second loop does not have to use a condition for detecting the first beat and
   // increase the performance 
    bool isFirstBeatDetected = false;
    int lastDetectedFrame = 0;

    for (int frameIndex = 0; frameIndex < booleanBeatSpectrogram.size(); frameIndex++)
    {
        if (threadShouldExit())
        {
            return std::map<int, int>{};
        }
        if (booleanBeatSpectrogram[frameIndex][band] == true)
        {
            //ther is no previous beat detected so the fist is 0 BPM
            bandTimeBeat[frameIndex] = 0;
            lastDetectedFrame = frameIndex;
            isFirstBeatDetected = true;
            break;
        }
    }

    if (isFirstBeatDetected)
    {
        //continue for the rest of the beats in the second loop
        for (int frameIndex = lastDetectedFrame + 1; frameIndex < booleanBeatSpectrogram.size(); frameIndex++)
        {
            if (threadShouldExit())
            {
                return std::map<int, int>{};
            }
            if (booleanBeatSpectrogram[frameIndex][band] == true)
            {

                double beatTime = frameIndex * frameDuration;
                int BPM = static_cast<int>(60.0 / (beatTime - lastDetectedFrame * frameDuration));

                //to ensure that multiple beats are not detected for a singgle event (extended in multiple frames)
                // a refactory time period is considered so time between beat detections can not be less than 
                //refactory time which is considered 250BPM
                //also beat detections less than 60BPM (one per second is not very common and rythmic in music
                //the reange of beats to be marked is considered between 60BPM and 350BPM
                if (BPM < 60 || BPM > 250)
                {
                    continue;
                }
                beatTimeBPM[band][frameIndex] = BPM;
                lastDetectedFrame = frameIndex;
            }
            atomicBPMPercentage = frameIndex / double(booleanBeatSpectrogram.size());
        }
    }

}

void MusicAnalyzer::setBooleanBeatSpectrogram()
{
    //number of frames for one second
    int numberOfFramesInWindow = static_cast<int>( 1.0f / frameDuration );

    //it holds the collection of time frams spectrum . std::queue is used to optimize the process
    //of removing the itme from front and add to the back
    std::queue< std::map< std::string, float>> windowSpectrum;

    //add the first batch of spectrum of frames  to windoSpectrum to have a 1-second baseline for beat detection
    for (int i = 0; i < numberOfFramesInWindow && i < sixBandSpectrogram.size(); i++)
    {
        windowSpectrum.push(sixBandSpectrogram[i]);

        //we do not detect beat for the first second of audio as we do not have enough data yet
        booleanBeatSpectrogram.push_back(std::map<std::string, bool>{
            {"subBass", false}, 
            { "bass", false }, 
            { "lowMid", false }, 
            { "mid", false }, 
            { "highMid", false }, 
            { "treble", false }
        });
    }
    
    //compare each next frame spectrum with a wincowSpectrum of 1_second period before the frame and detect beat
    for (int i = numberOfFramesInWindow; i < sixBandSpectrogram.size(); i++)
    {
        if (threadShouldExit())
        {
            return;
        }
        std::map<std::string, bool> detectedBeatOnFrame = detectBeatOnFrame(windowSpectrum, sixBandSpectrogram[i]);
        booleanBeatSpectrogram.push_back(detectedBeatOnFrame);

        //remove the old frame from the window and add a new one for the next calculation

        windowSpectrum.pop();
        windowSpectrum.push(sixBandSpectrogram[i]);
    }
    

}

std::map<std::string, bool> MusicAnalyzer::detectBeatOnFrame( std::queue< std::map< std::string, float>> FIFOWindoSpectrum,
                                                            const std::map< std::string, float>& nextFrameSixBandSpectrum)
{
    std::map<std::string, bool> bandbeatMap;

    //to be able to itterate through items of windowSpectrom i changed queue to a vector
    std::vector< std::map< std::string, float>>windoSpectrum;
    while (!FIFOWindoSpectrum.empty())
    {
        //moves the item to the vectorr without copying it
        windoSpectrum.push_back(std::move(FIFOWindoSpectrum.front()));
        FIFOWindoSpectrum.pop();
    }

    if (windoSpectrum.size() == 0)
    {
        return std::map<std::string, bool>{};
    }

    //This map keeps the sum energy of each band for this frame
    std::map< std::string, float> sumEnergyMap;

    //This map keeps the average energy of each band for all frames of this second
    std::map< std::string, float> averageEnergyMap;

    //This map keeps the square variance sum of each band through all frames of this second
    std::map< std::string, float> sumVarianceEnergyMap;

    //This map keeps the variance of each band through all frames of this second
    std::map< std::string, float> varianceEnergyMap;

    //calculate the sum of energy of all frames for each band 
    for (const auto& frame : windoSpectrum)
    {
        for (const auto& [bandCategory, energy] : frame)
        {
            sumEnergyMap[bandCategory] += energy;
        }
    }

    //calculate the average of energy of each band for the whole span of frames
    for (const auto& [bandCategory, energySum] : sumEnergyMap)
    {
        averageEnergyMap[bandCategory] = energySum / windoSpectrum.size();
    }

    //calculate the square variance sum of each band for the whole span of frames
    for (const auto& frame : windoSpectrum)
    {
        for (const auto& [bandCategory, energy] : frame)
        {
            sumVarianceEnergyMap[bandCategory] += ( (energy - averageEnergyMap[bandCategory]) *
                (energy - averageEnergyMap[bandCategory]));
        }
    }

    //calculate the variance sum  each band for the whole span of frames
    for (const auto& [bandCategory, arianceEnergySum] : sumVarianceEnergyMap)
    {
        varianceEnergyMap[bandCategory] = arianceEnergySum / windoSpectrum.size();
    }


    //check the energy of each band of the nextFrameSixBandSpectrum and if it pass the cireteria but true on the beatMap for that band 
    for (const auto& [bandCategory, energy] : nextFrameSixBandSpectrum)
    {
        //formula from https://gamedev.net/tutorials/programming/math-and-physics/beat-detection-algorithms-r1952

        float C = (-0.0025714 * varianceEnergyMap[bandCategory]) + 1.5142857;

        if (energy > C * averageEnergyMap[bandCategory] )
        {
            bandbeatMap[bandCategory] = true;
        }
        else
        {
            bandbeatMap[bandCategory] = false;
        }
    }

    return bandbeatMap;
}

int MusicAnalyzer::getLiveBPM(FrequencyBand band)
{
    int currentFrame = int(liveTime / frameDuration);
    if (isThreadRunning())
    {
        return 0;
    }

    std::string stringifiedBand = frequencyBandToString(band);

    //if beat at this moment is detected return its BPM
    if (beatTimeBPM[stringifiedBand].count(currentFrame))
    {
        //update the lastReadFrame for next call
        lastReadFrame = currentFrame;
        return beatTimeBPM[stringifiedBand][currentFrame];
    }
    //if not return the last one
    else
    {
		return beatTimeBPM[stringifiedBand][lastReadFrame];
    }

    return 0;
}

void MusicAnalyzer::timerCallback()
{
    spectrogramPercentage = static_cast<double>(atomicSpectrogramPercentage);
    BPMPercentage = static_cast<double>(atomicBPMPercentage);
}

std::string MusicAnalyzer::frequencyBandToString(FrequencyBand band)
{
    switch (band)
    {
        case FrequencyBand::subBass: 
            return "subBass";
        case FrequencyBand::bass:   
            return "bass";
        case FrequencyBand::lowMid:  
            return "lowMid";
        case FrequencyBand::mid:     
            return "mid";
        case FrequencyBand::highMid:
            return "highMid";
        case FrequencyBand::treble: 
            return "treble";
        default:
            return "unknown";
    }
}

MusicAnalyzer::FrequencyBand MusicAnalyzer::stringToFrequencyBand(std::string band)
{
    if (band == "subBass") return  FrequencyBand::subBass;
    if (band == "bass") return  FrequencyBand::bass;
    if (band == "lowMid") return  FrequencyBand::lowMid;
    if (band == "mid") return  FrequencyBand::mid;
    if (band == "highMid") return  FrequencyBand::highMid;
    if (band == "treble") return  FrequencyBand::treble;
    else return FrequencyBand::undefined;
    
}