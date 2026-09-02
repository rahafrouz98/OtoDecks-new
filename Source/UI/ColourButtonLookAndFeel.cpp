#include <JuceHeader.h>
#include "ColourButtonLookAndFeel.h"

ColourButtonLookAndFeel::ColourButtonLookAndFeel()
{
}

ColourButtonLookAndFeel::~ColourButtonLookAndFeel()
{
}

void ColourButtonLookAndFeel::drawButtonBackground(juce::Graphics& g, 
                                                   juce::Button& button, 
                                                   const juce::Colour& backgroundColour,
                                                   bool isMouseOver, 
                                                   bool isButtonDown)
{
    auto area = button.getLocalBounds();
    
    float alpha = button.isEnabled() ? 1.0f : 0.5f;

    juce::Colour colour = button.findColour(juce::TextButton::ColourIds::buttonColourId, false);

    juce::Colour tunedColour = isMouseOver ? colour.darker() : 
                                             (isButtonDown ? colour.darker().darker() : colour);

    g.setColour(tunedColour.withMultipliedAlpha(alpha));

    g.fillRect(area);
    if (button.getProperties()["isSelected"] == "true")
    {
        g.setColour(juce::Colours::white);
        g.drawRect(area, static_cast<int>(area.getWidth()/20));
    }
}