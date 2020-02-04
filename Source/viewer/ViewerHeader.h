/*
  ==============================================================================

    ViewerHeader.h
    Created: 9 Sep 2019 1:53:02pm
    Author:  artyom

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../SampleInfo.h"
#include "../SampleManager.h"
#include "../gui/SoomplerImageButton.h"

class ViewerHeader : public Component, public SampleChangeListener {

public:

    ViewerHeader(std::shared_ptr<SampleManager> manager);
    ~ViewerHeader() = default;

    void paint(Graphics &g) override;
    void sampleChanged(std::shared_ptr<SampleInfo> info) override;

private:
    std::shared_ptr<SampleManager> manager;
    std::shared_ptr<SampleInfo> sample;

    Image closeImage;

    void paintNextSampleHeader(int index, float width, std::shared_ptr<SampleInfo> info, Graphics& g);
    void mouseMove(const MouseEvent &event) override;
    void mouseDown(const MouseEvent &event) override;
    std::shared_ptr<SampleInfo> getSampleUnderMouse(Point<int> position);
    bool closeButtonPressed(Point<int> position);
};
