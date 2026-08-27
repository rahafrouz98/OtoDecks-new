/*
  ==============================================================================

    playlistComponent.h
    Created: 20 Jul 2026 2:11:28pm
    Author:  hraha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "utilities.h"


//==============================================================================
/*
*/
class PlaylistComponent  : public juce::Component, public TableListBoxModel, public juce::Button::Listener
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
    /**add audio file to the tracks vector*/
    void addTrackToLibrary(FileStruct loadedFile);
    /**takes the url and returns true if no otems in the tracks vector have the same url*/
    bool isURLUnique(std::string url);
    /**set the loadDeck1 callback function*/
    void setLoadDeck1Callback(std::function<void(URL, FileStruct)> callback);
    /**set the loadDeck2 callback function*/
    void setLoadDeck2Callback(std::function<void(URL, FileStruct)> callback);
    /**set the  deleteCallback function*/
    void setDeleteCallback(std::function<void()> callback);
   
private:
    TableListBox tableComponent;
    std::vector<FileStruct> tracks;

    /**callback functiuon to load the track from library to deck 1*/
    std::function<void(URL, FileStruct)> loadDeck1;
    /**callback functiuon to load the track from library to deck 2*/
    std::function<void(URL, FileStruct)> loadDeck2;
    /**callback function to be called when delete buttons are clicked */
    std::function<void()> deleteCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
