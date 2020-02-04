/*
  ==============================================================================

    ToggledImageButton.h
    Created: 17 Jul 2019 3:05:13pm
    Author:  Apopov

  ==============================================================================
*/

#include <../JuceLibraryCode/JuceHeader.h>

class ToggledImageButton : public ImageButton {
public:
    
    ToggledImageButton(String buttonName);
    ~ToggledImageButton() = default;
    
    bool isToggled() const;
    void setToggled(bool toggled);
    
protected:
    
    void paintButton (Graphics&, bool, bool) override;
    void mouseDown(const MouseEvent &event) override;
    void mouseMove(const MouseEvent &event) override;

private:
    
    bool toggled;
};

#pragma once
