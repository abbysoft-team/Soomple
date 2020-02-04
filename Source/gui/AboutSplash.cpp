/*
  ==============================================================================

    AboutSplash.cpp
    Created: 16 Aug 2019 4:07:15pm
    Author:  MacPro

  ==============================================================================
*/

#include "AboutSplash.h"
#include "../Strings.h"
#include "../Settings.h"

AboutSplash::AboutSplash() : Component() {
    background = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    
    goBackButton.setButtonText(Strings::GO_BACK);
    goBackButton.onClick = [this] {this->setVisible(false);};
    addAndMakeVisible(goBackButton);
    
    aboutStrings.push_back(Strings::ABOUT_DIALOG_1);
    aboutStrings.push_back(Strings::ABOUT_DIALOG_2);
    aboutStrings.push_back(Strings::ABOUT_DIALOG_3);
    aboutStrings.push_back(Strings::ABOUT_DIALOG_4);
    aboutStrings.push_back(Strings::ABOUT_DIALOG_5);
}

void AboutSplash::paint(Graphics &g) {
    g.drawImage(background, getLocalBounds().toFloat());
    
    g.setColour(Settings::PANEL_BACKGROUND_COLOR);
    g.fillRect(Rectangle<int>(50, 70, getWidth() - 100, getHeight() - 150));
    
    g.setColour(Colour(240, 240, 240));
    g.setFont(25);
    
    for (int i = 0; i < aboutStrings.size(); i++) {
        auto nextString = aboutStrings.at(i);
        g.drawSingleLineText(nextString, 90, 100 + 30 * i);
    }
    
    Component::paint(g);
}

void AboutSplash::resized() {
    goBackButton.setBounds(getWidth() / 2 - 120 / 2, getHeight() - 65, 120, 50);
}
