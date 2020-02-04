/*
  ==============================================================================

    AdsrPanel.h
    Created: 10 Sep 2019 12:14:30pm
    Author:  artyom

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LinearPanel.h"
#include "SoomplerKnob.h"

class AdsrPanel : public LinearPanel, public Slider::Listener, public SampleChangeListener {

public:
    AdsrPanel(AudioProcessorValueTreeState& state, std::shared_ptr<SampleManager> manager);
    ~AdsrPanel() = default;

    void sliderValueChanged(Slider *slider) override;
    void sampleChanged(std::shared_ptr<SampleInfo> info);

    void setValue(ADSR::Parameters adsr);

private:
    std::unique_ptr<SoomplerKnob> attackKnob;
    std::unique_ptr<SoomplerKnob> decayKnob;
    std::unique_ptr<SoomplerKnob> sustainKnob;
    std::unique_ptr<SoomplerKnob> releaseKnob;

    ADSR::Parameters adsrParams;

    std::shared_ptr<SampleManager> manager;
};
