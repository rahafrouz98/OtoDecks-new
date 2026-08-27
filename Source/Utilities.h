#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

struct CueStruct {
    std::string colour = "00000000";
    double time = -1.0;
    std::string name = "";
};

struct FileStruct {
    std::string name ="File not loaded";
    double duration = 0.0;
    std::string url = "";
    std::array<CueStruct, 8> cueStructs;
};

struct SampleStruct {
    juce::URL url{};
    juce::String name = "";
};

class Utilities
{
    public:
        /**takes the directory name which is in the same folder as execution file is located, takes file name and 
        returns a juce::var representing JSON data*/
        static juce::var loadJsonData(juce::String sourceChildDirectory, juce::String fileName);

        /**takes takes the directory name which is in the same folder as execution file is located, takes file name,
        takes data in the juce::var format and wirtes it in the file in JSON format*/
        static void writeJsonData(juce::String desChildDirectory, juce::String fileName, juce::var dataVar);

};