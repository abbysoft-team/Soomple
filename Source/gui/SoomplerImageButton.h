/*
  ==============================================================================

    SoomplerImageButton.h
    Created: 29 Jul 2019 8:24:44pm
    Author:  artyom

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SoomplerImageButton : public ImageButton {

public:
    SoomplerImageButton(String name);
    ~SoomplerImageButton() = default;

    void mouseMove(const MouseEvent &event) override;

};
