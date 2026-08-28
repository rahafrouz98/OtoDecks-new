#pragma once
#include "../JuceLibraryCode/JuceHeader.h"


class Utilities
{
    public:
        /**this Struct is used to collect the data of cueBurrons (Coloue, name, markedtime)*/
        struct CueStruct {
            juce::Colour colour = juce::Colours::transparentBlack;
            double time = -1.0;
            juce::String name = "";
        };
        /**this is a Struct to hold the metadata of audio and information about its cue buttons
        1-name
        2-duration
        3-url
        4-cueStructs
                    |
                    -coulour
                    -marked time
                    -name
        */
        struct FileStruct {
            juce::String name ="File not loaded";
            double duration = 0.0;
            juce::URL url = juce::URL{};
            juce::String comment = "";
            std::array<CueStruct, 8> cueStructs;
        };
        /**this is the Struct used to hold the data of each loop sample(URL, name)*/
        struct SampleStruct {
            juce::URL url{};
            juce::String name = "";
        };
        /**takes the directory name which is in the same folder as execution file is located, takes file name and 
        returns a juce::var representing JSON data*/
        static juce::var loadJsonData(juce::String fileName);

        /**takes takes the directory name which is in the same folder as execution file is located, takes file name,
        takes data in the juce::var format and wirtes it in the file in JSON format*/
        static void writeJsonData(juce::String fileName, juce::var dataVar);

        //this is the directory where execution file is located
        inline static const juce::File mainDirectory = juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory();
        
        /**this is the direcetory called data inside the mainDirectory to keep the samples and meta data */
        inline static const juce::File desChildDirectory = mainDirectory.getChildFile("data");
     
};