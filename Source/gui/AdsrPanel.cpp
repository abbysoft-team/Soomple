/*
  ==============================================================================

    AdsrPanel.cpp
    Created: 10 Sep 2019 12:14:30pm
    Author:  artyom

  ==============================================================================
*/

#include "AdsrPanel.h"

AdsrPanel::AdsrPanel(AudioProcessorValueTreeState& state, std::shared_ptr<SampleManager> manager) : LinearPanel(Orientation::HORIZONTAL, "ADSR envelope"), manager(manager)
{
    // ADSR controls
    attackKnob.reset (new SoomplerKnob ("Attack"));
    attackKnob->attachTo("attack", state);

    decayKnob.reset (new SoomplerKnob ("Decay"));
    decayKnob->attachTo("decay", state);

    sustainKnob.reset (new SoomplerKnob ("Sustain"));
    sustainKnob->attachTo("sustain", state);

    releaseKnob.reset (new SoomplerKnob ("Release"));
    releaseKnob->attachTo("release", state);

    addAndMakeVisible(attackKnob.get());
    addAndMakeVisible(decayKnob.get());
    addAndMakeVisible(sustainKnob.get());
    addAndMakeVisible(releaseKnob.get());

    attackKnob->addListener(this);
    decayKnob->addListener(this);
    sustainKnob->addListener(this);
    releaseKnob->addListener(this);
}

void AdsrPanel::sliderValueChanged(Slider *slider)
{
    adsrParams.attack = attackKnob->getValue();
    adsrParams.decay = decayKnob->getValue();
    adsrParams.sustain = sustainKnob->getValue();
    adsrParams.release = releaseKnob->getValue();

    manager->getActiveSample()->setAdsr(adsrParams);
}

void AdsrPanel::sampleChanged(std::shared_ptr<SampleInfo> info)
{
    setValue(info->getAdsr());
}

void AdsrPanel::setValue(ADSR::Parameters adsr)
{
    adsrParams = adsr;
    attackKnob->setValue(adsr.attack);
    decayKnob->setValue(adsr.decay);
    sustainKnob->setValue(adsr.sustain);
    releaseKnob->setValue(adsr.release);
}
