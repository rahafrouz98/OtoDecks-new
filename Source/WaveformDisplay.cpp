

#include <JuceHeader.h>
#include "WaveformDisplay.h"


WaveformDisplay::WaveformDisplay(AudioFormatManager& formatManagerToUse, 
                                 AudioThumbnailCache& cacheToUse):
	                             audioThumb(1000, formatManagerToUse, cacheToUse)
                  
{
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   

    g.fillAll(juce::Colours::black);
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   


    if (fileLoaded)
    {
        g.setColour (juce::Colour(88, 211, 255));
        audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength(), 0, 1.0f);

        //draw a position marker
        g.setColour(juce::Colour(183, 109, 255));
        g.drawLine(playHeadPosition, 0.0f, playHeadPosition, static_cast<float>(getHeight()), 2.0f);

        Path topArrow;
        topArrow.startNewSubPath(playHeadPosition, getHeight() / 15.0f);
        topArrow.lineTo(playHeadPosition + getWidth() / 50.0f, 0.0f);
        topArrow.lineTo(playHeadPosition  - getWidth() / 50.0f, 0.0f);
        topArrow.closeSubPath();
        g.fillPath(topArrow);

        Path bottomArrow;
        bottomArrow.startNewSubPath(playHeadPosition , getHeight()- getHeight() / 15.0f);
        bottomArrow.lineTo(playHeadPosition + getWidth() / 50.0f, static_cast<float>(getHeight()));
        bottomArrow.lineTo(playHeadPosition - getWidth() / 50.0f, static_cast<float>(getHeight()));
        bottomArrow.closeSubPath();
        g.fillPath(bottomArrow);

        //print mouse location
        if (isMouseOver(true) && mouseClickCallback!= nullptr)
        {
            g.setFont(juce::FontOptions(15.0f));

            g.setColour(juce::Colour(170, 90, 245));

            String timeStamp = String(getMouseX2TimeInString());

            if (mouseY < getHeight() / 2.0f && mouseX < getWidth() / 2.0f)
            {
                g.setColour(Colours::grey);

                g.fillRect(static_cast<int>(mouseX) + 10, 
                           static_cast<int>(mouseY), 
                           80,
                           20);

                g.setColour(Colours::blue);

                g.drawText(timeStamp, static_cast<int>(mouseX) + 10,
                                      static_cast<int>(mouseY), 
                                      80, 
                                      20, 
                                     Justification::left, 
                                     true);
            }
            else if (mouseY < getHeight() / 2.0f && mouseX > getWidth() / 2.0f)
            {
                g.setColour(Colours::grey);
                g.fillRect(static_cast<int>(mouseX) - 80, 
                           static_cast<int>(mouseY), 
                           80, 
                           20);

                g.setColour( Colours::blue );
                g.drawText( timeStamp,
                            static_cast<int>(mouseX) - 90, 
                            static_cast<int>(mouseY), 
                            80, 
                            20, 
                            Justification::right, 
                            true);
            }
            else if (mouseY > getHeight() / 2.0f && mouseX > getWidth() / 2.0f)
            {
                g.setColour(Colours::grey);
                g.fillRect(static_cast<int>(mouseX) - 80, 
                           static_cast<int>(mouseY)-20,
                           80, 
                           20);
                g.setColour(Colours::blue);
                g.drawText(timeStamp,
                           static_cast<int>(mouseX) - 90,
                           static_cast<int>(mouseY)-20, 
                           80, 
                           20, 
                           Justification::right, 
                           true);
            }
            else //mouseY > getHeight() / 2.0f && mouseX < getWidth() / 2.0f
            {
                g.setColour(Colours::grey);
                g.fillRect(static_cast<int>(mouseX) + 10, static_cast<int>(mouseY) - 20, 80, 20);
                g.setColour(Colours::blue);
                g.drawText(timeStamp, 
                          static_cast<int>(mouseX) + 10, 
                          static_cast<int>(mouseY) - 20, 
                          80, 
                          20, 
                          Justification::left,
                          true);
            }
        }
    }

    else
    {
        g.setFont(juce::FontOptions(20.0f));
        if (isRecording)
        {
            g.drawText("IT IS RECORDING ...", getLocalBounds(),
                juce::Justification::centred, true);
        }
        else
        {
            g.drawText ("NO AUDIO ...", getLocalBounds(),
                        juce::Justification::centred, true);  
        }
    }


}

void WaveformDisplay::resized()
{

}

void WaveformDisplay::loadURL(URL audioURL)
{
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new URLInputSource(audioURL));
}
void WaveformDisplay::unloadURL()
{
    audioThumb.clear();
    fileLoaded = false;
}

void WaveformDisplay::changeListenerCallback(ChangeBroadcaster*)
{
    repaint();
}

void WaveformDisplay::setPlayHeadPosition(float relativePos)
{
    if (std::isfinite(relativePos))
    {
        playHeadPosition = relativePos * getWidth();
        repaint();
    }
}

void WaveformDisplay::mouseDown(const MouseEvent& event)
{
    playHeadPosition = event.position.x;
    if (mouseClickCallback != nullptr)
    {
        mouseClickCallback(getPositionRelative());
    }
}

void WaveformDisplay::setMouseClickCallback(std::function<void(float)> callback)
{
    mouseClickCallback = callback;
}

float WaveformDisplay::getPositionRelative()
{
    return playHeadPosition / float(getWidth());
}

void WaveformDisplay::mouseMove(const MouseEvent& event)
{
    mouseX = event.position.x;
    mouseY = event.position.y;
    repaint();
}

String WaveformDisplay::getMouseX2TimeInString() const
{
    double mouseX2Time = audioThumb.getTotalLength()*mouseX/getWidth();
    String time = String(mouseX2Time);
    return time;
}

void WaveformDisplay::setIsRecording(bool status)
{
    isRecording = status;
}