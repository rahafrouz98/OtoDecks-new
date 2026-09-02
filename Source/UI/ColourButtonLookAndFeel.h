#pragma once

#include <JuceHeader.h>

class ColourButtonLookAndFeel  : public juce::LookAndFeel_V4
{
public:
    ColourButtonLookAndFeel();
    ~ColourButtonLookAndFeel() override;

    /* it draws the button with  a white boarder around the button when its property 
    called "isSelected" is true*/
    void ColourButtonLookAndFeel::drawButtonBackground(juce::Graphics& g, 
                                                       juce::Button& button, 
                                                       const juce::Colour& backgroundColour,
                                                       bool isMouseOver, 
                                                       bool isButtonDown)override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourButtonLookAndFeel)
};
