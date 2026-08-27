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
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

    g.fillAll(juce::Colours::black);

    if (energy32_bandSpectrogram.size() != 0 && !isThreadRunning())
    {

        //exclude last band as it might cover the less amount of bins 
        float freqBandHeight = getHeight()/ static_cast<float>(bandsNumber - 1);
        float margin = getWidth() / 9.0f;
        int lastFrame = static_cast<int>(liveTime / frameDuration);
        int spectrogramSize = static_cast<int> (energy32_bandSpectrogram.size());

        if (lastFrame >= spectrogramSize)
        {
            lastFrame = spectrogramSize - 1;
        }
        //draw spectrogram
        //The width of each frame bar is considered 1px
        for (int frameIndex = lastFrame; frameIndex >= 0 && frameIndex >= lastFrame - getWidth(); frameIndex--)
        {
            //last band including frequences higher than about 21000 is excluded
            for (int i = 0; i < bandsNumber - 1; ++i)
            {
                float bandEnergy = energy32_bandSpectrogram[frameIndex][i];
                juce::uint8 blue = static_cast<juce::uint8>(bandEnergy * 100);
                juce::uint8 green = static_cast<juce::uint8>(std::pow(bandEnergy, 3.0f));
                juce::uint8 red = static_cast<juce::uint8>(std::pow(bandEnergy, 5.0f));
                g.setColour(juce::Colour(red, green, blue));
                g.fillRect(getWidth() - static_cast<float>(lastFrame - frameIndex) - margin,
                    getHeight() - (freqBandHeight * (i + 1)), float(1), freqBandHeight);

            }
        }

        //the reange of frequency in each bin
        float binsFrequencyRange = 1 / frameDuration;
        //the number of frequency bins excluding Nyquist frequency and DC component is fftSize/2-2
        //Except the last band, each band contains equal number of frequency bins. the following division will be truncated to be
        //converted to int, so the result is number of bins in all bands excluding the last one
        int binsPerBands = (fftSize / 2 - 2) / bandsNumber;
        //the frequency range of each band (except last band) 
        float bandsFrequencyRange = binsPerBands * binsFrequencyRange;

        g.setColour(juce::Colours::white);
        float fontSize = getHeight()/float(20);
        g.setFont(juce::FontOptions(fontSize));


        int bandsGridSpace = 6;
        for (int band = 1; band <=bandsNumber-2; band += bandsGridSpace)
        {
            g.drawLine(getWidth() - margin / 15.0f, getHeight() - freqBandHeight *band,
                static_cast<float>(getWidth()), getHeight() - freqBandHeight * band);

            String marker = String(band * bandsFrequencyRange / 1000.0f, 2) + " KHz";

            g.drawText(marker, getWidth() - static_cast<int>(margin),
                getHeight() - (band * static_cast<int>(freqBandHeight)) - static_cast<int>(fontSize / 2.0f),
                static_cast<int>(margin), static_cast<int>(fontSize), Justification::centred, false);
        }
        g.setColour(juce::Colours::grey);

		// an arrow marjker to show the location of live time frame in spectrogram
        Path liveFrame;
        float delta = getHeight() / 20.0f;
        liveFrame.startNewSubPath(getWidth()-margin, delta);
        liveFrame.lineTo(getWidth() - margin-delta, 0.0f);
        liveFrame.lineTo(getWidth() - margin+delta, 0.0f);
        liveFrame.closeSubPath();
        g.fillPath(liveFrame);
        
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
    analyzer();
    setTimeBPMVector();
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
void MusicAnalyzer::analyzer()
{
    //reset the spectrogram vector for new audio
    energy32_bandSpectrogram.clear();
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

        if (numberOfAudioSamples - readerStartSample >= fftSize)
        {
			bufferSize = fftSize;
        }
        else
        {
			bufferSize = static_cast<int>(numberOfAudioSamples - readerStartSample);
        }

        reader->read(&audioBuffer, 0, bufferSize, readerStartSample, true, false);

		//Reset fftResult to zero
        std::fill(fftResult.begin(), fftResult.end(), 0.0f);


		//transfer buffer to fftResult
        auto it = audioBuffer.getReadPointer(0);
        std::copy(it, it+bufferSize, fftResult.begin());
       
		fft.performFrequencyOnlyForwardTransform(fftResult.data(), true);

		//convert the fftResault to a 32-band energy spectrum and add it to the energy spectrogram
		std::array<float, bandsNumber> bandSpectrum = generateEnergy32_bandSpectrum(fftResult);
		energy32_bandSpectrogram.push_back(bandSpectrum);
        
        atomicSpectrogramPercentage = readerStartSample / double(numberOfAudioSamples);
       
        readerStartSample += bufferSize;

		//reset the audioBuffer for the next frame
		audioBuffer.clear();
	}

}

std::array<float, MusicAnalyzer::bandsNumber>MusicAnalyzer::generateEnergy32_bandSpectrum(const std::array<float, fftSize*2>& spectrum)
{
	//The first element of the spectrum is the DC component and to exclude it the freqPointer starts from 1 
    int freqPointer = 1;
	int bandIndex = 0;

    //the number of frequency bins excluding Nyquist frequency and DC component is fftSize/2-2
    //this formula just returns the integer part of the number. The last band is reserved for the remaining.
    int maxFrequencyBinsPerBand = (fftSize / 2 - 2) / (bandsNumber-1);
   
    std::array<float, MusicAnalyzer::bandsNumber> bandSpectrum{0.0f};

    // fftSize/2-1 only includes the positivefrequencies excluding Nyquist frequency.
    while (freqPointer < fftSize/2-1)
    {
        if (threadShouldExit())
        {
            return bandSpectrum;
        }

        float totalBandEnergy = 0.0f;
		
        int bandCounter = 0;
		for (int i = 0; (i < maxFrequencyBinsPerBand) && (freqPointer < fftSize / 2 - 1); ++i)
		{
            bandCounter++;
            totalBandEnergy += spectrum[freqPointer]* spectrum[freqPointer];
            freqPointer++;
		}
       
        float averageBandEnergy = totalBandEnergy / bandCounter;
        bandSpectrum[bandIndex++] = averageBandEnergy;
        if (bandIndex >= bandsNumber)
        {
            break;
        }
    }

    return bandSpectrum;
}


void MusicAnalyzer::setLiveTime(float time)
{
    if (!isThreadRunning())
    {
       liveTime = time;
	   repaint();
    }
}

void MusicAnalyzer::setTimeBPMVector()
{
    //reset the beatTime_BPM vedtor for new audio
    beatTimeBPM.clear();
    if (frameDuration == 0)
    {
        return;
    }
	std::vector<bool> booleanBeatVector = extractBooleanBeetVector();

    //run a separate loop for the first beat so the second loop does not have to use a condition  for detecting the first beat and
    // increase the performance 
    bool isFirstBeatDetected = false;
    int lastDetectedFrame = 0;
    for (int frameIndex = 0; frameIndex < booleanBeatVector.size(); frameIndex++)
    {
        if (threadShouldExit())
        {
            return;
        }
        if (booleanBeatVector[frameIndex] == true)
        {
            beatTimeBPM[frameIndex] = 0;
            lastDetectedFrame = frameIndex;
            isFirstBeatDetected = true;
            break;
        }
    }
    if (isFirstBeatDetected)
    {
        if (threadShouldExit())
        {
            return;
        }
        //continue dor the rest of the beats in the second loop
	    for (int frameIndex = lastDetectedFrame+1; frameIndex < booleanBeatVector.size(); frameIndex++)
	    {
            //to ensure that multiple beats are not detected for a singgle event (extended in multiple frames)
            // a refactory time period is considered so time between beat detections can not be less than 
            //refactory time
            if (booleanBeatVector[frameIndex] == true)
            {
                double beatTime = frameIndex * frameDuration;
                int BPM = static_cast<int>(60.0/(beatTime - lastDetectedFrame*frameDuration));
                //to ensure that multiple beats are not detected for a singgle event (extended in multiple frames)
                // a refactory time period is considered so time between beat detections can not be less than 
                //refactory time which is considered 300BPM
                //also beat detections less than 60BPM (one per second is not very common and rythmic in music
                //the reange of beats to be marked is considered between 60BPM and 350BPM
                if (BPM < 60 || BPM > 250)
                {
                    continue;
                }
                beatTimeBPM[frameIndex]= BPM;
                lastDetectedFrame = frameIndex;
            }
            atomicBPMPercentage = frameIndex / double(booleanBeatVector.size());
	    }
    }

}
std::vector<bool> MusicAnalyzer::extractBooleanBeetVector()
{
    //collect the fifo of the energy of about last 1 second for all bands

    int numberOfFiFoFrames = int(1 / frameDuration);
    //queue is used to store energye of recent frames. it makes the add to the back  and remove from front less expensive than vector.
    std::queue<std::array<float, bandsNumber>> fifo;

    //beat detection starts after a specific number of frames (numberOfFiFoFrames) are added to fifo data so
    // beat for the ealry frames up to numberOfFiFoFrames is set false
    std::vector<bool> beatVector(numberOfFiFoFrames,false);

    //fill the fifo with the required number of frames to start the algorithm
    for (int frameIndex = 0; frameIndex < numberOfFiFoFrames; frameIndex++)
    { 
         fifo.push(energy32_bandSpectrogram[frameIndex]);  
    }
    //exclude the last frame to gurantee that there is always a frameIndex+1 for comparision
    for (int frameIndex = numberOfFiFoFrames; frameIndex < energy32_bandSpectrogram.size() -1; frameIndex++)
    {
        if (threadShouldExit())
        {
            return beatVector;
        }
        //calculate the average energy of all bands
        std::queue<std::array<float, bandsNumber>> tempFifo = fifo;
        std::array<float, bandsNumber> sumVector{ 0.0f };

        while (tempFifo.size() > 0)
        {

            for (int bandIndex = 0; bandIndex < bandsNumber; bandIndex++)
            {
                sumVector[bandIndex] += tempFifo.front()[bandIndex];
            }
            tempFifo.pop();
        }

        for (int bandIndex = 0; bandIndex < bandsNumber; bandIndex++)
        {
            //compare each bands energy of the current frame with its correspomdig average energy and
            // if it is larger than its average energy * 1.5detect a beat for the frame
            //the other crieteria for beat detection is that the average energy of the frame is larger than one frame 
            //before and one frame after
            float average = sumVector[bandIndex] / float(numberOfFiFoFrames);
            if (energy32_bandSpectrogram[frameIndex][bandIndex] > average*1.5 && 
                energy32_bandSpectrogram[frameIndex-1][bandIndex] < energy32_bandSpectrogram[frameIndex][bandIndex]&&
                energy32_bandSpectrogram[frameIndex + 1][bandIndex] < energy32_bandSpectrogram[frameIndex][bandIndex])
            {
                beatVector.push_back(true);
                break;
            }
        }
        //if beat is not detected for this frame add false to the beat vector
        if (beatVector.size() < frameIndex + 1)
        {
            beatVector.push_back(false);
        }

        //add frame to fifo and rempve the oldest one
        fifo.pop();
        fifo.push(energy32_bandSpectrogram[frameIndex]);
        
    }
    return beatVector;
}

int MusicAnalyzer::getLiveBPM()
{
    int currentFrame = int(liveTime / frameDuration);
    if (isThreadRunning())
    {
        return 0;
    }
    if (beatTimeBPM.count(currentFrame))
    {
        //live BPM
        lastReadFrame = currentFrame;
        return beatTimeBPM[currentFrame];
    }
    else
    {
        //if the current frame is not in the map, return the latest frame that has a BPM in the map
		return beatTimeBPM[lastReadFrame];
    }

    return 0;
}

void MusicAnalyzer::timerCallback()
{
    spectrogramPercentage = static_cast<double>(atomicSpectrogramPercentage);
    BPMPercentage = static_cast<double>(atomicBPMPercentage);
}

