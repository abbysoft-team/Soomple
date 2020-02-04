/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "TransportStateListener.h"
#include "Settings.h"
#include "gui/MainPanel.h"
#include "browser/SampleBrowser.h"
#include "gui/AboutSplash.h"

//==============================================================================
/**
*/
class SoomplerAudioProcessorEditor  :
        public AudioProcessorEditor,
        private TransportStateListener,
        private ChangeListener
{
public:
    explicit SoomplerAudioProcessorEditor (SoomplerAudioProcessor&);
    ~SoomplerAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void showAboutSplash();
    void hideAboutSplash();
    
    bool keyPressed (const KeyPress& key) override;

private:
    void transportStateChanged(TransportState state) override;

    void changeListenerCallback(ChangeBroadcaster* source) override ;
    void thumbnailChanged();
    void drawSampleNameOrMessage(Graphics& graphics);

    int64 calculateSampleByCoords(int coord);

    void calculateEndRangeX();

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SoomplerAudioProcessor& processor;

    MainPanel mainPanel;
    std::shared_ptr<SampleBrowser> sampleBrowser;
    AboutSplash aboutSplash;

    Font mainFont;

    // OpenGL context to speed up UI rendering
    OpenGLContext glContext;
    
    SharedResourcePointer<TooltipWindow> tooltipWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoomplerAudioProcessorEditor)
};
