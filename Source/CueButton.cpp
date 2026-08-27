/*
  ==============================================================================

    CueButton.cpp
    Created: 21 Aug 2026 8:07:05am
    Author:  hraha

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CueButton.h"

//==============================================================================
CueButton::CueButton()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    setSize(300, 300);

    //////////////////////////////// Images ///////////////////////////////////////
    editImage = ImageCache::getFromMemory(BinaryData::pen_png, BinaryData::pen_pngSize);
    removeImage = ImageCache::getFromMemory(BinaryData::remove_png, BinaryData::remove_pngSize);
    addImage = ImageCache::getFromMemory(BinaryData::add_png, BinaryData::add_pngSize);

    ////////////////////////////////Edit button ///////////////////////////////////////
    editButton.setImages(false, true, true,
        editImage, 0.8f, juce::Colours::transparentBlack,
        editImage, 0.9f, juce::Colours::transparentBlack,
        editImage, 1.0f, juce::Colours::transparentBlack);
    addAndMakeVisible(editButton);

   //opens a CueEditForm form to change the colour of the button or its text
    editButton.onClick = [this]() {
        std::unique_ptr<CueEditForm> editForm = std::make_unique<CueEditForm>(mainButton.findColour(juce::TextButton::ColourIds::buttonColourId)
                                                                                                       , nameLabel.getText());

                //this raw pointer is created to be captured inside lambda
                CueEditForm* editFormRawPointer = editForm.get();

                editForm->setSaveButtonCallBack([this, editFormRawPointer](juce::Colour selectedColour, juce::String selectedCueName) {
                    setCueButtonColour(selectedColour);
                    setCueButtonName(selectedCueName);

                    //send data to parent component(DeckGUI)
                    cueButtonEditedCallback(selectedColour.toString().toStdString(), selectedCueName.toStdString());

                    //close the calloutbox
             
                    juce::CallOutBox* parentCalloutBox = editFormRawPointer->findParentComponentOfClass<juce::CallOutBox>();
                    if (parentCalloutBox != nullptr)
                    {
                        parentCalloutBox->dismiss();
                    }
            });

        
        juce::CallOutBox::launchAsynchronously(std::move(editForm), editButton.getScreenBounds(), nullptr);

       
    };

    /////////////////////////////// Add button ////////////////////////////////////////
    addButton.setImages(false, true, true,
        addImage, 0.8f, juce::Colours::transparentBlack,
        addImage, 0.9f, juce::Colours::transparentBlack,
        addImage, 1.0f, juce::Colours::transparentBlack);

    //give a random colour to the button and send data to DeckGui
    addButton.onClick = [this]() { 
            //this is a callback from parent
            cueButtonAddCallback();
            toggleEnablingStatusOfChildButtons(true);
        };

    addAndMakeVisible(addButton);

    //////////////////////////////// Remove button //////////////////////////////////////
    addAndMakeVisible(removeButton);
    removeButton.setImages(false, true, true,
        removeImage, 0.8f, juce::Colours::transparentBlack,
        removeImage, 0.9f, juce::Colours::transparentBlack,
        removeImage, 1.0f, juce::Colours::transparentBlack);

            removeButton.onClick = [this]() { 
                mainButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::transparentBlack);
                nameLabel.setText("", dontSendNotification);
                //this is a callback from parent
                cueButtonRemoveCallback(); 
                toggleEnablingStatusOfChildButtons(false);
             };

    //////////////////////////////// nameLabel /////////////////////////////////////////////////
    nameLabel.setColour(juce::Label::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
    nameLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    nameLabel.setInterceptsMouseClicks(false, false); //it is needed to prevent overlapping with addButton
    nameLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(nameLabel);
    
    ////////////////////////////////Main button //////////////////////////////////////////
    addAndMakeVisible(mainButton);

    mainButton.onClick = [this]() {("main");};


}

CueButton::~CueButton()
{
}

void CueButton::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

}

void CueButton::resized()
{


    int gridSize = getWidth() / 6;
    int padding = gridSize / 4;

    editButton.setBounds(getWidth() - gridSize- padding, padding, gridSize, gridSize);

    removeButton.setBounds(getWidth() - gridSize * 2 - padding * 2, padding, gridSize, gridSize);

    nameLabel.setBounds(padding * 2, getHeight()/2, getWidth() * 0.8f, getHeight()/3 );

    auto area = getLocalBounds();

    if (addButton.isEnabled())
    {
        addButton.setVisible(true);
        addButton.setBounds(area.withSizeKeepingCentre(gridSize*2, gridSize*2));
    }
    else
    {
        addButton.setVisible(false);
    }
    
    mainButton.setBounds(area);
    mainButton.toBack();
}

void CueButton::setCueButtonColour(juce::Colour selectedColour )
{
   
    mainButton.setColour( juce::TextButton::ColourIds::buttonColourId ,selectedColour);
    
}
void CueButton::setCueButtonName(juce::String name)
{
    nameLabel.setText(name, dontSendNotification);
}

void CueButton::setCueButtonEditedCallback(std::function<void(std::string, std::string)> callback)
{
    cueButtonEditedCallback = callback;
}

void CueButton::setCueButtonAddCallback(std::function<void()> callback)
{
    cueButtonAddCallback = callback;
}

void CueButton::setCueButtonRemoveCallback(std::function<void()> callback)
{
    cueButtonRemoveCallback = callback;
}

void CueButton::setMarkedTime(double time)
{
    markedTime = time;
}


double CueButton::getMarkedTime() const
{
    return markedTime;
}

void CueButton::setEnableButtons(bool statusTarget)
{
    status = statusTarget;

    if (statusTarget)
    {
        if (markedTime != -1.0)
        {
            toggleEnablingStatusOfChildButtons(true);
        }
        else
        {
            toggleEnablingStatusOfChildButtons(false);
        }
    }
    else
    {
        removeButton.setEnabled(false);
        addButton.setEnabled(false);
        editButton.setEnabled(false);
    }
}

void CueButton::toggleEnablingStatusOfChildButtons(bool targetStatus)
{
    if (targetStatus)
    {
        editButton.setEnabled(true);
        removeButton.setEnabled(true);
        addButton.setEnabled(false);

        //resized is needed to update the visibility and positioning of addButton
        resized();
    }
    else
    {
        editButton.setEnabled(false);
        removeButton.setEnabled(false);
        addButton.setEnabled(true);

        //resized is needed to update the visibility and positioning of addButton
        resized();
    }
}

const juce::Button* CueButton::getButtonPointer()const
{
    return &mainButton;
}

void CueButton::addListener(juce::Button::Listener* listener)
{
    mainButton.addListener(listener);
}