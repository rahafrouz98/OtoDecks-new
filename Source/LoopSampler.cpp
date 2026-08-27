/*
  ==============================================================================

    LoopSampler.cpp
    Created: 23 Aug 2026 7:52:08am
    Author:  hraha

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LoopSampler.h"

//==============================================================================
LoopSampler::LoopSampler(DJAudioPlayer* _player, DJAudioPlayer& _leftPlayer,
    DJAudioPlayer& _rightPlayer, Microphone& _microphone, AudioFormatManager& _formatManagerToUse,
    AudioThumbnailCache& _cacheToUse): leftPlayer(_leftPlayer), rightPlayer(_rightPlayer),microphone(_microphone),
    player(_player), waveformDisplay(_formatManagerToUse, _cacheToUse)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    /////////////////////////////////////////////// loop sample buttons ///////////////////////////////////////////////
    for (int i = 0; i < sampleButtons.size(); ++i)
    {
        addAndMakeVisible(sampleButtons[i]);
        sampleButtons[i].addListener(this);
        sampleButtons[i].setTextChangeCallBack([this](juce::String text) {(text.toUpperCase());});
        sampleButtons[i].setID(i);
    }

    ///////////////////////////////////////////// left deck button //////////////////////////////////////////////
    addAndMakeVisible(leftDeckImageButton);
    leftDeckImageButton.addListener(this);
    leftDeckImageButton.setFirstMode(false);

    ////////////////////////////////////////////// Start stop record button //////////////////////////////////////////////
    addAndMakeVisible(startStopRecordImageButton);
    startStopRecordImageButton.addListener(this);

    ////////////////////////////////////////////// delete image button ///////////////////////////////////////////
    addAndMakeVisible(deleteImageButton);
    deleteImageButton.addListener(this);
    deleteImageButton.setButtonEnabled(false);
    ///////////////////////////////////////////// play sample record //////////////////////////////////////////////
    addAndMakeVisible(playSampleImageButton);
    playSampleImageButton.addListener(this);
    

    ///////////////////////////////////////////// right deck button //////////////////////////////////////////////
    addAndMakeVisible(rightDeckImageButton);
    rightDeckImageButton.addListener(this);
    rightDeckImageButton.setFirstMode(false);

    ///////////////////////////////////////////// wavedisplay ///////////////////////////////////////////////////////
    addAndMakeVisible(waveformDisplay);

    ///////////////////////////////////////////// Timer ////////////////////////////////////////////////////////////
    startTimer(100);


}

LoopSampler::~LoopSampler()
{
    writeLoopSamplesData();
    stopTimer();
}

void LoopSampler::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

}

void LoopSampler::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    auto area = getLocalBounds();
    area = area.reduced(getWidth() / 70.0f);

    auto loopSamplesArea = area.removeFromTop(getHeight() * .60);

    // it is a four row table of loopSample buttons
    int cueButtonHeight = static_cast<int>(loopSamplesArea.getHeight() / 4.0f);
    int cueButtonWidth = static_cast<int>(loopSamplesArea.getWidth() / 2.0f);

    for (int row = 0; row < 4; ++row)
    {
        auto rowArea = loopSamplesArea.removeFromBottom(cueButtonHeight);
        for (int col = 0; col < 2; ++col)
        {
            int index = row * 2 + col;
            if (index >= sampleButtons.size())
            {
                break;
            }
            auto cueButtonArea = rowArea.removeFromLeft(cueButtonWidth).reduced(static_cast<int>(cueButtonWidth*.01f));
            sampleButtons[index].setBounds(cueButtonArea);
        }
    }

    auto waveArea = area.removeFromTop(getHeight() * .20);
    waveformDisplay.setBounds(waveArea);

    auto buttonArea = area.withTrimmedLeft(static_cast<int>(getWidth() / 6.0f)).withTrimmedRight(static_cast<int>(getWidth() /6.0f)).
        withTrimmedTop(static_cast<int>(getHeight() / 40));
    
    auto leftDeckButtonArea = buttonArea.removeFromLeft(static_cast<int>(buttonArea.getWidth() / 5.0f)).reduced(static_cast<int>(getHeight() / 70.0f));
    leftDeckImageButton.setBounds(leftDeckButtonArea);

    auto starStoptRecordArea = buttonArea.removeFromLeft(static_cast<int>(buttonArea.getWidth() / 4.0f)).reduced(static_cast<int>(getHeight() / 70.0f));
    startStopRecordImageButton.setBounds(starStoptRecordArea);

    auto deleteArea = buttonArea.removeFromLeft(static_cast<int>(buttonArea.getWidth() / 3.0f)).reduced(static_cast<int>(getHeight() / 70.0f));
    deleteImageButton.setBounds(deleteArea);

    auto playSampleArea = buttonArea.removeFromLeft(static_cast<int>(buttonArea.getWidth() / 2.0f)).reduced(static_cast<int>(getHeight() / 70.0f));
    playSampleImageButton.setBounds(playSampleArea);

    auto rightDeckButtonArea = buttonArea.reduced(static_cast<int>(getHeight() / 70.0f));
    rightDeckImageButton.setBounds(rightDeckButtonArea);

}

void LoopSampler::processSignals()
{

    if (startStopRecordImageButton.getStatus())
    {
        return;
    }
    samplerBuffer.clear();
    const juce::AudioBuffer<float>& micBuffer = microphone.getProceccedBuffer();
    const juce::AudioBuffer<float>& leftPlayerBuffer = leftPlayer.getLatestBuffer();
    const juce::AudioBuffer<float>& rightPlayerBuffer = rightPlayer.getLatestBuffer();

    //get the next block of samples from the left player
    if (microphone.getStatus())
    {
        for (int channel = 0; channel < samplerBuffer.getNumChannels(); ++channel)
        {
            samplerBuffer.addFrom(channel, 0, micBuffer, channel, 0, leftPlayerBuffer.getNumSamples());
        }
    }
    
    //get the next block of samples from the left player
    if (leftDeckImageButton.getStatus())
    {

        for (int channel = 0; channel < samplerBuffer.getNumChannels(); ++channel)
        {
            samplerBuffer.addFrom(channel, 0, leftPlayerBuffer, channel ,0, leftPlayerBuffer.getNumSamples());
        }
    }
    //get the next block of samples from the right player
    if (rightDeckImageButton.getStatus())
    {
        for (int channel = 0; channel < samplerBuffer.getNumChannels(); ++channel)
        {
            samplerBuffer.addFrom(channel, 0, rightPlayerBuffer, channel, 0, rightPlayerBuffer.getNumSamples());
        }
    }
   
    writeOnFile(samplerBuffer, 0, samplerBuffer.getNumSamples());

}
void LoopSampler::writeOnFile(juce::AudioBuffer<float> buffer, int startSample,int numSamples )
{
    if (writer != nullptr)
    {
        writer->writeFromAudioSampleBuffer(buffer, startSample, numSamples);
    }
}

void LoopSampler::startRecording()
{
    juce::File file = selectSampleAudioFile();
    if (file == juce::File{})
    {
        startStopRecordImageButton.setFirstMode(true);
        return;
    }


    std::unique_ptr<juce::OutputStream> stream = file.createOutputStream();

    writer.reset(wavFormat.createWriterFor(stream.release(), possibleSampleRate,
                                             static_cast<unsigned int>(numberOfChannels), possibleBitsPerSample, {}, 0));

}


juce::File LoopSampler::selectSampleAudioFile()
{

    juce::File mainDirectory = juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory();
    juce::File sampleDir = mainDirectory.getChildFile("samples");
    if (!sampleDir.exists())
    {
        sampleDir.createDirectory();
    }
   
    juce::File sampledFile = sampleDir.getChildFile(juce::String(fileNumberHolder)).withFileExtension("wav");

    if (sampledFile.existsAsFile())
    {
        sampledFile.deleteFile();
    }
    sampledURL = juce::URL{ sampledFile };
    fileNumberHolder++;
    return sampledFile;
}

void LoopSampler::setNumberOfChannels(int numChannels)
{
    numberOfChannels = numChannels;
}

void LoopSampler::releaseResources()
{

}
void LoopSampler::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{

}
void LoopSampler::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    numberOfChannels = 2;
    samplerBuffer.setSize(numberOfChannels, samplesPerBlockExpected,false,true,true);

}
void LoopSampler::removeSampleFromRecordingSection()
{
    deleteImageButton.setButtonEnabled(false);
    player->unloadFile();
    playSampleImageButton.setFirstMode(true);
    playSampleImageButton.setEnabled(false);
    waveformDisplay.unloadURL();
}
void LoopSampler::buttonClicked(Button* button)
{
    //delete the recorded sample
    if (static_cast<const juce::Button*>(button) == deleteImageButton.getButtonPointer())
    {
        removeSampleFromRecordingSection();
        startStopRecordImageButton, setEnabled(true);
    }

    else if (static_cast<const juce::Button*>(button) == startStopRecordImageButton.getButtonPointer())
    {
        if (startStopRecordImageButton.getStatus())
        {
            //start recording
            waveformDisplay.unloadURL();
            startRecording();
            deleteImageButton.setButtonEnabled(false);
            disableAddRemoveForEmptysampleButtons();

        }
        else
        {
            //stop recording
            writer.reset();
            if (sampledURL != juce::URL{})
            {
                player->loadURL(sampledURL);
                deleteImageButton.setButtonEnabled(true);
                playSampleImageButton.setEnabled(true);
                waveformDisplay.loadURL(sampledURL);
                enableAddRemoveForEmptysampleButtons();
                startStopRecordImageButton.setEnabled(false);
            }
        }
    }
    else if (static_cast<const juce::Button*>(button) == playSampleImageButton.getButtonPointer())
    {
        if (player->isPlaying())
        {
            player->stop();

        }
        else
        {
            player->start();
        }
    }
    //check the buttons of each sampleButtons
    else
    {
        // paly and stop button on sample buttons
        for (auto& sampleButton : sampleButtons)
        {
            if (static_cast<const juce::Button*>(button) == sampleButton.getPlayStopButtonPointer())
            {
                //play sample
                if (sampleButton.getPlayStopButtonStatus())
                {
                    player->loadURL(sampleButton.getURL());
                    player->start();
                    waveformDisplay.loadURL(sampleButton.getURL());
                    
                }
                //stop sample
                else
                {
                    player->stop();
                }
            }
            // add and remove button on sample buttons
            else if (static_cast<const juce::Button*>(button) == sampleButton.getAddRemoveButtonPointer())
            {
                //add sample
                if (sampleButton.getAddRemoveButtonStatus())
                {
                    //unload the url from recording gsection
                    player->unloadFile();
                    juce::String loopName = "loop " + juce::String(sampleButton.getID()+1);
                    sampleButton.setSample(sampledURL, loopName);
                    removeSampleFromRecordingSection();
                    disableAddRemoveForEmptysampleButtons();
                    samplesRecord[sampleButton.getID()].url = sampledURL;
                    samplesRecord[sampleButton.getID()].name = loopName;
                    int index = sampleButton.getID();
                    DBG("loopName: " << loopName);
                    DBG("ID: " << index);
                    DBG("samplesRecord name: " << samplesRecord[index].name);
                    startStopRecordImageButton.setEnabled(true);

                }
                //remove sample
                else
                {
                    deleteLocalFile(sampleButton.getURL());
                    samplesRecord[sampleButton.getID()].name = "";
                    samplesRecord[sampleButton.getID()].url = juce::URL{};
                    sampleButton.resetButtonData();
                    if (sampledURL == juce::URL{})
                    {
                        sampleButton.setAddButtonEnabled(false);
                    }
                }
            }
        }
    }

}
void LoopSampler::enableAddRemoveForEmptysampleButtons()
{
    for (auto& sampleButton : sampleButtons)
    {
        if (sampleButton.getURL() == juce::URL{})
        {
            sampleButton.setAddButtonEnabled(true);
        }
    }
}

void LoopSampler::disableAddRemoveForEmptysampleButtons()
{
    for (auto& sampleButton : sampleButtons)
    {
        if (sampleButton.getURL() == juce::URL{})
        {
            sampleButton.setAddButtonEnabled(false);
        }
    }
}

void LoopSampler::timerCallback()
{
    if (player != nullptr)
    {
        waveformDisplay.setPlayHeadPosition(player->getPostionRelative());
    }
}

void LoopSampler::deleteLocalFile(juce::URL url)
{
    juce::File file = url.getLocalFile();

    if (file.existsAsFile())
    {
        file.deleteFile();
    }

}

void LoopSampler::writeLoopSamplesData()
{
    //convert data to json
    juce::Array<juce::var> data;
    for (int i = 0; i < samplesRecord.size(); ++i)
    {
        juce::DynamicObject* sampleObject = new juce::DynamicObject();
        sampleObject->setProperty("url", samplesRecord[i].url.toString(false));
        sampleObject->setProperty("name", samplesRecord[i].name);
        data.add(sampleObject);
    }
    
    const juce::var dataVar{ data };

    //select folder and file
    Utilities::writeJsonData("samples", "sampleData", dataVar);

}

void LoopSampler::loadLoopSamplesData()
{
    juce::var varData = Utilities::loadJsonData("samples", "sampleData");

    juce::Array arrayData{ varData };



}