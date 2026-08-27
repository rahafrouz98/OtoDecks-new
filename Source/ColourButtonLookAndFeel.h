/*
  ==============================================================================

    ColourButtonLookAndFeel.h
    Created: 22 Aug 2026 6:45:01pm
    Author:  hraha

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ColourButtonLookAndFeel  : public juce::LookAndFeel_V4
{
public:
    ColourButtonLookAndFeel();
    ~ColourButtonLookAndFeel() override;


    void ColourButtonLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                                                            bool isMouseOver, bool isButtonDown)override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourButtonLookAndFeel)
};
