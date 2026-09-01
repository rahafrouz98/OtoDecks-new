#include <JuceHeader.h>
#include "PlaylistComponent.h"

PlaylistComponent::PlaylistComponent()
{
    loadPlayListData();
  
    ////////////////////////////////// Table component/////////////////////////////////
    tableComponent.getHeader().addColumn("X", 1, 50 );
    tableComponent.getHeader().addColumn("Track Title", 2, 300);
    tableComponent.getHeader().addColumn("Duration", 3, 300);
    tableComponent.getHeader().addColumn("Comments", 4, 400);
    tableComponent.getHeader().addColumn("Left Deck", 5, 100);
    tableComponent.getHeader().addColumn("Right Deck",6, 100 );
    
    tableComponent.setModel(this);

    addAndMakeVisible(tableComponent);
    tableComponent.updateContent();

    /////////////////////////////////// FormatMAnager //////////////////////////////////
    formatManager.registerBasicFormats();
}

PlaylistComponent::~PlaylistComponent()
{
    /**writes the data from the tracks vector on a json file*/
    writePlayListData();
    tableComponent.setModel(nullptr);
}

void PlaylistComponent::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("playlistComponent", getLocalBounds(),
                juce::Justification::centred, true);   
}

void PlaylistComponent::resized()
{
    tableComponent.setBounds(0, 0, getWidth(), getHeight());
    tableComponent.getHeader().setColumnWidth(1, getWidth() * 1 / 32);
    tableComponent.getHeader().setColumnWidth(2, getWidth() * 8 / 32 );
    tableComponent.getHeader().setColumnWidth(3, getWidth() * 3 / 32 );
    tableComponent.getHeader().setColumnWidth(4, getWidth() * 8 / 32);
    tableComponent.getHeader().setColumnWidth(5, getWidth() * 6 / 32);
    tableComponent.getHeader().setColumnWidth(6, getWidth() * 6 / 32);
}

int PlaylistComponent::getNumRows()
{
    return static_cast<int>(tracks.size());
}
void PlaylistComponent::paintRowBackground( Graphics& g, 
                                            int rowNumber, 
                                            int width, 
                                            int height, 
                                            bool rowIsSelected)
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

void PlaylistComponent::paintCell( Graphics& g, 
                                   int rowNumber, 
                                   int columnId, 
                                   int width, 
                                   int height,
                                   bool rowIsSelected)
{
    if (columnId == 2)
    {
        g.drawText(tracks[rowNumber].name, 
                   2, 
                   0,
                   width - 4, 
                   height, 
                   Justification::centredLeft, 
                   true );
    }
    else if (columnId == 3)
    {
        g.drawText(String(tracks[rowNumber].duration), 
                   2, 
                   0, 
                   width - 4, 
                   height, 
                   Justification::centredLeft, true);
    }
}

void PlaylistComponent::addTrackToLibrary(Utilities::FileStruct loadedFile)
{
    tracks.push_back(loadedFile);
    tableComponent.updateContent();
}

void PlaylistComponent::addTrackToLibrary(juce::File file)
{
    Utilities::FileStruct loadedFile;
    loadedFile.url = URL{ file };
    loadedFile.duration = getAudioDuration(loadedFile.url);
    loadedFile.name = file.getFileName();
    
    tracks.push_back(loadedFile);
    tableComponent.updateContent();
}

double PlaylistComponent::getAudioDuration(juce::URL url)
{
    std::unique_ptr<AudioFormatReader> reader;
    auto* rawReader = formatManager.createReaderFor(url.createInputStream(false));
    reader.reset(rawReader);
    if (reader != nullptr)
    {
        if (reader->sampleRate != 0)
        {
             return reader->lengthInSamples / reader->sampleRate;
        }
    }
    return 0;
}

bool PlaylistComponent::isURLUnique(juce::URL url)
{
    auto it = std::find_if(tracks.begin(), 
                           tracks.end(), 
                           [url](auto track) {return track.url == url;});

    if (it == tracks.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void PlaylistComponent::writePlayListData()
{
    /**Each item of this array represent a FileStruct which is converted to an
    juce::DynamicObject.Each of these items have another juce::array for representing 
    the cueButtons for each track*/
    juce::Array<juce::var> playlistData;
    for (int i = 0; i < tracks.size(); ++i)
    {
        juce::DynamicObject* trackObject = new juce::DynamicObject();

        trackObject->setProperty("url", tracks[i].url.toString(false));
        trackObject->setProperty("name", tracks[i].name);
        trackObject->setProperty("duration", tracks[i].duration);
        trackObject->setProperty("comment", tracks[i].comment);

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
            tempFileStruct.comment = trackObject->getProperty("comment");

            juce::var varCueList = trackObject->getProperty("cuelist");

            juce::Array<juce::var>* arrayCueList = varCueList.getArray();
            /**extract the nested array */
            for (int j = 0; j < arrayCueList->size(); ++j)
            {
                juce::DynamicObject* cueObject = new juce::DynamicObject();
                cueObject = (*arrayCueList)[j].getDynamicObject();
                tempFileStruct.cueStructs[j].colour = juce::Colour::
                                                      fromString(cueObject->getProperty("colour").
                                                                 toString());
                tempFileStruct.cueStructs[j].name = cueObject->getProperty("name");
                tempFileStruct.cueStructs[j].time = cueObject->getProperty("time");

            }
            tracks.push_back(tempFileStruct);
        }
    }
}

Component* PlaylistComponent::refreshComponentForCell(int rowNumber, 
                                                      int columnId, 
                                                      bool isRowSelected, 
                                                      Component* existingComponentToUpdate)
{
    if (existingComponentToUpdate == nullptr)
    {
        if (columnId == 5 || columnId == 6)
        {
            String side = "";
            if (columnId == 5)
            {
                side = "deckLeft_";
            }
            else
            {
                side = "deckRight_";
            }
            juce::TextButton* btn = new juce::TextButton("load");
            String id{ side + std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
        else if (columnId == 4)
        {
            juce::TextEditor* textEditor = new juce::TextEditor();
            String id{std::to_string(rowNumber) };
            textEditor->setComponentID(id);
            textEditor->addListener(this);
            textEditor->setText(tracks[rowNumber].comment, false);
            existingComponentToUpdate = textEditor;
        }
        else if (columnId == 1)
        {
            juce::TextButton* btn = new juce::TextButton("X");
            String id{ "delete_" + std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

void PlaylistComponent::updateTextEditorsID(int startIndex)
{
    for (int i = startIndex; i < tableComponent.getNumRows(); i++)
    {
        //dynamicly casted fron Component pointer to a textEditor pointer to 
        // be able to use setText() function
        auto* textEditorTarget = dynamic_cast<juce::TextEditor*>
                                      (tableComponent.getCellComponent(4, i));
        if (textEditorTarget != nullptr)
        {
            textEditorTarget->setComponentID(String(i));
            textEditorTarget->setText(tracks[i].comment);
        }
    }
}

////////////////////////////////////////////////// Call backs ////////////////////////////
void PlaylistComponent::buttonClicked(juce::Button* button)
{
    String id = button->getComponentID();
    StringArray tokens;
    tokens.addTokens(id, "_", "");
    int index = tokens[1].getIntValue();
    if (tokens[0] == "deckLeft")
    {
        loadDeckLeft(tracks[index]);
    }
    else if (tokens[0] == "deckRight")
    {
        loadDeckRight(tracks[index]);
    }
    else if (tokens[0] == "delete")
    {
        tracks.erase(tracks.begin() + index);
        tableComponent.updateContent();
        updateTextEditorsID(index);
        deleteCallback();
    }
}

void PlaylistComponent::textEditorTextChanged(juce::TextEditor& editor)
{
    tracks[editor.getComponentID().getIntValue()].comment = editor.getText();
}

void PlaylistComponent::filesDropped(const StringArray& files, int x, int y)
{
    if (files.size() == 1)
    {
        auto chosenFile = File{ files[0] };
        addTrackToLibrary(chosenFile);
    }
}
bool PlaylistComponent::isInterestedInFileDrag(const StringArray& files)
{
    return true;
}

void PlaylistComponent::setLoadDeckLeftCallback
                        (std::function<void(Utilities::FileStruct)> callback)
{
    loadDeckLeft = callback;
}

void PlaylistComponent::setLoadDeckRightCallback
                        (std::function<void(Utilities::FileStruct)> callback)
{
    loadDeckRight = callback;
}
void PlaylistComponent::setDeleteCallback(std::function<void()> callback)
{
    deleteCallback = callback;
}
