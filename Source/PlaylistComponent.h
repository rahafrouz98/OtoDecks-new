#pragma once

#include <JuceHeader.h>
#include "Utilities.h"

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

    /**Implement TableLiatBoxModel. Returns the number of tracks*/
    int getNumRows()override;
    /**Implement TableLiatBoxModel */
    void paintRowBackground(Graphics&, 
                            int rowNumber, 
                            int width, 
                            int height, 
                            bool rowIsSelected)override;
    /**Implement TableLiatBoxModel. draws one of the cells */
    void paintCell( Graphics&, 
                    int rowNumber, 
                    int columnId, 
                    int width, 
                    int height, 
                    bool rowIsSelected)override;
    /**create or update a custom component to go in a cell*/
    Component* refreshComponentForCell(int rowNumber, 
                                       int columnId, 
                                       bool isRowSelected, 
                                       Component* existingComponentToUpdate)override;

    /**Called when the button is clicked*/
    void buttonClicked(juce::Button* button) override;

    /**implement FileDragAndDropTarget*/
    void filesDropped(const StringArray& files, int x, int y) override;
    /**implement FileDragAndDropTarget*/
    bool isInterestedInFileDrag(const StringArray& files) override;

    /**implement TextEditor Listener*/
    void textEditorTextChanged(juce::TextEditor& editor) override;

    /**add audio file to the tracks and table by taking the FileStruct as the argument*/
    void addTrackToLibrary(Utilities::FileStruct loadedFile);
    /**takes the url and returns true if no otems in the tracks vector have the same url*/
    bool isURLUnique(juce::URL url);
    /**set the loadDeckRight callback function*/
    void setLoadDeckLeftCallback(std::function<void(Utilities::FileStruct)> callback);
    /**set the loadDeckLeft callback function*/
    void setLoadDeckRightCallback(std::function<void(Utilities::FileStruct)> callback);
    /**set the  deleteCallback function. Is used in the main component to receive 
    notification that the row is deleted and update the decks button*/
    void setDeleteCallback(std::function<void()> callback);
   
private:
    TableListBox tableComponent;

    std::vector<Utilities::FileStruct> tracks;

    juce::AudioFormatManager formatManager;

    /**callback functiuon to load the track from library to deck 1*/
    std::function<void(Utilities::FileStruct)> loadDeckLeft;

    /**callback functiuon to load the track from library to deck 2*/
    std::function<void(Utilities::FileStruct)> loadDeckRight;

    /**callback function to be called when delete buttons are clicked */
    std::function<void()> deleteCallback;

    /**writes the play list data from the vector of tracks to  a json file in a 
    directory calledsamples in the same directory as EXE is located*/
    void writePlayListData();

    /**loads playlist data ,in JSON format, from a directory called playlist located
    in the same directory as EXE file is located and save it in the samplesRecord array*/
    void loadPlayListData();

    /**it is an override for addTrackToLibrary. It takes juce::file as argument and
    adds its data to traks and table*/
    void PlaylistComponent::addTrackToLibrary(juce::File file);
   
    /**takes an audio fle url in juce::URL format  and return the duration of the audio*/
    double getAudioDuration(juce::URL url);

    /**itterates throw text editors and updates their ID form the the startIndex 
    argument to the end of the list. It is used to update the textEditor ids after 
    deleting a row from the track. */
    void updateTextEditorsID(int startIndex);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
