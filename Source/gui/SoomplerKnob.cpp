/*
  ==============================================================================

    SoomplerKnob.cpp
    Created: 18 Aug 2019 12:13:46pm
    Author:  MacPro

  ==============================================================================
*/

#include "SoomplerKnob.h"
#include "../PluginProcessor.h"

SoomplerKnob::SoomplerKnob(const String &name) : Component(name), label(name, name) {
    slider.reset(new SoomplerSlider());
    addAndMakeVisible(slider.get());
    addAndMakeVisible(label);
    
    slider->setTooltip(name);
    
    setSize(50, 65);
}

void SoomplerKnob::resized() {
    FlexBox fb;
    fb.flexDirection = FlexBox::Direction::column;
    
    fb.items.add(FlexItem(label).withFlex(3).withMargin(FlexItem::Margin(0, 0, 0, 7.0f)));
    fb.items.add(FlexItem(*(slider.get())).withFlex(12).withMargin(FlexItem::Margin(-10.0f, 0, 0, 0)));
    
    fb.performLayout(getLocalBounds());
}

void SoomplerKnob::setPosition(int x, int y) {
    setBounds(x, y, 50, 65);
}

void SoomplerKnob::addListener(Slider::Listener *listener) {
    slider->addListener(listener);
}

double SoomplerKnob::getValue() const {
    return slider->getValue();
}

void SoomplerKnob::setValue(double value)
{
    slider->setValue(value, NotificationType::sendNotification);
}

juce::Slider *SoomplerKnob::getSlider() {
    return slider.get();
}

void SoomplerKnob::attachTo(const String &parameter, AudioProcessorValueTreeState &stateManager) {
    attachment.reset(new SliderAttachment(stateManager, parameter, *(slider.get())));
}



