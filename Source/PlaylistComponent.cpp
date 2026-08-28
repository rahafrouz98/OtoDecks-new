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

    /**reads the data from the playlist file and load on the tracks vector*/
    loadPlayListData();
  
    tableComponent.getHeader().addColumn("X", 1, 100 );
    tableComponent.getHeader().addColumn("Track Title", 2, 400);
    tableComponent.getHeader().addColumn("Duration", 3, 400);
    tableComponent.getHeader().addColumn("Left Deck", 4, 100);
    tableComponent.getHeader().addColumn("Right Deck",5, 100 );
    
    tableComponent.setModel(this);

    addAndMakeVisible(tableComponent);
    tableComponent.updateContent();
}

PlaylistComponent::~PlaylistComponent()
{
    /**writes the data from the tracks vector on a json file*/
    writePlayListData();
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

void PlaylistComponent::addTrackToLibrary(Utilities::FileStruct loadedFile)
{
    tracks.push_back(loadedFile);
    tableComponent.updateContent();
}

bool PlaylistComponent::isURLUnique(juce::URL url)
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

void PlaylistComponent::setLoadDeck1Callback(std::function<void(URL, Utilities::FileStruct)> callback)
{
    loadDeck1 = callback;
}

void PlaylistComponent::setLoadDeck2Callback(std::function<void(URL, Utilities::FileStruct)> callback)
{
    loadDeck2 = callback;
}
void PlaylistComponent::setDeleteCallback(std::function<void()> callback)
{
    deleteCallback = callback;
}

void PlaylistComponent::writePlayListData()
{
    /**Each item of this array represent a FileStruct which is converted to an juce::DynamicObject.
    Each of these items have another juce::array for representing the cueButtons for each track*/
    juce::Array<juce::var> playlistData;
    for (int i = 0; i < tracks.size(); ++i)
    {
        juce::DynamicObject* trackObject = new juce::DynamicObject();

        trackObject->setProperty("url", tracks[i].url.toString(false));
        trackObject->setProperty("name", tracks[i].name);
        trackObject->setProperty("duration", tracks[i].duration);

        juce::Array<juce::var> cueList;
        for (const auto& cue : tracks[i].cueStructs)
        {
            juce::DynamicObject* cueObject = new juce::DynamicObject();
            cueObject->setProperty("colour", cue.colour.toString());
            cueObject->setProperty("name", cue.name);
            cueObject->setProperty("time", cue.time);
            cueList.add(cueObject);
        }

        trackObject->setProperty("cuelist", cueList);
       
        playlistData.add(trackObject);
    }

    /**gets a juce:var from the dynamic array */
    const juce::var dataVar{ playlistData };

    //writes the data to a json file called sampleData
    Utilities::writeJsonData("playlist", dataVar);

}

void PlaylistComponent::loadPlayListData()
{
    /**read the json file called sampleData and return the content of file in the json format*/
    juce::var varPlaylist = Utilities::loadJsonData("playlist");

    if (varPlaylist.isArray())
    {
        /**gets an array of  juce::var from the juce::var*/
        juce::Array<juce::var>* arrayPlayList = varPlaylist.getArray();

        for (int i = 0; i < arrayPlayList->size(); ++i)
        {
            /**creates a dynamic object  */
            juce::DynamicObject* trackObject = new juce::DynamicObject();
            trackObject = (*arrayPlayList)[i].getDynamicObject();
            Utilities::FileStruct tempFileStruct;

            tempFileStruct.name = trackObject->getProperty("name");
            tempFileStruct.url = juce::URL{ trackObject->getProperty("url") };
            tempFileStruct.duration = trackObject->getProperty("duration");

            juce::var varCueList = trackObject->getProperty("cuelist");

            juce::Array<juce::var>* arrayCueList = varCueList.getArray();
            /**extract the nested array */
            for (int j = 0; j < arrayCueList->size(); ++j)
            {
                juce::DynamicObject* cueObject = new juce::DynamicObject();
                cueObject = (*arrayCueList)[j].getDynamicObject();
                tempFileStruct.cueStructs[j].colour = juce::Colour::fromString(cueObject->getProperty("colour").toString());
                tempFileStruct.cueStructs[j].name = cueObject->getProperty("name");
                tempFileStruct.cueStructs[j].time = cueObject->getProperty("time");
            }
            tracks.push_back(tempFileStruct);
        }
    }
}
