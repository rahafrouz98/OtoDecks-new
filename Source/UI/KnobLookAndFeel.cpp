#include <JuceHeader.h>
#include "KnobLookAndFeel.h"

KnobLookAndFeel::KnobLookAndFeel()
{
    /**Customize the juce::Slider to draw a rotary slider in the shape of */
    setColour(juce::Slider::thumbColourId, juce::Colours::red);
}
KnobLookAndFeel::~KnobLookAndFeel()
{ }

void KnobLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                       int x, 
                                       int y, 
                                       int width, 
                                       int height, 
                                       float sliderPos, 
                                       const float rotaryStartAngle, 
                                       const float rotaryEndAngle,
                                       juce::Slider& slider)
{ 
    
    float offsetX = x + width / 10.0f;
    float offsety = y + height / 10.0f;
    float ellipseWidth = 8 * width / 10.0f;
    float ellipseHeight = 8 * height / 10.0f;
    // fill

    g.setColour(juce::Colour(183,109,255));
    g.fillEllipse(offsetX, offsety, ellipseWidth, ellipseHeight);

    float outlineThinkness = 2.0f;
    float xCenter = ellipseWidth / 2 + offsetX;
    float yCenter = ellipseHeight / 2 + offsety;
    float radius = xCenter - offsetX;

    // outline
    juce::Path outlinePath;
    g.setColour(juce::Colours::silver);
    outlinePath.addCentredArc(xCenter,
                              yCenter, 
                              radius, 
                              radius, 
                              0.0f, 
                              rotaryStartAngle, 
                              rotaryEndAngle, 
                              true);

    g.strokePath(outlinePath, juce::PathStrokeType(outlineThinkness));
    juce::Path filledOutline;
    outlineThinkness = 4.0f;
    g.setColour(juce::Colours::midnightblue);

    float filledAngle = rotaryStartAngle + 
                        sliderPos * (rotaryEndAngle - rotaryStartAngle);

    filledOutline.addCentredArc(xCenter,
                                yCenter,
                                radius, 
                                radius, 
                                0.0f, 
                                rotaryStartAngle, 
                                filledAngle, 
                                true);

    g.strokePath(filledOutline, juce::PathStrokeType(outlineThinkness));

    //marker
    juce::Path markerPath;
    g.setColour(juce::Colours::silver);
    float pointerLength = radius * 0.4f;
    float pointerThickness = 2.0f;
    markerPath.addRectangle(0.0,
                            -radius * 0.8f, 
                            pointerThickness, 
                            pointerLength);

    markerPath.applyTransform(juce::AffineTransform::rotation(filledAngle).
                                             translated(xCenter, yCenter));
    g.fillPath(markerPath);
}


