/*
  ==============================================================================

    AboutSplash.h
    Created: 16 Aug 2019 4:07:15pm
    Author:  MacPro

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class AboutSplash : public Component {
    
public:
    
    AboutSplash();
    ~AboutSplash() = default;
    
    void paint(Graphics &g) override;
    void resized() override;
    
private:
    Image background;
    
    TextButton goBackButton;
    std::vector<String> aboutStrings;
    
};
