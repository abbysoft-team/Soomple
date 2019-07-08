/*
  ==============================================================================

    SampleInfo.h
    Created: 5 Jul 2019 2:41:38pm
    Author:  apopov

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
 * All required info about sample.
 * Convinient to message this info between objects.
 *
 * @brief The SampleInfo class
 */
class SampleInfo
{
public:

    SampleInfo(float lengthInSeconds, float sampleRate, String sampleName)
    {
        DBG("New file: ");
        DBG(sampleName);
        this->lengthInSeconds = lengthInSeconds;
        this->sampleRate = sampleRate;
        this->lengthInSamples = lengthInSeconds * sampleRate;
        this->startSample = 0;
        this->endSample = lengthInSamples;
        this->sampleName = sampleName;
    }

    ~SampleInfo() = default;

    float sampleRate;
    float lengthInSeconds;
    int64 lengthInSamples;

    // this sample range marked as active, other samples would be not
    // played
    int64 startSample;
    int64 endSample;

    String sampleName;
};

/**
 * Interface for SampleInfo consumers
 *
 * @brief The SampleInfoListener class
 */
class SampleInfoListener
{
public:
    virtual void newSampleInfoRecieved(std::shared_ptr<SampleInfo> info) = 0;
};
