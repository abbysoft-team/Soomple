/*
  ==============================================================================

    SamplePreviewSource.h
    Created: 13 Aug 2019 6:51:09pm
    Author:  artyom

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SamplePreviewSource {
public:

    virtual void prepareToPlay(int samplesPerBlockExpected, double sampleRate) = 0;
    virtual void releaseResources() = 0;

    virtual void getNextAudioBlock (AudioBuffer<float>& buffer) = 0;

    virtual bool isReady() = 0;

    ~SamplePreviewSource() = default;
};
