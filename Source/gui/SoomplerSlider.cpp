/*
  ==============================================================================

    SoomplerSlider.cpp
    Created: 29 Jul 2019 8:23:24pm
    Author:  artyom

  ==============================================================================
*/

#include "SoomplerSlider.h"

SoomplerSlider::SoomplerSlider() : Slider(SliderStyle::Rotary, TextEntryBoxPosition::NoTextBox)
{
    
}

SoomplerSlider::SoomplerSlider(Slider::SliderStyle style, Slider::TextEntryBoxPosition textBoxPosition) : Slider(style, textBoxPosition)
{

}

void SoomplerSlider::mouseMove(const MouseEvent &event)
{
    Slider::mouseMove(event);

    setMouseCursor(MouseCursor::PointingHandCursor);
}
