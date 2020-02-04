/*
  ==============================================================================

    ExtendedSound.h
    Created: 27 Jun 2019 4:53:19pm
    Author:  AbbySoft

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
 * Wrapper around @SamplerVoice
 *
 * @brief The ExtendedSound class
 */
class ExtendedVoice : public SynthesiserVoice {

public:

    /** Creates a SamplerVoice. */
    ExtendedVoice();

    /** Destructor. */
    ~ExtendedVoice() override;


    void setStartSample(int64 startSample);
    void setEndSample(int64 endSample);

    bool canPlaySound (SynthesiserSound*) override;

    void startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff) override;

    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;

    void renderNextBlock (AudioBuffer<float>&, int startSample, int numSamples) override;

private:
    double pitchRatio = 0;

    int64 endSample;
    int64 startSample;


    JUCE_LEAK_DETECTOR (ExtendedVoice)
};
