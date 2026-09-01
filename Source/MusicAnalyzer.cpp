
#include <JuceHeader.h>
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

    int liveFrameIndex = 0;
            
    juce::ScopedLock scopeLock(lock);
    if (frameDuration > 0)
    {
        liveFrameIndex = static_cast<int>(liveTime / frameDuration);
    }

    /**because vector.size() is unsigned liveFrameIndex < fourBandSpectrogram.size() - 1  
    condition is not enough as it underflows*/
    if ( fourBandSpectrogram.size() > 0 && 
         liveFrameIndex < fourBandSpectrogram.size() - 1  &&
         !isThreadRunning())
    {
        int spectrogramSize = static_cast<int>(fourBandSpectrogram.size());
        
        auto area = getLocalBounds();
        auto barGraphArea = area.removeFromTop(getHeight() * 5 / 6);

        for (const auto& [band, energy] : fourBandSpectrogram[liveFrameIndex])
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

            int barWidth = static_cast<int>( getWidth() / bandsNumber );
            int barHeight = static_cast<int>(energy / 2.0f);
            g.fillRect(barGraphArea.removeFromLeft(barWidth).removeFromBottom(barHeight));
            
            g.setColour(juce::Colours::white);
            g.drawText(band, area.removeFromLeft(barWidth), juce::Justification::centred);
        }     
    }
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);  
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
    setTimeBPM();
    stopTimer();
    //sends the lambda function to the message thread queue to hide the progress bar
    MessageManager::callAsync([this](){
        SpectrogramProgressBar.setVisible(false);
        BPMProgressBar.setVisible(false);
        specProgLabel.setVisible(false);
        BPMProgLabel.setVisible(false);
     });
  
}
void MusicAnalyzer::setFourBandEnergySpectrogram()
{
    //this lock solves the issue of competition between paint() and FourBandSpectrogram.clear()
    {
        juce::ScopedLock scopeLock(lock);
         fourBandSpectrogram.clear();
    }

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

        /**it makes all frames have a equal number of samples except the last frame 
        that might be less*/
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

		//convert the fftResault to a four-band energy spectrum for this frame
        std::map< std::string, float>  bandSpectrum = generateFourbandEnergySpectrum(fftResult);

        fourBandSpectrogram.push_back( bandSpectrum );
        
        atomicSpectrogramPercentage = readerStartSample / double(numberOfAudioSamples);
       
        readerStartSample += bufferSize;

		//reset the audioBuffer for the next frame
		audioBuffer.clear();
	}
}

std::map< std::string, float> MusicAnalyzer::generateFourbandEnergySpectrum(
                                                const std::array<float, fftSize*2>& spectrum)
{
	/**The first element of the spectrum is the DC component and to exclude it the binPointer
    starts from 1*/
    int binPointer = 1;
   
    std::map< std::string, float> bandSpectrum;

    float frequrncyResulotion = 1.0f / frameDuration;


    // (fftSize/2)-1 only includes the positive frequencies excluding Nyquist frequency.
    while (binPointer < fftSize/2-1)
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

void MusicAnalyzer::setLiveTime(float time)
{
    if (!isThreadRunning())
    {
       liveTime = time;
	   repaint();
    }
}


void MusicAnalyzer::setTimeBPM()
{
   
    {
        juce::ScopedLock scopeLock(lock);
        timeBPM.clear();
    }
    
    std::vector<bool> booleanBeatSpectrogram = extractBooleanBeatSpectrogram();

   /**run a separate loop for detecting the first beat so the second loop does not have to 
   use a condition for detecting the first beat and increase the performance */

    bool isFirstBeatDetected = false;
    int lastDetectedFrame = 0;

    for (int frameIndex = 0; frameIndex < booleanBeatSpectrogram.size(); frameIndex++)
    {
        if (threadShouldExit())
        {
            return;
        }
        if (booleanBeatSpectrogram[frameIndex] == true)
        {
            //ther is no previous beat detected so the fist is 0 BPM
            timeBPM[frameIndex] = 0;
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
                return;
            }
            if (booleanBeatSpectrogram[frameIndex] == true)
            {

                double frameTime = frameIndex * frameDuration;
                int BPM = static_cast<int>(60.0 / (frameTime - lastDetectedFrame * frameDuration));

                /**to ensure that multiple beats are not detected for a singgle event 
                (extended in multiple frames) a refactory time period is considered so time
                between beat detections can not be less than refactory time which is considered 
                250BPM also beat detections less than 60BPM (one per second is not very common and
                rythmic in music*/
                if (BPM < 60 || BPM > 250)
                {
                    continue;
                }
                timeBPM[frameIndex] = BPM;
                lastDetectedFrame = frameIndex;
            }
        }
    }
}

std::vector<bool> MusicAnalyzer::extractBooleanBeatSpectrogram()
{
    std::vector<bool> booleanBeatSpectrogram;

    //number of frames for one second
    int numberOfFramesInWindow = static_cast<int>(1.0f / frameDuration);

    /**it holds the energy of last one second frames.std::queue is used to optimize the process
    of removing the itme from front and add to the back*/
    std::queue< float> windowSpectrum;

    //add the first batch of total energy of frames as a baseline for beat detection
    for (int i = 0; i < numberOfFramesInWindow && i < fourBandSpectrogram.size(); i++)
    {
        windowSpectrum.push(fourBandSpectrogram[i]["all"]);

        //we do not detect beat for the first second of audio as we do not have enough data yet
        booleanBeatSpectrogram.push_back(false);
    }

    /**compare each next frame spectrum with a wincowSpectrum of 1_second period before the frame
    and detect beat*/
    for (int i = numberOfFramesInWindow; i < fourBandSpectrogram.size(); i++)
    {
        if (threadShouldExit())
        {
            return booleanBeatSpectrogram;
        }
        bool isBeatDetected = detectBeatOnFrame(windowSpectrum, fourBandSpectrogram[i]["all"]);
        booleanBeatSpectrogram.push_back(isBeatDetected);

        //remove the old frame from the window and add a new one for the next calculation
        windowSpectrum.pop();
        windowSpectrum.push(fourBandSpectrogram[i]["all"]);

        atomicBPMPercentage = (static_cast<double>(booleanBeatSpectrogram.size()) /
            (fourBandSpectrogram.size()));
    }
    return booleanBeatSpectrogram;
}

bool MusicAnalyzer::detectBeatOnFrame(std::queue< float> FIFOWindoAllSpectrum, float nextFrameEnergy)
{
    //to be able to itterate through items of windowSpectrom i changed queue to a vector
    std::vector< float> windoSpectrum;
    while (!FIFOWindoAllSpectrum.empty())
    {
        //moves the item to the vectorr without copying it
        windoSpectrum.push_back(std::move(FIFOWindoAllSpectrum.front()));
        FIFOWindoAllSpectrum.pop();
    }

    if (windoSpectrum.size() == 0)
    {
        return false;
    }

    //This map keeps the sum energy of all frames of one second
    float sumEnergy = 0;

    //This map keeps the average energy for all frames of one second
    float averageEnergy = 0;

    //calculate the sum of energy of all frames 
    for (const auto& energy : windoSpectrum)
    {
            sumEnergy+= energy;
    }

     averageEnergy= sumEnergy / windoSpectrum.size();

    /**check the energy of the nextFrameFourBandSpectrum and if it pass the cireteria return true
    otherwise return false.
    Formula from: https://gamedev.net/tutorials/programming/math-and-physics/beat-detection-algorithms-r1952*/

    if (nextFrameEnergy > 1.3 * averageEnergy)
    {
        return true;
    }

    return false;
}

int MusicAnalyzer::getLiveBPM()
{
    if (isThreadRunning() || frameDuration == 0 )
    {
        return 0;
    }
    int currentFrame = int(liveTime / frameDuration);
  
    juce::ScopedLock scopeLock(lock);
      
    //finds the next detected frame after this current time to return its BPM
    auto nextBeatedFramePointer = timeBPM.upper_bound(currentFrame);

    if (nextBeatedFramePointer != timeBPM.end())
    {
        return nextBeatedFramePointer->second;
    }

    return 0;
}