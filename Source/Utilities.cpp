
#include "Utilities.h"


juce::var Utilities::loadJsonData(juce::String sourceChildDirectory, juce::String fileName)
{
    //select folder and file
    juce::File mainDirectory = juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory();
    juce::File sampleDir = mainDirectory.getChildFile(sourceChildDirectory);
 
    juce::File file = sampleDir.getChildFile( fileName + ".json" );

    if (file.existsAsFile())
    {
        juce::String content  = file.loadFileAsString();
        juce::var parsedJSON = juce::JSON::parse(content);
        return parsedJSON;
    }

    return juce::var();
}

void Utilities::writeJsonData(juce::String desChildDirectory, juce::String fileName, juce::var dataVar)
{
    //select folder and file
    juce::File mainDirectory = juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory();
    juce::File sampleDir = mainDirectory.getChildFile(desChildDirectory);
    if (!sampleDir.exists())
    {
        sampleDir.createDirectory();
    }

    juce::File file = sampleDir.getChildFile(fileName + ".json");

    FileOutputStream stream{ file };

    stream.setPosition(0);
    stream.truncate();
    juce::JSON::writeToStream(stream, dataVar, juce::JSON::FormatOptions().withIndentLevel(2));

    stream.flush();
}