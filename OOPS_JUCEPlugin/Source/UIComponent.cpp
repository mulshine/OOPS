/*
  ==============================================================================

    UIComponent.c
    Created: 18 Jan 2017 11:19:15am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "UIComponent.h"

#include "OOPSLink.h"

UIComponent::UIComponent()

{
    for (int i = 0; i < cSliderNames.size(); i++)
    {
        
        sliders.set(i,  new Slider());
        sliders[i]->setSliderStyle(Slider::SliderStyle::LinearBarVertical);
        sliders[i]->setRange(0.0, 1.0, 1.0/50000.0);
        sliders[i]->setName(cSliderNames[i]);
        sliders[i]->setComponentID(String(i));
        sliders[i]->addListener(this);
        addAndMakeVisible(sliders[i]);
        
        sliderLabels.set(i,   new Label());
        sliderLabels[i]->setName(cSliderNames[i]);
        sliderLabels[i]->setColour(Label::textColourId, Colours::aliceblue);
        sliderLabels[i]->setText(cSliderNames[i], NotificationType::dontSendNotification);
        addAndMakeVisible(sliderLabels[i]);
        
        textFields.set(i, new TextEditor());
        textFields[i]->addListener(this);
        textFields[i]->setName(cSliderNames[i]);
        addAndMakeVisible(textFields[i]);
    }
    
    for (int i = 0; i < cButtonNames.size(); i++)
    {
        buttons.set(i, new TextButton(cButtonNames[i]));
        buttons[i]->changeWidthToFitText();
        buttons[i]->setButtonText(cButtonNames[i]);
        buttons[i]->addListener(this);
        addAndMakeVisible(buttons[i]);
    }
    
    for (int i = 0; i < cComboBoxNames.size(); i++)
    {
        comboBoxes.set(i, new ComboBox());
        comboBoxes[i]->setName(cComboBoxNames[i]);
        comboBoxes[i]->addSeparator();
        comboBoxes[i]->addListener(this);
        addAndMakeVisible(comboBoxes[i]);
        
    }
    
    for (int i = 0; i < cComboBoxNames.size(); i++)
    {
        for (int j = 0; j < cWaveformTypes.size(); j++)
        {
            comboBoxes[i]->addItem(cWaveformTypes[j], j+1);
        }
        comboBoxes[i]->setSelectedItemIndex(0);
    }
    
    
    startTimerHz(20);
}

void UIComponent::timerCallback(void)
{
    for (int i = 0; i < cSliderNames.size(); i++)
    {
        if (!textFields[i]->isTextInputActive()) textFields[i]->setText(String(cSliderModelValues[i]), false);
    }
}

void UIComponent::textEditorReturnKeyPressed (TextEditor& tf)
{
    for (auto slider : sliders)
    {
        if (slider->getName() == tf.getName())
        {
            slider->setValue(tf.getText().getDoubleValue());
        }
    }
}

UIComponent::~UIComponent()
{
}

void UIComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
     draws some placeholder text to get you started.
     
     You should replace everything in this method with your own
     drawing code..
     */
    
    g.fillAll (Colours::slategrey);   // clear the background
    
    g.setColour (Colours::aliceblue);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void UIComponent::resized()
{
    for (int i = 0; i < cSliderNames.size(); i++)
    {
        float width = getWidth() / 8.0f;
        float height = getHeight();
        sliders[i]      ->setBounds(width * i,
                                    cTopOffset,
                                    width - 3.0f,
                                    height * 0.9f);
        /*
        textFields[i]   ->setBounds(cLeftOffset + cXSpacing * i,
                                    sliders[i]->getBottom() + cYSpacing,
                                    cXSpacing - 5,
                                    20);
         */
        
        sliderLabels[i] ->setBounds(width * i,
                                    sliders[i]->getBottom() + cYSpacing,
                                    width - 3.0f,
                                    height * 0.1f);
    }
    
    for (int i = 0; i < cButtonNames.size(); i++)
    {
        buttons[i]      ->setBounds(cLeftOffset + ((cButtonWidth + cXSpacing) * i),
                                    500,
                                    cButtonWidth,
                                    cButtonHeight);
    }
    
    for (int i = 0; i < cComboBoxNames.size(); i++)
    {
        comboBoxes[i]   ->setBounds(
                                    cLeftOffset + ((cButtonWidth + cXSpacing) * i),
                                    cTopOffset + cSliderHeight + cLabelHeight + cButtonHeight + 3 * cYSpacing,
                                    cBoxWidth,
                                    cBoxHeight    );
    }
}

void UIComponent::sliderValueChanged(Slider* s)
{
    setSliderValue(s->getName(), s->getValue());
}

void UIComponent::buttonClicked (Button* b)
{
    setButtonState(b->getName(), true);
}

void UIComponent::comboBoxChanged (ComboBox* cb)
{
    setComboBoxState(cb->getName(), cb->getSelectedId() - 1);
}
