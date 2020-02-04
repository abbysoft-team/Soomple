#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../viewer/SampleViewer.h"
#include "../PluginProcessor.h"
#include "PianoRoll.h"
#include "GuiEditor.h"
#include "ToggledImageButton.h"
#include "SoomplerSlider.h"
#include "SoomplerImageButton.h"
#include "SoomplerToggleButton.h"
#include "LinearPanel.h"
#include "SoomplerKnob.h"
#include "AdsrPanel.h"
#include "SoomplerScrollbar.h"

class MainPanel  : public Component,
                   public Slider::Listener,
                   public Button::Listener,
                   public TransportStateListener,
                   public FileDragAndDropTarget,
                   public SampleChangeListener,
                   private Timer

{
public:
    MainPanel (SoomplerAudioProcessor& processor);
    ~MainPanel();

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void transportStateChanged(TransportState state) override;

    float getVolume() const;

    void sampleChanged(std::shared_ptr<SampleInfo> info);
    void noSamplesLeft() override;

private:

    AudioProcessorValueTreeState& stateManager;
    
    std::unique_ptr<ButtonAttachment> loopAttachment;
    std::unique_ptr<ButtonAttachment> reverseAttachment;
    
    std::unique_ptr<SoomplerKnob> volumeKnob;
    std::unique_ptr<SoomplerKnob> glideKnob;
    
    std::shared_ptr<AdsrPanel> adsrPanel;
    
    std::unique_ptr<Label> loadSampleTip;

    std::shared_ptr<SampleViewer> sampleViewer;
    std::shared_ptr<PianoRoll> pianoRoll;
    std::unique_ptr<SoomplerScrollbar> pianoScroll;

    std::unique_ptr<SoomplerImageButton> openFileButton;
    std::unique_ptr<SoomplerImageButton> aboutButton;
    std::unique_ptr<ToggledImageButton> loopButton;

    std::unique_ptr<SoomplerToggleButton> reverseButton;
    
    Image backgroundImage;

    SoomplerAudioProcessor& processor;

    // ability to edit components for this panel
    GuiEditor editor;

    bool sampleLoaded;

    void restoreMainPanelState();

    void openFileButtonClicked();
    void aboutButtonClicked();
    void playSampleButtonClicked();
    void stopSampleButtonClicked();
    void loopButtonClicked();
    void reverseButtonClicked();
    
    bool isInterestedInFileDrag(const juce::StringArray &files) override;
    void filesDropped(const juce::StringArray &files, int x, int y) override;
    
    void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
};

