#include <JuceHeader.h>
#include "CueButton.h"

CueButton::CueButton()
{
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

   //opens a CueEditForm to change the colour of the button or its name
    editButton.onClick = 
        [this]() 
        {
            juce::Colour mainButtonColour = mainButton.findColour(juce::TextButton::ColourIds::buttonColourId);
            //creates a unique pointer to and editform
             std::unique_ptr<CueEditForm> editForm = 
                 std::make_unique<CueEditForm>( mainButtonColour,nameLabel.getText());

            //this raw pointer is created to be captured and used inside the lambda function for save button 
            //as we can not capture the unique pointer in lambda without moving the ownership
            CueEditForm* editFormRawPointer = editForm.get();

            /**sets the saveButton callback to receive the selected colour and name from the editForm*/
            editForm->setSaveButtonCallBack(
                [this, editFormRawPointer](juce::Colour selectedColour, juce::String selectedCueName) 
                {
                    setCueButtonColour(selectedColour);
                    setCueButtonName(selectedCueName);

                    //send data to parent component(DeckGUI)
                    cueButtonEditedCallback(selectedColour, selectedCueName);

                    //close the calloutbox
                    
                    //closes the edit form by finding the parent callout box and calling dismiss()
                    juce::CallOutBox* parentCalloutBox = editFormRawPointer->findParentComponentOfClass<juce::CallOutBox>();
                    if (parentCalloutBox != nullptr)
                    {
                        parentCalloutBox->dismiss();
                    }
                });
            /**creates an asynchronous calloutbox and transfers the ownership of editForm to it */
            juce::CallOutBox::launchAsynchronously(std::move(editForm), editButton.getScreenBounds(), nullptr);
         };

    /////////////////////////////// Add button ////////////////////////////////////////
    addButton.setImages(false, true, true,
        addImage, 0.8f, juce::Colours::transparentBlack,
        addImage, 0.9f, juce::Colours::transparentBlack,
        addImage, 1.0f, juce::Colours::transparentBlack);


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
    nameLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
    addAndMakeVisible(nameLabel);
    
    ////////////////////////////////Main button //////////////////////////////////////////
    addAndMakeVisible(mainButton);


}

CueButton::~CueButton()
{
}

void CueButton::paint (juce::Graphics& g)
{
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
        addButton.setBounds(area.withSizeKeepingCentre(gridSize*2, gridSize*2));
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

void CueButton::setCueButtonEditedCallback(std::function<void(juce::Colour, juce::String)> callback)
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
        addButton.setVisible(false);

    }
    else
    {
        editButton.setEnabled(false);
        removeButton.setEnabled(false);
        addButton.setEnabled(true);
        addButton.setVisible(true);

    }
    //resized is needed to update the visibility and positioning of addButton
    resized();
}

const juce::Button* CueButton::getButtonPointer()const
{
    return &mainButton;
}

void CueButton::addListener(juce::Button::Listener* listener)
{
    mainButton.addListener(listener);
}