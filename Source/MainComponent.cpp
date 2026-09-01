#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize (1200, 900);

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
    playlistComponent.setLoadDeckLeftCallback([this](Utilities::FileStruct filedata) {
            deckGuiLeft.loadAudioFile(filedata);
        });

    playlistComponent.setLoadDeckRightCallback([this](Utilities::FileStruct filedata) {
            deckGuiRight.loadAudioFile(filedata);
        });

    //callback function to update the addToLibraryButton of DeckGui instances when a track
    // is deleted from the library 
    playlistComponent.setDeleteCallback([this]() {
            deckGuiLeft.updateAddToLibraryButton();
            deckGuiRight.updateAddToLibraryButton();
         });

    /////////////////////////////////// Microphone ///////////////////////////////////////
    addAndMakeVisible(microphone);

    /////////////////////////////////// Loop sampler ////////////////////////////////////
    addAndMakeVisible(loopSampler);

}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    loopSampler.prepareToRcord(samplesPerBlockExpected, 2);
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

    leftPlayer.releaseResources();
    rightPlayer.releaseResources();
}

void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}
void MainComponent::paintOverChildren(Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    auto decksArea = area.removeFromTop(static_cast<int>(getHeight() * 2.0f / 3.0f));

    auto deckLeftArea = decksArea.removeFromLeft(static_cast<int>(getWidth() *3 / 8.0f));
    deckGuiLeft.setBounds(deckLeftArea);

    auto loopsamplerMicArea = decksArea.removeFromLeft(static_cast<int>(getWidth() * 2 / 8.0f));

    deckGuiRight.setBounds(decksArea);

                                         
    auto loopSamplerArea = loopsamplerMicArea.removeFromTop(loopsamplerMicArea.getHeight() * 5.0f / 6.0f);
    
    looperAndMicrophonWrapper = loopsamplerMicArea;
    
    auto micArea = loopsamplerMicArea;
                                  
    loopSampler.setBounds(loopSamplerArea);
    
    microphone.setBounds(micArea);

    auto playlistArea = area;

    playlistComponent.setBounds(playlistArea);
}


