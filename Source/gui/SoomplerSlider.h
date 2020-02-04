/*
  ==============================================================================

    SoomplerSlider.h
    Created: 29 Jul 2019 8:23:24pm
    Author:  artyom

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SoomplerSlider : public Slider {

public:

    SoomplerSlider();
    SoomplerSlider(SliderStyle style, TextEntryBoxPosition textBoxPosition);
    ~SoomplerSlider() = default;

    void mouseMove(const MouseEvent &event) override;
};
