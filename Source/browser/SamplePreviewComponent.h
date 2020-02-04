/*
  ==============================================================================

    SamplePreviewComponent.h
    Created: 9 Aug 2019 6:21:41pm
    Author:  artyom

  ==============================================================================
*/
#include <JuceHeader.h>
#include "../gui/SoomplerSlider.h"
#include "../gui/SoomplerImageButton.h"
#include "../gui/SoomplerToggleButton.h"
#include "../PluginProcessor.h"
#include "SamplePreviewSource.h"
#include <thread>

#pragma once

class SamplePreviewComponent : public FilePreviewComponent, ChangeListener, SamplePreviewSource, Slider::Listener, public SaveableState {
public:
    SamplePreviewComponent(SoomplerAudioProcessor &processor);
    ~SamplePreviewComponent();

    // FilePreviewComponent interface
public:
    void selectedFileChanged(const File &newSelectedFile);
    // Component interface
public:
    void resized();
    // ChangeListener interface
public:
    void changeListenerCallback(ChangeBroadcaster *source);
    // AudioSource interface
public:
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;


    void setAutoplay(bool autoplay);
    bool isReady() override;

    void getNextAudioBlock (AudioBuffer<float>& buffer) override;

private:
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    SoomplerAudioProcessor &processor;

    std::shared_ptr<SoomplerSlider> volume;
    std::shared_ptr<SoomplerImageButton> playButton;
    std::shared_ptr<SoomplerImageButton> stopButton;
    std::shared_ptr<SoomplerToggleButton> autoplayButton;

    AudioTransportSource transportSource;
    TransportState transportState;
    bool autoplay;
    std::unique_ptr<File> currentFile;

    std::unique_ptr<std::thread> transportStopThread;
    void playButtonClicked();
    void stopButtonClicked();

    void changeState (TransportState newState);

    void setFileAsTransportSource(File &file);

    void sliderValueChanged(Slider *slider) override;

    void stopTransportAsync();

    void saveStateToMemory(StateBundle &bundle);
    void getStateFromMemory(StateBundle &bundle);
};
