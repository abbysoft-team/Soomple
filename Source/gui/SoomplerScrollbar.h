/*
  ==============================================================================

    SoomplerScrollbar.h
    Created: 12 Sep 2019 12:28:21pm
    Author:  artyom

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SoomplerScrollbar : public ScrollBar {
public:
    SoomplerScrollbar(bool isVertical);
    ~SoomplerScrollbar() = default;

    void paint(Graphics &g) override;

};
