/*
  ==============================================================================

    SoomplerKnob.h
    Created: 18 Aug 2019 12:13:46pm
    Author:  MacPro

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SoomplerSlider.h"
#include "../PluginProcessor.h"

class SoomplerKnob : public Component {
    
public:
    SoomplerKnob(const String &name);
    ~SoomplerKnob() = default;
    
    void resized() override;
    
    void setPosition(int x, int y);
    
    void addListener(Slider::Listener *listener);
    
    double getValue() const;
    void setValue(double value);
    
    Slider* getSlider();
    
    void attachTo(const String &parameter, AudioProcessorValueTreeState &stateManager);
    
private:
    std::shared_ptr<SoomplerSlider> slider;
    Label label;
    std::shared_ptr<SliderAttachment> attachment;
};
