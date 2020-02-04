/*
  ==============================================================================

    SampleManager.h
    Created: 9 Sep 2019 2:26:58pm
    Author:  artyom

  ==============================================================================
*/

#pragma once

#include "SampleInfo.h"
#include "SaveableState.h"

class SampleManager : public SampleChangeListener, public SaveableState {

public:
    SampleManager();
    ~SampleManager() = default;

    void sampleChanged(std::shared_ptr<SampleInfo> info) override;
    void removeSample(std::shared_ptr<SampleInfo> sample);
    void addSampleInfoListener(SampleChangeListener* sampleInfoListener);
    void notifySampleInfoListeners();
    std::shared_ptr<SampleInfo> getActiveSample();
    std::vector<std::shared_ptr<SampleInfo>> getAllSamples();
    void addChangeListener(ChangeListener* listener);
    void removeChangeListener(ChangeListener* listener);

    void saveStateToMemory(StateBundle &bundle);
    void getStateFromMemory(StateBundle &bundle);
private:
    std::vector<std::shared_ptr<SampleInfo>> samples;
    std::shared_ptr<SampleInfo> activeSample;
    std::vector<SampleChangeListener*> sampleInfoListeners;
    std::vector<ChangeListener*> changeListeners;

    bool contains(std::shared_ptr<SampleInfo> sample);

    void saveSample(StateBundle& bundle, std::shared_ptr<SampleInfo> sample, int index);
    void loadSample(StateBundle& bundle, const String& name);

};
