
#pragma once

#include <JuceHeader.h>
/**Customize the juce::Slider to draw a rotary slider in the shape of a knob*/
class KnobLookAndFeel  :  public juce::LookAndFeel_V4
{
public:
    KnobLookAndFeel();
    ~KnobLookAndFeel() override;
    /**Customize the juce::Slider to draw a rotary slider in the shape of a knob */
    void drawRotarySlider(juce::Graphics& g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPos,
                          const float rotaryStartAngle, 
                          const float rotaryEndAngle, 
                          juce::Slider&) override;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KnobLookAndFeel)
};
