/*
  ==============================================================================

    SoomplerToggleButton.cpp
    Created: 29 Jul 2019 8:37:19pm
    Author:  artyom

  ==============================================================================
*/

#include "SoomplerToggleButton.h"

SoomplerToggleButton::SoomplerToggleButton(String buttonName) : ToggleButton(buttonName), toggled(false)
{

}
bool SoomplerToggleButton::isToggle() const
{
    return toggled;
}

void SoomplerToggleButton::setToggled(bool toggled)
{
    this->toggled = toggled;
    ToggleButton::setToggleState(toggled, false);
}

void SoomplerToggleButton::mouseMove(const MouseEvent &event)
{
    ToggleButton::mouseMove(event);

    setMouseCursor(MouseCursor::PointingHandCursor);
}

void SoomplerToggleButton::mouseDown(const MouseEvent &event)
{
    ToggleButton::mouseDown(event);

    toggled = !toggled;
}
