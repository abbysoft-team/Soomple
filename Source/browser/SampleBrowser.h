/*
  ==============================================================================

    SampleBrowser.h
    Created: 9 Aug 2019 5:05:47pm
    Author:  artyom

  ==============================================================================
*/
#include <JuceHeader.h>
#include "../FileListener.h"
#include "../PluginProcessor.h"
#include "../SaveableState.h"
#include "SamplePreviewComponent.h"

#pragma once

class SampleBrowser : public Component, public FileBrowserListener, public SaveableState {

public:
    SampleBrowser(FileListener& listener, SoomplerAudioProcessor &processor);
    ~SampleBrowser() = default;

private:
        std::shared_ptr<SamplePreviewComponent> previewComponent;
        std::unique_ptr<FileBrowserComponent> browser;
        FileListener& fileListener;
        Image background;

        // Component interface
public:
        void resized();

        // FileBrowserListener interface
public:
        void selectionChanged() override;
        void fileClicked(const File &file, const MouseEvent &e) override;
        void fileDoubleClicked(const File &file) override;
        void browserRootChanged(const File &newRoot) override;

        // Component interface
public:
        void paint(Graphics &g);

        // SaveableState interface
public:
        void saveStateToMemory(StateBundle &bundle);
        void getStateFromMemory(StateBundle &bundle);
};
