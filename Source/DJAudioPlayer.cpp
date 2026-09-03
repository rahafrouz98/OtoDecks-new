#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager) :
    formatManager(_formatManager), isLooping(true)
{
}
DJAudioPlayer::~DJAudioPlayer()
{
}

void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) 
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void DJAudioPlayer::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) 
{
    resampleSource.getNextAudioBlock(bufferToFill);

    int numSamples = bufferToFill.numSamples;
    //save the last fetched buffer from resampleSource
    latestBuffer.setSize(2, numSamples, false, true, true);
    for (int channel = 0; channel < latestBuffer.getNumChannels(); ++channel)
    {
        latestBuffer.copyFrom(channel, 
                              0, 
                              *bufferToFill.buffer, 
                              channel,bufferToFill.startSample,
                              bufferToFill.numSamples);
    }
}
void DJAudioPlayer::releaseResources()
{
    resampleSource.releaseResources();
    transportSource.releaseResources();
}

bool DJAudioPlayer::loadURL(URL audioURL)
{
    juce::File file = (audioURL.getLocalFile());
    juce::AudioFormat* extention = formatManager.findFormatForFileExtension(file.getFileExtension());
    
    if (extention == nullptr)
    {
        return false;
    }
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(true));
    if (reader != nullptr) // good file!
    {
        std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader,
            true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
		readerSource->setLooping(isLooping);

        return true;
    }
    return false;
}

void DJAudioPlayer::setGain(double gain)
{
    if (gain >= 0.0 && gain <= 2.0)
    {
        transportSource.setGain(gain);
    }
}
void DJAudioPlayer::setSpeed(double ratio)
{
    if (ratio > 0 && ratio < 100.0)
    {
        resampleSource.setResamplingRatio(ratio);
    }
}
void DJAudioPlayer::setPosition(double posInsecs)
{
    transportSource.setPosition(posInsecs);
    //to clean the buffer from samples hold for interpolation
    // when music jumps to another location
    resampleSource.flushBuffers();
}

void DJAudioPlayer::start()
{
    transportSource.start();
}
void DJAudioPlayer::stop()
{
    transportSource.stop();
}

float DJAudioPlayer::getPostionRelative() const
{
    if (transportSource.getLengthInSeconds() <= 0)
    {
        return 0.0;
    }
    return float(transportSource.getCurrentPosition() / 
                 transportSource.getLengthInSeconds());
}
double DJAudioPlayer::calculateAudioLength() const
{ 
    return transportSource.getLengthInSeconds();
}
double DJAudioPlayer::getCurrentPosition() const
{
    return transportSource.getCurrentPosition();
}
AudioFormatReader* DJAudioPlayer::getAudioFormatReader() const
{
	if (readerSource != nullptr)
	{
		return readerSource->getAudioFormatReader();
	}
	return nullptr;
}

void DJAudioPlayer::setPostion(double targetTime)
{
    transportSource.setPosition(targetTime);
}

bool DJAudioPlayer::isPlaying() const
{
    return transportSource.isPlaying();
}
void DJAudioPlayer::toggleLooping()
{
    isLooping = !isLooping;

    if (readerSource != nullptr)
    {
        //this reseting the position is because when the looping status 
        // of reader changes, for some reason the total length of the audio
        // is added to the transortSource postion and it cause when it is set to no loop
        // the playhead jump to the end
        auto transportPosition = transportSource.getCurrentPosition();
        readerSource->setLooping(isLooping);
        transportSource.setPosition(transportPosition);

    }

}
void DJAudioPlayer::setLoopingStatus(bool status)
{
    isLooping = status;
    if (readerSource != nullptr)
    {
        readerSource->setLooping(isLooping);
    }
}

const juce::AudioBuffer<float>& DJAudioPlayer::getLatestBuffer()const
{
    return latestBuffer;
}

void DJAudioPlayer::unloadFile()
{
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
}
