/*
  ==============================================================================

    SoomplerScrollbar.cpp
    Created: 12 Sep 2019 12:28:21pm
    Author:  artyom

  ==============================================================================
*/

#include "SoomplerScrollbar.h"
#include "../Settings.h"

SoomplerScrollbar::SoomplerScrollbar(bool isVertical) : ScrollBar(isVertical)
{

}

void SoomplerScrollbar::paint(Graphics &g)
{
    g.setColour(Settings::SCROLL_BAR_BACKGROUND_COLOR);
    g.fillRect(0, 0, getWidth(), getHeight());

    ScrollBar::paint(g);
}
