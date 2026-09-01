#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "utilities.h"
#include "playlistComponent.h"
#include "MusicAnalyzer.h"
#include "UI/KnobButton.h"
#include "UI/OnOffButton.h"
#include <numbers>
#include "UI/CueButton.h"
#include "LoopSampler.h"

class DeckGUI  : public juce::Component, 
                 public juce::Button::Listener, 
                 public juce::Slider::Listener, 
                 public juce::FileDragAndDropTarget,
                 public juce::Timer
	            
{
public:
    /**it creates a deck for the DJ app. left=true places cue buttons and knobs on 
    left and left=false places them on right*/
    DeckGUI(DJAudioPlayer* _player,
            AudioFormatManager& formatManagerToUse, 
            AudioThumbnailCache& cacheToUse, 
            PlaylistComponent* _playlistComponent, 
            bool left =true);

    ~DeckGUI() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    /** implement Button::Listener */
    void buttonClicked(Button*) override;

    /** implement Slider::Listener */
    void sliderValueChanged(Slider* slider) override;

    /**implement FileDragAndDropTarget*/
    void filesDropped(const StringArray& files, int x, int y) override;
    bool isInterestedInFileDrag(const StringArray& files) override;

    /**implement Timer*/
    void timerCallback()override;

    /**take a FileStruct of an audio form playListColmonent and loads it to the
    DJAudioPlayer, audioAnalyzer and WaveformDisplay and updates the loadedFile property*/
    void loadAudioFile(Utilities::FileStruct filedata);

    /**update addToLibraryButton. If there is a file loaded in the deck and the url of 
    file is unique in the playlist it enables the addToLibraryButton otherwise it will be 
    disabled*/
	void updateAddToLibraryButton();

    /**set colour in cue data of FileStruct for corresponding cue button*/
    void setCueButtonColourData(int cueID, juce::Colour colour);

    /**set name in cue data of FileStruct for corresponding cue button*/
    void setCueButtonNameData(int cueID, juce::String name);

    /**set the time in the cue data of FileStruct for corresponding cue button*/
    void DeckGUI::setCueButtonTimeData(int cueIndex, double time);

private:

    bool isleft;

    juce::ComboBox frequencyBandSelector;

    juce::TextButton loadButton{ "LOAD" };
    juce::TextButton addToLibraryButton{ "ADD TO LIBRARY" };
    juce::TextButton clearCueButtons{ "CLEAR CUE BUTTON" };

    WaveformDisplay waveformDisplay;

    PlaylistComponent* playlistComponent;

    /**hold the data of the loaded audio track */
    Utilities::FileStruct loadedFile;

    juce::Label trackNameLabel;
    juce::Label timerLabel;
    juce::Label BPMLabel;


    KnobButton volumeKnob{ juce::String("VOL") };
    KnobButton tempoKnob{ juce::String("BPM") };
    KnobButton positionKnob{ juce::String("POS") };

    OnOffButton playStopButton{ BinaryData::play_png, BinaryData::play_pngSize, 
                                BinaryData::pause_png, BinaryData::pause_pngSize };

    OnOffButton loopNoLoopButton{ BinaryData::loop_png, BinaryData::loop_pngSize, 
                                  BinaryData::noloop_png, BinaryData::noloop_pngSize };
    double currentTime;

    double BPMRelativeRate = 1;

    DJAudioPlayer* player{};

    std::unique_ptr<FileChooser> fChooser;

	MusicAnalyzer musicAnalyzer;

    std::array<CueButton, 8> cueButtons;

    /**take a file and loads it to the DJAudioPlayer and WaveformDisplay and updates
    the loadedFile data*/
    void loadAudioFile(File chosenFile);

    /**set the BPM label*/
    void DeckGUI::setBPMLabel();

    /**load file on waveformDisplay and musicAnalyzer, updates metadata of loadedFile. 
    playStopButton and labels these tasks are put in a single function to be used in two 
    different (overriden) loadAudioFile() with no repeatation*/
    void DeckGUI::stageNewLoadedFile(juce::File file);

    /**create a dialog browser and selects a file */
    void DeckGUI::selectFile();

    /** reset all cueButtons: removes data and colour and disables them*/
    void resetCueButtons();

    /**reset loadedFile Struct, addToLibraryButton, playStopButton and  cueButtons before
    loading a new file */
    void resetComponentsBeforeLoadingFile();

    /**update cueButtons status in accordance with passed FileStruct */
    void DeckGUI::updateCueButtonsStatus();
    
    /**disable cue buttons*/
    void DeckGUI::disableCueButtons();

    /**return a string to represent the current position of player and total length
    in seconds*/
    String currentTime2String() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
