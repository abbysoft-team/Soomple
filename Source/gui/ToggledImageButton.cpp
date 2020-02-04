/*
  ==============================================================================

    ToggledImageButton.cpp
    Created: 17 Jul 2019 3:05:13pm
    Author:  apopov

  ==============================================================================
*/

#include "ToggledImageButton.h"
#include "../Settings.h"


ToggledImageButton::ToggledImageButton(juce::String buttonName) : ImageButton(buttonName), toggled(false) {
}

bool ToggledImageButton::isToggled() const {
    return toggled;
}

void ToggledImageButton::paintButton (Graphics& g,
                                       bool shouldDrawButtonAsHighlighted,
                                       bool shouldDrawButtonAsDown) {
    ImageButton::paintButton(g, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    
    if (toggled) {
        g.setColour(Settings::TOGGLED_BUTTON_MASK_COLOR);
        g.fillRect(Rectangle<int>(0, 0, this->getWidth(), this->getHeight()));
    }
}

void ToggledImageButton::mouseDown(const juce::MouseEvent &event) {
    ImageButton::Component::mouseDown(event);
    toggled = !toggled;
}

void ToggledImageButton::mouseMove(const MouseEvent &event)
{
    ImageButton::Component::mouseMove(event);

    this->setMouseCursor(MouseCursor::PointingHandCursor);
}

void ToggledImageButton::setToggled(bool toggled) {
    this->toggled = toggled;
}






