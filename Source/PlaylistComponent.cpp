/*
  ==============================================================================

    playlistComponent.cpp
    Created: 20 Jul 2026 2:11:28pm
    Author:  hraha

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
   

    tableComponent.getHeader().addColumn("X", 1, 100 );
    tableComponent.getHeader().addColumn("Track Title", 2, 400);
    tableComponent.getHeader().addColumn("Duration", 3, 400);
    tableComponent.getHeader().addColumn("Left Deck", 4, 100);
    tableComponent.getHeader().addColumn("Right Deck",5, 100 );
    
    tableComponent.setModel(this);

    addAndMakeVisible(tableComponent);

}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    //g.setColour(juce::Colours::grey);
    //g.drawRect(getLocalBounds(), 1);

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("playlistComponent", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void PlaylistComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    tableComponent.setBounds(0, 0, getWidth(), getHeight());
    tableComponent.getHeader().setColumnWidth(1, getWidth() *2 / 32);
    tableComponent.getHeader().setColumnWidth(2, getWidth() * 12/ 32 );
    tableComponent.getHeader().setColumnWidth(3, getWidth() * 6/32 );
    tableComponent.getHeader().setColumnWidth(4, getWidth() * 6 /32);
    tableComponent.getHeader().setColumnWidth(5, getWidth() * 6 /32);
}

int PlaylistComponent::getNumRows()
{
    return static_cast<int>(tracks.size());
}
void PlaylistComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(Colours::orange);
    }
    else
    {
        g.fillAll(Colours::darkgrey);
    }
}
void PlaylistComponent::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (columnId == 2)
    {
        g.drawText(tracks[rowNumber].name, 2, 0, width - 4, height, Justification::centredLeft, true);
    }
    else if (columnId == 3)
    {
        g.drawText(String(tracks[rowNumber].duration), 2, 0, width - 4, height, Justification::centredLeft, true);
    }
}
Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)
{
    if (existingComponentToUpdate == nullptr)
    {
        if (columnId == 4 || columnId == 5)
        {
            String side = "";
            if (columnId == 4)
            {
                side = "deck1_";
            }
            else
            {
                side = "deck2_";
            }
            juce::TextButton* btn = new juce::TextButton("load");
            String id{ side + std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    
        else if (columnId == 1)
        {
            juce::TextButton* btn = new juce::TextButton("X");
            String id{ "delete_" + std::to_string(rowNumber)};
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(juce::Button* button)
{
    String id = button->getComponentID();
    StringArray tokens;
    tokens.addTokens(id, "_", "");
    int index = tokens[1].getIntValue();
    URL url{ tracks[index].url };
    if (tokens[0] == "deck1")
    {
        loadDeck1(url, tracks[index]);
    }
    else if (tokens[0] == "deck2")
    {
        loadDeck2(url, tracks[index]);
    }
    else if (tokens[0] == "delete")
    {
        tracks.erase(tracks.begin() + index);
        tableComponent.updateContent();
        deleteCallback();
    }
}

void PlaylistComponent::addTrackToLibrary(FileStruct loadedFile)
{
    tracks.push_back(loadedFile);
    tableComponent.updateContent();
}

bool PlaylistComponent::isURLUnique(std::string url)
{
    auto it = std::find_if(tracks.begin(), tracks.end(), [url](auto track) {return track.url == url;});
    if (it == tracks.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void PlaylistComponent::setLoadDeck1Callback(std::function<void(URL, FileStruct)> callback)
{
    loadDeck1 = callback;
}

void PlaylistComponent::setLoadDeck2Callback(std::function<void(URL, FileStruct)> callback)
{
    loadDeck2 = callback;
}
void PlaylistComponent::setDeleteCallback(std::function<void()> callback)
{
    deleteCallback = callback;
}