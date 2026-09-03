#include "DeckGUI.h"

DeckGUI::DeckGUI( DJAudioPlayer* _player, 
                  AudioFormatManager& formatManagerToUse, 
                  AudioThumbnailCache& cacheToUse, 
                  PlaylistComponent* _playlistComponent, 
                  bool _left):
                      player(_player),
                      waveformDisplay(formatManagerToUse, cacheToUse), 
                      playlistComponent(_playlistComponent), 
                      currentTime(0.0), 
                      isleft(_left)
{

    //////////////////////////////// WaveformDisplay //////////////////
    addAndMakeVisible(waveformDisplay);

    waveformDisplay.setMouseClickCallback([this](float relativePosition) {
            double targetTime = relativePosition* loadedFile.duration;
            player->setPosition(targetTime);
        });
    
    ///////////////////////////////buttons///////////////////////////////
    addAndMakeVisible(playStopButton);
	playStopButton.setButtonEnabled(false);
    playStopButton.addListener(this);

    addAndMakeVisible(loopNoLoopButton);
    loopNoLoopButton.addListener(this);

    addAndMakeVisible(loadButton);
    loadButton.addListener(this);

    addAndMakeVisible(addToLibraryButton);
    addToLibraryButton.setEnabled(false);
    addToLibraryButton.addListener(this);

    addAndMakeVisible(clearCueButtons);
    clearCueButtons.addListener(this);

    ////////////////////////// Cue buttons ///////////////////////////////////////

    for (int i = 0; i < cueButtons.size() ; i++)
    {
        addAndMakeVisible(cueButtons[i]);

        cueButtons[i].setEnableButtons(false);

        //set callback to receive data from CueEditForm when its save button is clicked
        cueButtons[i].setCueButtonEditedCallback( [this, i](juce::Colour colour,
                                                  juce::String name) {
                setCueButtonNameData(i, name);
                setCueButtonColourData(i, colour);
            });

        /**set callback to receive add button clicked event and add colour to button,
        mark time and update cue data int the loadedFile Struct*/
        cueButtons[i].setCueButtonAddCallback([this,i]() {
                cueButtons[i].setMarkedTime(currentTime);
                juce::Colour colour = CueEditForm::convertCueColourToJuceColour( static_cast<CueEditForm::CueColour>(i));
                cueButtons[i].setCueButtonColour(colour);
                setCueButtonColourData(i, colour);
                setCueButtonTimeData(i, currentTime);
             
            });

        //set callback to receive remove button clicked event, remove colour, mark time and remove them from data
        cueButtons[i].setCueButtonRemoveCallback([this, i]() {
                //reset the button
                cueButtons[i].setMarkedTime(-1.0);
                cueButtons[i].setCueButtonColour(juce::Colours::transparentBlack);
                cueButtons[i].setCueButtonName("");
                //remove data from FileStruct
                setCueButtonNameData(i, "");
                setCueButtonColourData(i, juce::Colours::transparentBlack);
                setCueButtonTimeData(i, -1.0);
            });

        //Register DechGUI to the mainButton of cueButton Component
        cueButtons[i].addListener(this);
    }

    ////////////////////////// labels/////////////////////////////////////////
	addAndMakeVisible(trackNameLabel);
    trackNameLabel.setText(loadedFile.name, dontSendNotification);
    trackNameLabel.setJustificationType(Justification::centred);
    trackNameLabel.setColour(Label::ColourIds::backgroundColourId, Colours::black);
    trackNameLabel.setColour(Label::ColourIds::outlineColourId, Colours::grey);

    addAndMakeVisible(timerLabel);
    timerLabel.setJustificationType(Justification::centred);
    timerLabel.setColour(Label::ColourIds::backgroundColourId, Colours::black);
    timerLabel.setColour(Label::ColourIds::outlineColourId, Colours::grey);

    addAndMakeVisible(BPMLabel);
    BPMLabel.setJustificationType(Justification::centred);
    BPMLabel.setColour(Label::ColourIds::backgroundColourId, Colours::black);
    BPMLabel.setColour(Label::ColourIds::outlineColourId, Colours::grey);

    ///////////////////////////////Knobs////////////////////////////////////////////
    addAndMakeVisible(tempoKnob);
    tempoKnob.setRange(0.1, 10.0);
    tempoKnob.setKnobValue(1.0);

    addAndMakeVisible(volumeKnob);
    volumeKnob.setRange(0.0, 2.0);
    volumeKnob.setKnobValue(1.0);
    addAndMakeVisible(positionKnob);

    positionKnob.addListener(this);
    tempoKnob.addListener(this);
    volumeKnob.addListener(this);

    ///////////////////////// MusicAnalyzer ///////////////////////////////////////
    addAndMakeVisible(musicAnalyzer);

    //////////////////////// Timer //////////////////////////////////////////////
    startTimer(20);

}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);  
}

void DeckGUI::resized()
{
    auto area = getLocalBounds();


    int height = getHeight();
    ////////////////////////////////////////////////////////// Spectrogram ///////////////////////////////
    musicAnalyzer.setBounds(area.removeFromTop(static_cast<int>(height * 0.3f)));

    ////////////////////////////////////////////////////////// Waveform /////////////////////////////////
    waveformDisplay.setBounds(area.removeFromTop(static_cast<int>(height * 0.1f)));
    
    int width = area.getWidth();
    auto labelArea = area.removeFromTop(static_cast<int>(height * 0.05f));
    auto rowOneArea = area.removeFromTop(static_cast<int>(height * 0.25f));
    auto rowTwoArea = area.removeFromTop(static_cast<int>(height * 0.25f));
    auto rowThreeArea = area;

    //////////////////////////////// components justification ////////////////////////////////

    juce::Rectangle<int> timerlabelArea;
    juce::Rectangle<int> BPMLAbelArea;
    juce::Rectangle<int> nameLabelArea;
    juce::Rectangle<int> loopButtonArea;
    juce::Rectangle<int> playStopArea;
    juce::Rectangle<int> knobsArea;
    juce::Rectangle<int> tempoArea;
    juce::Rectangle<int> volArea;
    juce::Rectangle<int> posArea;
    juce::Rectangle<int> loadAndLibButtonArea;
    juce::Rectangle<int> cueArea;

    if (isleft)
    {
        nameLabelArea = labelArea.removeFromLeft( width / 3 );
        timerlabelArea = labelArea.removeFromLeft (width / 3 );
        BPMLAbelArea = labelArea;

        //row one
        knobsArea = rowOneArea.removeFromLeft( getWidth() * 2 / 3 );
        playStopArea = rowOneArea.removeFromLeft( getWidth() / 3 );

        tempoArea = knobsArea.removeFromLeft( knobsArea.getWidth() / 3 );
        volArea = knobsArea.removeFromLeft( knobsArea.getWidth() / 2);
        posArea = knobsArea;
        
        //row two
        cueArea = rowTwoArea.removeFromLeft( width * 2 / 3 );
        loopButtonArea = rowTwoArea.removeFromLeft( getWidth() / 3 );

        //row three
        loadAndLibButtonArea = rowThreeArea;
    }
    else
    {
        BPMLAbelArea = labelArea.removeFromLeft( width / 3 );
        timerlabelArea = labelArea.removeFromLeft( width / 3 );
        nameLabelArea = labelArea;

        //row one
        knobsArea = rowOneArea.removeFromRight( getWidth() * 2 / 3 );       
        playStopArea = rowOneArea.removeFromRight( getWidth() / 3 );
        
        tempoArea = knobsArea.removeFromRight( knobsArea.getWidth() / 3 );
        volArea = knobsArea.removeFromRight( knobsArea.getWidth() / 2 );
        posArea = knobsArea;
 
        //row two
        cueArea = rowTwoArea.removeFromRight( width * 2 / 3 );
        loopButtonArea = rowTwoArea.removeFromRight( getWidth() / 3);

        //row three
        loadAndLibButtonArea = rowThreeArea;
    }


    ///////////////////////////////////// labels //////////////////////////////////////

    trackNameLabel.setBounds(nameLabelArea);
    BPMLabel.setBounds(BPMLAbelArea);
    timerLabel.setBounds(timerlabelArea);



    ///////////////////////////////////// Knobs /////////////////////////////////////////

    tempoKnob.setBounds(tempoArea.
                        withSizeKeepingCentre(static_cast<int>(tempoArea.getWidth() * 0.9f), 
                                              static_cast<int>(tempoArea.getHeight() * 0.9f)));
    volumeKnob.setBounds(volArea.
                         withSizeKeepingCentre(static_cast<int>(volArea.getWidth() * 0.9f),
                                               static_cast<int>(volArea.getHeight() * 0.9f)));
    positionKnob.setBounds(posArea.
                         withSizeKeepingCentre(static_cast<int>(posArea.getWidth() * 0.9f),
                                               static_cast<int>(posArea.getHeight() * 0.9f)));

    ////////////////////////////////////// Play Stop button ///////////////////////////////
    playStopButton.setBounds(playStopArea.
                         withSizeKeepingCentre(static_cast<int>(playStopArea.getWidth() * 0.7f),
                                               static_cast<int>(playStopArea.getHeight() * 0.7f)));

    ////////////////////////////////////// looping button /////////////////////////////////////
    loopNoLoopButton.setBounds(loopButtonArea.
                         withSizeKeepingCentre(static_cast<int>(loopButtonArea.getWidth() * 0.7f),
                                               static_cast<int>(loopButtonArea.getHeight() * 0.7f)));

    ///////////////////////////////////// cue buttons ////////////////////////////////////////
    cueArea = cueArea.reduced(getWidth() / 70);
    auto removeButton = cueArea.removeFromTop(static_cast<int>(cueArea.getHeight() / 4.0f));
    clearCueButtons.setBounds(removeButton);
    // it is a two row table of cue buttons
    int cueButtonHeight = static_cast<int>(cueArea.getHeight() / 2.0f);
    int cueButtonWidth = static_cast<int>(cueArea.getWidth() / 4.0f);

    for (int row = 0; row < 2; ++row)
    {
        auto rowArea = cueArea.removeFromBottom(cueButtonHeight);
        for (int col = 0; col < 4; ++col)
        {
            int index = row * 4 + col;
            if ( index >= cueButtons.size())
            {
                break;
            }
            auto cueButtonArea = rowArea.removeFromLeft(cueButtonWidth);
            cueButtons[index].setBounds(cueButtonArea);
        }
    }
    

    ///////////////////////////////////// load button ///////////////////////////////////
    loadButton.setBounds(loadAndLibButtonArea.
                          removeFromLeft(
                              static_cast<int>(loadAndLibButtonArea.getWidth() / 2.0f))
                                                                    .reduced(2));

    ///////////////////////////////////// AddToLibrary button ////////////////////////////
    addToLibraryButton.setBounds(loadAndLibButtonArea.reduced(2));

}

void DeckGUI::resetComponentsBeforeLoadingFile()
{
    loadedFile = Utilities::FileStruct{};
    resetCueButtons();
    disableCueButtons();

    addToLibraryButton.setEnabled(false);
    playStopButton.setButtonEnabled(false);
}

void DeckGUI::loadAudioFile(Utilities::FileStruct filedata)
{
    resetComponentsBeforeLoadingFile();

    bool isFileLoaded = player->loadURL(filedata.url);
    if (isFileLoaded)
    {
        File chosenFile = filedata.url.getLocalFile();

        loadedFile = filedata;

        stageNewLoadedFile(chosenFile);
        updateCueButtonsStatus();
    }
}

void DeckGUI::loadAudioFile(File chosenFile)
{
    resetComponentsBeforeLoadingFile();

    URL fileURL = URL{ chosenFile };
    bool isFileLoaded = player->loadURL(fileURL);
    if (isFileLoaded)
    {
        //update the loadedFile metadata
        loadedFile.url = fileURL;
        loadedFile.name = chosenFile.getFileName();
        loadedFile.duration = player->calculateAudioLength();

        stageNewLoadedFile(chosenFile);
    }
}

void DeckGUI::stageNewLoadedFile(juce::File chosenFile)
{
    //set the looping condition of transportSource
    player->setLoopingStatus(loopNoLoopButton.getStatus());

    //setup file on waveformDisplay
    waveformDisplay.loadURL(loadedFile.url);

    //setup file on musicAnalyzer
    musicAnalyzer.analyzeAudio(chosenFile);


    updateAddToLibraryButton();

    //update knob properties
    tempoKnob.setKnobValue(1.0);
    if (loadedFile.duration > 0)
    {
        positionKnob.setRange(0, loadedFile.duration);
    }

    //update buttons
    playStopButton.setButtonEnabled(true);
    playStopButton.setFirstMode(true);

    //update labels
    trackNameLabel.setText(loadedFile.name, dontSendNotification);
    setBPMLabel();

    //enable cueButtons
    for (auto& cueButton : cueButtons)
    {
        cueButton.setEnableButtons(true);
    }
}

void DeckGUI::updateAddToLibraryButton()
{
    if (loadedFile.url != juce::URL{} && playlistComponent->isURLUnique(loadedFile.url))
    {
		addToLibraryButton.setEnabled(true);
	}
    else
    {
        addToLibraryButton.setEnabled(false);
    }
}

void DeckGUI::setBPMLabel()
{
    int adjustedBPM = static_cast<int>( musicAnalyzer.getLiveBPM() * BPMRelativeRate );
    String newBPM = String( adjustedBPM ) + " BPM";
    
    BPMLabel.setText(newBPM, dontSendNotification);
    
}

String DeckGUI::currentTime2String() const
{
    String time = String(currentTime) + " / " + String(loadedFile.duration);
    return time;
}

void DeckGUI::selectFile()
{
    fChooser= std::make_unique<FileChooser>("Please select the file you want to load...");
    auto folderChooserFlags = juce::FileBrowserComponent::openMode 
                              | FileBrowserComponent::canSelectFiles;
    fChooser->launchAsync(folderChooserFlags, [this](const FileChooser& chooser)
        {
            File chosenFile = chooser.getResult();
            if (chosenFile.existsAsFile())
            {
                 loadAudioFile(chosenFile);
            }
        });
}

void DeckGUI::setCueButtonColourData(int cueIndex, juce::Colour colour)
{
    loadedFile.cueStructs[cueIndex].colour = colour;
}

void DeckGUI::setCueButtonNameData(int cueIndex, juce::String name)
{
    loadedFile.cueStructs[cueIndex].name = name;
}
void DeckGUI::setCueButtonTimeData( int cueIndex, double time)
{
    loadedFile.cueStructs[cueIndex].time = time;
}

void DeckGUI::resetCueButtons()
{
    for (int i = 0; i <  cueButtons.size() ; ++i)
    {
        cueButtons[i].setCueButtonColour(juce::Colours::transparentBlack);
        cueButtons[i].setCueButtonName("");
        cueButtons[i].setMarkedTime(-1.0);
        loadedFile.cueStructs[i].time = -1;
        loadedFile.cueStructs[i].name = "";
        loadedFile.cueStructs[i].colour = juce::Colours::transparentBlack;
        cueButtons[i].toggleEnablingStatusOfChildButtons(false);
    }
}
void DeckGUI::disableCueButtons()
{
    for (int i = 0; i < cueButtons.size(); ++i)
    {
        cueButtons[i].setEnableButtons(false);
    }
}

void DeckGUI::updateCueButtonsStatus()
{
    for (int i = 0; i < cueButtons.size(); ++i)
    {
        cueButtons[i].setCueButtonColour(loadedFile.cueStructs[i].colour);
        cueButtons[i].setCueButtonName(loadedFile.cueStructs[i].name);
        cueButtons[i].setMarkedTime(loadedFile.cueStructs[i].time);
        cueButtons[i].setEnableButtons(true);
    }
}


////////////////////////////////////////////////////////// call backs ///////////////////
/** implement Button::Listener */
void DeckGUI::buttonClicked(Button* button)
{
    //play and stop button
    if (static_cast<const juce::Button*>(button) == playStopButton.getButtonPointer())
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
    //loop and no loop
    else if (static_cast<const juce::Button*>(button) == loopNoLoopButton.getButtonPointer())
    {
        player->toggleLooping();
    }
    //Load file
    else if (button == &loadButton)
    {

        selectFile();

    }
    //addToLibraryButton button
    else if (button == &addToLibraryButton)
    {
        if (loadedFile.duration != 0.0)
        {
            playlistComponent->addTrackToLibrary(loadedFile);
            addToLibraryButton.setEnabled(false);
        }
    }
    else if (button == &clearCueButtons)
    {
        resetCueButtons();
    }
    //check cuebuttons
    else
    {
        for (auto& cueButton : cueButtons)
        {
            if (static_cast<const juce::Button*>(button) == cueButton.getButtonPointer())
            {
                player->setPosition(cueButton.getMarkedTime());
            }
        }
    }
}

/** implement Slider::Listener */
void DeckGUI::sliderValueChanged(Slider* slider)
{
    //volume slider
    if (static_cast<juce::Slider*>(slider) == volumeKnob.getSliderPointer())
    {
        player->setGain(slider->getValue());
    }
    //tempo slider
    else if (static_cast<juce::Slider*>(slider) == tempoKnob.getSliderPointer())
    {
        double tempoRelativeRate = slider->getValue();
        player->setSpeed(tempoRelativeRate);
        BPMRelativeRate = tempoRelativeRate;
    }
    //position slider
    else if (static_cast<juce::Slider*>(slider) == positionKnob.getSliderPointer())
    {
        player->setPosition(slider->getValue());
    }
}

void DeckGUI::filesDropped(const StringArray& files, int, int)
{
    if (files.size() == 1)
    {
        auto chosenFile = File{ files[0] };
        loadAudioFile(chosenFile);
    }
}
bool DeckGUI::isInterestedInFileDrag(const StringArray&)
{
    return true;
}

void DeckGUI::timerCallback()
{
    if (player != nullptr)
    {
        currentTime = player->getCurrentPosition();
        // if the looping is off set the play head to the start position and 
        // update the playStop button
        if (player->getPostionRelative() >= .999 && !(player->isPlaying()))
        {
            player->setPosition(0);
            playStopButton.setFirstMode(true);
            player->stop();
        }

        waveformDisplay.setPlayHeadPosition(player->getPostionRelative());
    }
    else
    {
        currentTime = 0;
    }

    musicAnalyzer.setLiveTime(static_cast<float>(currentTime));

    timerLabel.setText(currentTime2String(), dontSendNotification);
    setBPMLabel();
}