/*
  ==============================================================================

    SoomplerSynth.h
    Created: 9 Sep 2019 12:29:29pm
    Author:  artyom

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SoomplerSynth : public Synthesiser {

public:
    SoomplerSynth();
    ~SoomplerSynth() = default;

    void noteOn(int midiChannel, int midiNoteNumber, float velocity) override;

};
