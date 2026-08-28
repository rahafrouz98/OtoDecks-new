
#include "Utilities.h"


juce::var Utilities::loadJsonData(juce::String fileName)
{

    juce::File file = desChildDirectory.getChildFile( fileName + ".json" );
    DBG("File Path: " << file.getFullPathName());
    juce::FileInputStream stream{ file };
    if (stream.openedOk())
    {
        stream.setPosition(0);

        juce::var parsedJson = juce::JSON::parse(stream);

        return parsedJson;
    }

    return juce::var();
}

void Utilities::writeJsonData(juce::String fileName, juce::var dataVar)
{
    //this check is here so it clreates the folder if it does not exist for the first time
    if (!desChildDirectory.exists())
    {
        desChildDirectory.createDirectory();
    }

    juce::File file = desChildDirectory.getChildFile(fileName + ".json");

    juce::FileOutputStream stream{ file };
    
    if (stream.openedOk())
    {
        stream.setPosition(0);
        stream.truncate();
        juce::JSON::writeToStream(stream, dataVar, juce::JSON::FormatOptions().withIndentLevel(2));
    }
    stream.flush();
}