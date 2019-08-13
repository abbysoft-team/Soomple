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
    : AudioProcessorEditor (&p), processor (p), mainPanel(processor), sampleBrowser(processor, processor)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT);
   
    // load built-in font
    auto typeface = Typeface::createSystemTypefaceFor(BinaryData::RobotoCondensedRegular_ttf, BinaryData::RobotoCondensedRegular_ttfSize);
    mainFont = Font(typeface);
    // set OpenGL renderer
    this->glContext.attachTo(*this);

    // SampleBrowser
    sampleBrowser.setBounds(Settings::BROWSER_BOUNDS);
    mainPanel.setBounds(Settings::MAIN_PANEL_X, Settings::MAIN_PANEL_Y, mainPanel.getWidth(), mainPanel.getHeight());

    addAndMakeVisible(sampleBrowser);
    addAndMakeVisible(mainPanel);

    // subscribe to all transport events from processor
    processor.setTransportStateListener(this);
    // subscribe to thumbnail events, to catch thumbnail fully loaded time
    processor.getThumbnail().addChangeListener(this);

    LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(typeface);
}

SoomplerAudioProcessorEditor::~SoomplerAudioProcessorEditor()
{
    processor.getThumbnail().removeChangeListener(this);
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
    if (source == &processor.getThumbnail()) {
        thumbnailChanged(processor.getThumbnail());
    }
}

void SoomplerAudioProcessorEditor::thumbnailChanged(SAudioThumbnail &thumbnail)
{
    // sample loaded
    processor.setVolume(mainPanel.getVolume());
    repaint();
}
