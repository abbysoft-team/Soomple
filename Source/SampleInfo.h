/*
  ==============================================================================

    SampleInfo.h
    Created: 5 Jul 2019 2:41:38pm
    Author:  apopov

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Settings.h"
#include "gui/SAudioThumbnail.h"
#include "synth/ExtendedSampler.h"

/**
 * All required info about sample.
 * Convinient to message this info between objects.
 *
 * @brief The SampleInfo class
 */
class SampleInfo
{
public:

    SampleInfo(float lengthInSeconds, float sampleRate, String sampleName, String samplePath)
    {
        this->lengthInSeconds = lengthInSeconds;
        this->sampleRate = sampleRate;
        this->lengthInSamples = lengthInSeconds * sampleRate;
        this->startSample = 0;
        this->endSample = lengthInSamples;
        this->sampleName = sampleName;
        this->samplePath = samplePath;
        this->rootNote = Settings::DEFAULT_ROOT_NOTE;
        this->minNote = Settings::DEFAULT_MIN_NOTE;
        this->maxNote = Settings::DEFAULT_MAX_NOTE;


        thumbnail = nullptr;

        adsr.attack = 0;
        adsr.decay = 0;
        adsr.sustain = 1;
        adsr.release = 0;

        volume = 1;
        sound = nullptr;
        reversed = false;
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
    String samplePath;

    // root note and range for this sample
    int rootNote;
    int minNote;
    int maxNote;

    bool reversed;

    std::shared_ptr<SAudioThumbnail> thumbnail;
    soompler::ExtendedSound* sound;

    String getCroppedName(float width, int fontSize)
    {
        auto lettersCount = width / fontSize * 5 / 2;

        if (sampleName.length() <= lettersCount) {
          return sampleName;
        }

        String result =  sampleName.substring(0, lettersCount - 4);
        result.append("...", 3);

        return result;
    }

    void setAdsr(ADSR::Parameters params) {
        adsr = params;
        if (sound != nullptr) {
            sound->setAdsrParams(adsr);
        }
    }

    void setVolume(float volume) {
        this->volume = volume;
        if (sound != nullptr) {
            // set volume here
            sound->setVolume(volume);
        }
    }

    float getVolume() {
        return volume;
    }

    ADSR::Parameters getAdsr() {
        return adsr;
    }

private:
    ADSR::Parameters adsr;
    float volume;

};

/**
 * Interface for SampleInfo consumers
 *
 * @brief The SampleInfoListener class
 */
class SampleChangeListener
{
public:
    virtual void sampleChanged(std::shared_ptr<SampleInfo> info) = 0;
    virtual void noSamplesLeft() {}
    
    virtual ~SampleChangeListener() = default;
};
