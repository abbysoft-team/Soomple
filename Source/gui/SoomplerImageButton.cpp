/*
  ==============================================================================

    SoomplerImageButton.cpp
    Created: 29 Jul 2019 8:24:44pm
    Author:  artyom

  ==============================================================================
*/

#include "SoomplerImageButton.h"

SoomplerImageButton::SoomplerImageButton(String name) : ImageButton(name)
{

}

void SoomplerImageButton::mouseMove(const MouseEvent &event)
{
    ImageButton::mouseMove(event);

    setMouseCursor(MouseCursor::PointingHandCursor);
}
