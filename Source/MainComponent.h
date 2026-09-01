#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "playlistComponent.h"
#include "utilities.h"
#include "Microphone.h"

class MainComponent   : public AudioAppComponent            
{
public:
    MainComponent();
    ~MainComponent();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void paintOverChildren(Graphics& g) override;
   
    void paint (Graphics& g) override;
    void resized() override;

private:

    AudioFormatManager formatManager;
    PlaylistComponent playlistComponent;

    
    DJAudioPlayer leftPlayer{ formatManager };
    DJAudioPlayer rightPlayer{ formatManager };
    DJAudioPlayer samplerPlayer{ formatManager };

    AudioThumbnailCache thumbCache{100};

    DeckGUI deckGuiLeft{&leftPlayer, formatManager, thumbCache, &playlistComponent };
    DeckGUI deckGuiRight{&rightPlayer, formatManager, thumbCache, &playlistComponent, false };
    Microphone microphone{deviceManager};

    LoopSampler loopSampler{ &samplerPlayer, leftPlayer, rightPlayer, microphone, formatManager, thumbCache };
    MixerAudioSource mixerSource;

    juce::Rectangle<int> looperAndMicrophonWrapper;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
