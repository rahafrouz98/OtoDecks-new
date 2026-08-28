/*
  ==============================================================================

    playlistComponent.h
    Created: 20 Jul 2026 2:11:28pm
    Author:  hraha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Utilities.h"


//==============================================================================
/*
*/
class PlaylistComponent  : public juce::Component, 
                           public juce::TableListBoxModel, 
                           public juce::Button::Listener,
                           public juce::FileDragAndDropTarget,
                           public juce::TextEditor::Listener
{
public:
    PlaylistComponent();
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    int getNumRows()override;
    void paintRowBackground(Graphics&, int rowNumber, int width, int height, bool rowIsSelected)override;
    void paintCell(Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected)override;
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)override;
    void buttonClicked(juce::Button* button) override;

    /**implement FileDragAndDropTarget*/
    void filesDropped(const StringArray& files, int x, int y) override;
    bool isInterestedInFileDrag(const StringArray& files) override;

    /**implement TExtEditor Listener*/
    void textEditorTextChanged(juce::TextEditor& editor) override;

    /**add audio file to the tracks and table by taking the FileStruct as the argument*/
    void addTrackToLibrary(Utilities::FileStruct loadedFile);
    /**takes the url and returns true if no otems in the tracks vector have the same url*/
    bool isURLUnique(juce::URL url);
    /**set the loadDeckRight callback function*/
    void setLoadDeckLeftCallback(std::function<void(URL, Utilities::FileStruct)> callback);
    /**set the loadDeckLeft callback function*/
    void setLoadDeckRightCallback(std::function<void(URL, Utilities::FileStruct)> callback);
    /**set the  deleteCallback function. Is used in the main component to receive notification that 
    the row is deleted and update the decks button*/
    void setDeleteCallback(std::function<void()> callback);
   
private:
    TableListBox tableComponent;

    std::vector<Utilities::FileStruct> tracks;

    juce::AudioFormatManager formatManager;

    /**callback functiuon to load the track from library to deck 1*/
    std::function<void(URL, Utilities::FileStruct)> loadDeckLeft;

    /**callback functiuon to load the track from library to deck 2*/
    std::function<void(URL, Utilities::FileStruct)> loadDeckRight;

    /**callback function to be called when delete buttons are clicked */
    std::function<void()> deleteCallback;

    /**writes the play list data from the vector of tracks to  a json file in a directory called
    samples in the same directory as EXE is located*/
    void writePlayListData();

    /**loads playlist data ,in JSON format, from a directory called playlist located in the same directory as EXE file is located
    and save it in the samplesRecord array*/
    void loadPlayListData();

    /**it is an override for addTrackToLibrary. It takes juce::file as argument and  adds its 
    data to traks and table*/
    void PlaylistComponent::addTrackToLibrary(juce::File file);
   
    /**takes an audio fle url in juce::URL format  and return the duration of the audio*/
    double getAudioDuration(juce::URL url);

    /**itterates throw text editors and updates their ID form the the startIndex argument to the end of the 
    list. It is used to update the textEditor ids after deleting a row from the track. */
    void updateTextEditorsID(int startIndex);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
