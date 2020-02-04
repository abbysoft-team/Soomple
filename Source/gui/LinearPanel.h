/*
  ==============================================================================

    LinearPanel.h
    Created: 16 Aug 2019 8:01:25pm
    Author:  MacPro

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum Orientation {
    VERTICAL,
    HORIZONTAL
};

class LinearPanel : public Component {
public:
    LinearPanel(Orientation orientation, const String &name = "");
    
    ~LinearPanel() = default;
    
    void resized() override;
    void paint(Graphics &g) override;
    void addAndMakeVisible(Component *component);
    void setPosition(int x, int y);
    void setPadding(int padding);
    
private:
    Orientation orientation;
    int paddingX;
    int paddingY;
    
    std::unique_ptr<Label> panelLabel;
    
    void pack();
};
