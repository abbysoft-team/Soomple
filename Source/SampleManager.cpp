/*
  ==============================================================================

    SampleManager.cpp
    Created: 9 Sep 2019 2:26:58pm
    Author:  artyom

  ==============================================================================
*/

#include "SampleManager.h"


SampleManager::SampleManager()
{

}

void SampleManager::sampleChanged(std::shared_ptr<SampleInfo> info)
{
    activeSample = info;
    notifySampleInfoListeners();

    if (contains(info)) {
        return;
    }
    samples.push_back(info);

    // update change listener
    if (info->thumbnail == nullptr) {
        return;
    }
    for (auto listener : changeListeners) {
        info->thumbnail->addChangeListener(listener);
    }
}

void SampleManager::removeSample(std::shared_ptr<SampleInfo> sample)
{
    samples.erase(std::remove(samples.begin(), samples.end(), sample), samples.end());
}

void SampleManager::addSampleInfoListener(SampleChangeListener* sampleInfoListener)
{
    sampleInfoListeners.push_back(sampleInfoListener);
}

bool SampleManager::contains(std::shared_ptr<SampleInfo> info) {
    for (auto sample : samples) {
        if (sample == info) {
            return true;
        }
    }

    return false;
}

void SampleManager::notifySampleInfoListeners()
{
    for (auto sampleInfoListener : sampleInfoListeners) {
        sampleInfoListener->sampleChanged(activeSample);
    }
}

std::shared_ptr<SampleInfo> SampleManager::getActiveSample()
{
    return activeSample;
}

std::vector<std::shared_ptr<SampleInfo> > SampleManager::getAllSamples()
{
    return samples;
}

void SampleManager::addChangeListener(ChangeListener* listener)
{
    changeListeners.push_back(listener);
    if (activeSample != nullptr && activeSample->thumbnail != nullptr) {
        activeSample->thumbnail->addChangeListener(listener);
    }
}

void SampleManager::removeChangeListener(ChangeListener *listener)
{
    for (auto sample : samples) {
        if (sample->thumbnail != nullptr) {
            sample->thumbnail->removeChangeListener(listener);
        }
    }
}

void SampleManager::saveStateToMemory(StateBundle &bundle)
{
    // reset state
    bundle.addProperty((int) samples.size(), "sampleCount");

    for (auto i = 0; i < samples.size(); i++) {
        saveSample(bundle, samples.at(i), i);
    }
}

void SampleManager::saveSample(StateBundle &bundle, std::shared_ptr<SampleInfo> sample, int index)
{
    String name = "sample" + index;

    bundle.addProperty(sample->sampleName, name);
    bundle.addProperty(sample->samplePath, name + "path");
    bundle.addProperty(sample->startSample, name + "start");
    bundle.addProperty(sample->endSample, name + "end");
    bundle.addProperty(sample->lengthInSeconds, name + "length");
    bundle.addProperty(sample->sampleRate, name + "sampleRate");
    bundle.addProperty(sample->rootNote, name + "root");
    bundle.addProperty(sample->minNote, name + "min");
    bundle.addProperty(sample->maxNote, name + "max");
    bundle.addProperty(sample->getVolume(), name + "volume");
    bundle.addProperty(sample->getAdsr().attack, name + "start");
    bundle.addProperty(sample->getAdsr().decay, name + "decay");
    bundle.addProperty(sample->getAdsr().sustain, name + "sustain");
    bundle.addProperty(sample->getAdsr().release, name + "release");
}

void SampleManager::getStateFromMemory(StateBundle &bundle)
{
    int samples = bundle.getProperty("sampleCount");
    for (int i = 0; i < samples; i++) {
        loadSample(bundle, "sample" + i);
    }
}

void SampleManager::loadSample(StateBundle &bundle, const String &property)
{
    String name = bundle.getProperty(property);
    String path = bundle.getProperty(property + "path");
    float length = bundle.getProperty(property + "length");
    float sampleRate = bundle.getProperty(property + "sampleRate");

    auto sampleInfo = std::make_shared<SampleInfo>(length, sampleRate, name, path);
    samples.push_back(sampleInfo);
    activeSample = sampleInfo;
}

