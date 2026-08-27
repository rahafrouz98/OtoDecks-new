/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (1200, 900);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }  
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    } 
    addAndMakeVisible(deckGuiLeft);
    addAndMakeVisible(deckGuiRight);
    addAndMakeVisible(playlistComponent);

    formatManager.registerBasicFormats();
    
    //define the callback functions of playlistComponent for clicking the load buttons
    playlistComponent.setLoadDeck1Callback([this](URL url, FileStruct filedata) {deckGuiLeft.loadAudioFile(url, filedata);});
    playlistComponent.setLoadDeck2Callback([this](URL url, FileStruct filedata) {deckGuiRight.loadAudioFile(url, filedata);});

    //callback function to update the addToLibraryButton of DeckGui instances when a track is deleted from the library 
    playlistComponent.setDeleteCallback([this]() {
        deckGuiLeft.updateAddToLibraryButton();
        deckGuiRight.updateAddToLibraryButton();
     });

    /////////////////////////////////// Microphone //////////////////////////////////////////////
    addAndMakeVisible(microphone);

    /////////////////////////////////// Loop sampler ////////////////////////////////////////////
    addAndMakeVisible(loopSampler);

}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    loopSampler.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource.addInputSource(&leftPlayer, false);
    mixerSource.addInputSource(&rightPlayer, false);
    mixerSource.addInputSource(&microphone, false);
    mixerSource.addInputSource(&samplerPlayer, false);

 }
void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    microphone.setInputBuffer(bufferToFill);
    mixerSource.getNextAudioBlock(bufferToFill);
    loopSampler.processSignals();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    leftPlayer.releaseResources();
    rightPlayer.releaseResources();
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}
void MainComponent::paintOverChildren(Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    if (!looperAndMicrophonWrapper.isEmpty())
    {
        g.drawRect(looperAndMicrophonWrapper, 1);
    }
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    auto decksArea = area.removeFromTop(static_cast<int>(getHeight() * 2.0f / 3.0f));

    auto deckLeftArea = decksArea.removeFromLeft(static_cast<int>(getWidth() *3 / 8.0f));
    deckGuiLeft.setBounds(deckLeftArea);

    auto loopsamplerMicArea = decksArea.removeFromLeft(static_cast<int>(getWidth() * 2 / 8.0f));

    deckGuiRight.setBounds(decksArea);

    auto playlistArea = area;
    playlistComponent.setBounds(playlistArea);
                                         
    looperAndMicrophonWrapper = loopsamplerMicArea;
    auto micArea = loopsamplerMicArea.removeFromTop(static_cast<int>(loopsamplerMicArea.getHeight() * 0.5f));
                                  
    auto loopSamplerArea = loopsamplerMicArea;

    microphone.setBounds(micArea);
    loopSampler.setBounds(loopSamplerArea);
    
}


