/*
  ==============================================================================

    DeckGUI.h
    Created: 17 Jul 2026 12:18:23pm
    Author:  hraha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "utilities.h"
#include "playlistComponent.h"
#include "MusicAnalyzer.h"
#include "KnobButton.h"
#include "OnOffButton.h"
#include <numbers>
#include "CueButton.h"
#include "LoopSampler.h"
//==============================================================================
/*
*/
class DeckGUI  : public juce::Component, 
                 public Button::Listener, 
                 public Slider::Listener, 
                 public FileDragAndDropTarget,
                 public Timer
	            
{
public:
    /**it creates a deck for the DJ app. left=true places cue buttons and knobs on left and left=false places them on right*/
    DeckGUI(DJAudioPlayer* _player, AudioFormatManager& formatManagerToUse, AudioThumbnailCache& cacheToUse, PlaylistComponent* _playlistComponent, bool left =true);
    ~DeckGUI() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    /** implement Button::Listener */
    void buttonClicked(Button*) override;

    /** implement Slider::Listener */
    void sliderValueChanged(Slider* slider) override;

    void filesDropped(const StringArray& files, int x, int y) override;
    bool isInterestedInFileDrag(const StringArray& files) override;

    void timerCallback()override;

    /**takes a file URL  and its data frin playListColmonent and loads it to the DJAudioPlayer, audioAnalyzer and WaveformDisplay and updates the loadedFile property*/
    void loadAudioFile(const URL& fileURL, FileStruct filedata);

    /**update addToLibraryButton */
	void updateAddToLibraryButton();

    /**set colour in cue data of FileStruct for corresponding cue button*/
    void setCueButtonColourData(int cueID, std::string colour);

    /**set name in cue data of FileStruct for corresponding cue button*/
    void setCueButtonNameData(int cueID, std::string name);

    /**set the time in the cue data of FileStruct for corresponding cue button*/
    void DeckGUI::setCueButtonTime(int cueIndex, double time);

private:
    bool left;

    juce::TextButton loadButton{ "LOAD" };
    juce::TextButton addToLibraryButton{ "Add to Library" };
    juce::TextButton clearCueButtons{ "Clear Cue Buttons" };

    WaveformDisplay waveformDisplay;

    PlaylistComponent* playlistComponent;

    /**holds the data of the last loaded audio trach to the deck*/
    FileStruct loadedFile;

    juce::Label trackNameLabel;
    juce::Label timerLabel;
    juce::Label BPMLabel;


    KnobButton volumeKnob{ juce::String("VOL") };
    KnobButton tempoKnob{ juce::String("BPM") };
    KnobButton positionKnob{ juce::String("POS") };

    OnOffButton playStopButton{ BinaryData::play_png, BinaryData::play_pngSize, BinaryData::pause_png, BinaryData::pause_pngSize };
    OnOffButton loopNoLoopButton{ BinaryData::loop_png, BinaryData::loop_pngSize, BinaryData::noloop_png, BinaryData::noloop_pngSize };

    double currentTime;

    double BPMRelativeRate = 1;

    DJAudioPlayer* player{};

    std::unique_ptr<FileChooser> fChooser;

	MusicAnalyzer musicAnalyzer;

    std::array<CueButton, 8> cueButtons;

    /**takes a file and loads it to the DJAudioPlayer and WaveformDisplay and updates the loadedFile property*/
    void loadAudioFile(File chosenFile);

    /**set the BPM label*/
    void DeckGUI::setBPMLabel();

    /**convert current time to String format and return*/
    juce::String currentTime2String() const;

    /**loads file on waveformDisplay and musicAnalyzer, updates metadata of loadedFile. playStopButton and labels 
    these tasks are put in a single function to be used in two different (overriden) loadAudioFile() with no repeatation
    */
    void DeckGUI::stageNewLoadedFile(juce::File file, juce::URL url);

    /**creates a dialog browser and selects a file */
    void DeckGUI::selectFile();

    /** resets the cueButtons: removes data and colour and disables them*/
    void resetCueButtons();

    /**reset loadedFile Struct, addToLibraryButton, playStopButton and  cueButtons before loading a new file */
    void resetComponentsBeforeLoadingFile();

    /**updates cueButtons status in accordance with passed FileStruct */
    void DeckGUI::updateCueButtonsStatus();
    /**disables cue buttons*/
    void DeckGUI::disableCueButtons();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
