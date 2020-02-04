/*
  ==============================================================================

    Glide.cpp
    Created: 13 Sep 2019 1:47:08pm
    Author:  artyom

  ==============================================================================
*/

#include "Glide.h"

Glide::Glide() : currentNote(-1), prevNote(-1), level(0.0f), sampleRate(44100), samplesLeft(0), relation(0)
{

}

float Glide::getGlide(int numSamples)
{
    if (samplesLeft <= 0 || prevNote == -1 || level == 0.0f) {
        return 1.0f;
    }

    // calc glide
    auto slope = numSamples / (samplesLeft * 1.0f);

    if (slope >= 0.9) {
        slope = 1;
    }

    samplesLeft -= numSamples;

    auto rel = relation - (relation - 1.0f) * slope;
    DBG(rel);

    return relation - (relation - 1.0f) * slope;
}

void Glide::noteOn(int noteNumber)
{
    prevNote = currentNote;
    currentNote = noteNumber;

    relation = (currentNote - prevNote) / 12.0f;

    if (relation == 0) {
        samplesLeft = 0;
        return;
    }
    relation = pow(2, -1 * relation);

    samplesLeft = level * sampleRate;
}

void Glide::setLevel(float level)
{
    this->level = level;
}

void Glide::setSampleRate(float sampleRate)
{
    this->sampleRate = sampleRate;
}

// test

void GlideTest::runTest()
{
    auto glide = Glide();

    glide.setLevel(0.5f);
    glide.noteOn(50);

    for (int i = 0; i < 3; i++) {
        glide.getGlide(5120);
    }

    glide.noteOn(62);

    for (int i = 0; i < 3; i++) {
        glide.getGlide(10000);
    }

    glide.noteOn(38);

    for (int i = 0; i < 3; i++) {
        glide.getGlide(10000);
    }

}
