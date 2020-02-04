/*
  ==============================================================================

    Glide.h
    Created: 13 Sep 2019 1:47:08pm
    Author:  artyom

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Glide {
public:
    Glide();
    ~Glide() = default;

    float getGlide(int numSamples);
    void noteOn(int noteNumber);
    void setLevel(float level);
    void setSampleRate(float sampleRate);

private:
    int currentNote;
    int prevNote;
    float sampleRate;
    float relation;

    // glide level from 0.0f to 1.0f
    float level;

    int samplesLeft;
};

class GlideTest : UnitTest {
public:
    // UnitTest interface
    GlideTest()  : UnitTest ("Glide testing") {}

    void runTest();

};

static GlideTest test;
