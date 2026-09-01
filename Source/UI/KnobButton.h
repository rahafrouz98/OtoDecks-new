#pragma once

#include <JuceHeader.h>
#include "KnobLookAndFeel.h"
#include <regex>

class KnobButton  : public juce::Component
{
public:
    KnobButton(juce::String title = "", double min = 0.0, double max = 1.0 );
    ~KnobButton() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    /**set the minimum and maximum of the knob*/
    void setRange(double _min, double _max);

    /**return a const pointer of the slider */
    const juce::Slider* getSliderPointer() const;

    /**add listener to the slider*/
    void addListener(juce::Slider::Listener*);

    /**set the value of knob*/
    void setKnobValue(double value);

private:
    KnobLookAndFeel knobLookandFeel;

    juce::GroupComponent groupComponent;
    juce::Slider knob;
    juce::TextEditor textEditor;

    double min;
    double max;

    /**Text is starting with a one to eleven digits on the left side of decimal, 
    and with maximum 3 digits on the right side*/
    std::regex finalPattern{ R"(^[0-9]{1,11}(?:\.{0,1}[0-9]{0,3})?$)" };

    /**Text is starting with a one to eleven digits on the left side of decimal,
    and with maximum 3 digits on the right side. It can have  no charechter, or
    it can accept numbers with no digit on the right side of decimal point*/

    std::regex editingPattern{ R"(^[0-9]{0,11}(?:\.{0,1}[0-9]{0,3})?$)" };

    /**returns true if the final value of textEditor (after clicking return key) is valid*/
    bool isfinalInputValid() const;

    bool isEditingInputValid() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KnobButton)
};
