/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Strings.h"

//==============================================================================
SoomplerAudioProcessorEditor::SoomplerAudioProcessorEditor (SoomplerAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), mainPanel(processor)
{
    sampleBrowser.reset(new SampleBrowser(processor, processor));

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT);
   
    // load built-in font
    auto typeface = Typeface::createSystemTypefaceFor(BinaryData::RobotoCondensedRegular_ttf, BinaryData::RobotoCondensedRegular_ttfSize);
    mainFont = Font(typeface);
    // set OpenGL renderer
    this->glContext.attachTo(*this);

    // SampleBrowser
    sampleBrowser->setBounds(Settings::BROWSER_BOUNDS);
    mainPanel.setBounds(Settings::MAIN_PANEL_X, Settings::MAIN_PANEL_Y, mainPanel.getWidth(), mainPanel.getHeight());
    aboutSplash.setBounds(0, 0, getWidth(), getHeight());

    addAndMakeVisible(sampleBrowser.get());
    addAndMakeVisible(mainPanel);
    addAndMakeVisible(aboutSplash);
    
    aboutSplash.setVisible(false);

    // subscribe to all transport events from processor
    processor.setTransportStateListener(this);
    // subscribe to thumbnail events, to catch thumbnail fully loaded time
    processor.getSampleManager()->addChangeListener(this);

    // StateSaving
    processor.addNewSaveableObject(sampleBrowser);

    LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(typeface);
}

SoomplerAudioProcessorEditor::~SoomplerAudioProcessorEditor()
{
    processor.saveStateAndReleaseObjects();
    processor.getSampleManager()->removeChangeListener(this);
}

//==============================================================================
void SoomplerAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

}

void SoomplerAudioProcessorEditor::resized()
{
}

void SoomplerAudioProcessorEditor::transportStateChanged(TransportState state)
{
    switch (state) {
    case Ready:
        break;
    case Starting:
        break;
    case Stopped:
        break;
    default:
        break;
    }

    // pass state to current active panel
    mainPanel.transportStateChanged(state);
}

void SoomplerAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster *source)
{
    auto thumbnail = processor.getSampleManager()->getActiveSample()->thumbnail;
    if (source == thumbnail.get()) {
        thumbnailChanged();
    }
}

void SoomplerAudioProcessorEditor::thumbnailChanged()
{
    // sample loaded
    processor.setVolume(mainPanel.getVolume());
    repaint();
}

void SoomplerAudioProcessorEditor::showAboutSplash() {
    aboutSplash.setVisible(true);
}

void SoomplerAudioProcessorEditor::hideAboutSplash() {
    aboutSplash.setVisible(false);
}

bool SoomplerAudioProcessorEditor::keyPressed (const KeyPress& key) {
    // interfere with DAW keybinding
//    if (key.getKeyCode() == KeyPress::spaceKey) {
//        processor.playOrStopRootNote();
//        return true;
//    }
//
    static String keys = "q2w3er5t6y7ui9o0p[azsxdcvgbhnmk,l.;/";
    auto firstC = Settings::DEFAULT_FIRST_KEY;
    
    auto pressedChar = key.getTextCharacter();
    auto nextChar = keys[0];
    auto note = -1;
    for (auto i = 0; i < keys.length(); i++) {
        nextChar = keys[i];
        if (nextChar == pressedChar) {
            note = i + firstC;
            break;
        }
    }
    
    if (note == -1) {
        return false;
    }
    
    processor.noteOn(note);
    
    return true;
}
