#include "MainPanel.h"
#include "../Settings.h"
#include "../Strings.h"
#include "../PluginEditor.h"

//==============================================================================
MainPanel::MainPanel (SoomplerAudioProcessor& processor) : stateManager(processor.getStateManager()),
    processor(processor), editor(this)
{
    setSize(Settings::MAIN_PANEL_WIDTH, Settings::MAIN_PANEL_HEIGHT);
    setName("mainPanel");

    backgroundImage = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);

    volumeKnob.reset (new SoomplerKnob("Volume"));
    addAndMakeVisible(volumeKnob.get());
    editor.addToGuiEditor(volumeKnob.get());
    volumeKnob->attachTo("volume", stateManager);

    glideKnob.reset(new SoomplerKnob("Glide"));
    addAndMakeVisible(glideKnob.get());
    editor.addToGuiEditor(glideKnob.get());
    glideKnob->attachTo("glide", stateManager);

    openFileButton.reset (new SoomplerImageButton ("open file button"));
    addAndMakeVisible (openFileButton.get());
    editor.addToGuiEditor (openFileButton.get());
    openFileButton->setTooltip (TRANS("Open Sample\n"));
    openFileButton->setButtonText (TRANS("new button"));
    openFileButton->addListener (this);
    openFileButton->setImages (false, true, true,
                               ImageCache::getFromMemory (BinaryData::openFile_png, BinaryData::openFile_pngSize),
                               1.000f, Colour (0x00000000),
                               Image(), 1.000f, Colour (0x00000000),
                               Image(), 1.000f, Colour (0x00000000));
    openFileButton->setBounds (8, 8, 39, 32);

    aboutButton.reset (new SoomplerImageButton ("about button"));
    addAndMakeVisible (aboutButton.get());
    editor.addToGuiEditor (aboutButton.get());
    aboutButton->setTooltip (TRANS("About Program\n"));
    aboutButton->setButtonText (TRANS("About program"));
    aboutButton->addListener (this);
    aboutButton->setImages (false, true, true,
                               ImageCache::getFromMemory (BinaryData::about_png, BinaryData::about_pngSize),
                               1.000f, Colour (0x00000000),
                               Image(), 1.000f, Colour (0x00000000),
                               Image(), 1.000f, Colour (0x00000000));
    aboutButton->setBounds (8, 8, 39, 32);
    
    loopButton.reset (new ToggledImageButton ("loop button"));
    addAndMakeVisible (loopButton.get());
    editor.addToGuiEditor (loopButton.get());
    loopButton->setTooltip (TRANS("Loop sample playback"));
    loopButton->setButtonText (TRANS("Loop"));
    loopButton->addListener (this);
    loopButton->setImages (false, true, true,
                            ImageCache::getFromMemory (BinaryData::loop_png, BinaryData::loop_pngSize),
                            1.000f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000));
    loopButton->setBounds (450, 60, 30, 30);
    loopAttachment.reset(new ButtonAttachment(stateManager, "loopMode", *loopButton));

    sampleViewer.reset (new SampleViewer(processor, processor, processor.getSampleManager()));
    addAndMakeVisible (sampleViewer.get());
    editor.addToGuiEditor (sampleViewer.get());
    sampleViewer->setName ("SampleViewer");
    sampleViewer->setBounds(Settings::SAMPLE_VIEWER_BOUNDS);
    // not visible until sample is loaded
    sampleViewer->setVisible(false);
    processor.addSampleInfoListener(sampleViewer.get());

    loadSampleTip.reset (new Label ("no sample loaded label",
                                    TRANS(Strings::NO_SAMPLE_LOADED_TEXT)));
    addAndMakeVisible (loadSampleTip.get());
    editor.addToGuiEditor (loadSampleTip.get());
    loadSampleTip->setFont (Font (27.90f, Font::plain));
    loadSampleTip->setJustificationType (Justification::centred);
    loadSampleTip->setEditable (false, false, false);
    loadSampleTip->setColour (Label::textColourId, Colour (0xff0a0000));

    loadSampleTip->setBounds (20, 136, 460, 56);

    pianoRoll.reset (new PianoRoll(processor, processor));
    pianoRoll->setBounds (Settings::PIANO_ROLL_BOUNDS);
    pianoRoll->setName ("piano roll component");
    processor.addSampleInfoListener(pianoRoll.get());

    pianoScroll.reset(new SoomplerScrollbar(false));
    pianoScroll->setRangeLimits(0, 200);
    pianoScroll->setCurrentRangeStart(Settings::PIANO_ROLL_RANGE_START);
    pianoScroll->addListener(pianoRoll.get());
    pianoScroll->setBounds(0, Settings::MAIN_PANEL_HEIGHT - Settings::PIANO_ROLL_SCROLL_HEIGHT, pianoRoll->getWidth(), Settings::PIANO_ROLL_SCROLL_HEIGHT);

    addAndMakeVisible (pianoRoll.get());
    addAndMakeVisible(pianoScroll.get());
    editor.addToGuiEditor (pianoRoll.get());
    editor.addToGuiEditor (pianoScroll.get());

    // adsr panel
    adsrPanel.reset(new AdsrPanel(stateManager, processor.getSampleManager()));
    adsrPanel->setPosition(150, 225);
    addAndMakeVisible(adsrPanel.get());

    // Reverse sample
    reverseButton.reset(new SoomplerToggleButton(TRANS("Reverse\n")));
    editor.addToGuiEditor(reverseButton.get());
    addAndMakeVisible(reverseButton.get());
    reverseButton->onClick = [this] {this->reverseButtonClicked();};
    //reverseAttachment.reset(new ButtonAttachment(stateManager, "reverse", *reverseButton));

    // place components like adsr panel
    auto miscControllsBaseline = adsrPanel->getY() + 25;
    volumeKnob->setPosition(20, miscControllsBaseline);
    glideKnob->setPosition(80, miscControllsBaseline);
    reverseButton->setBounds(380, miscControllsBaseline, 100, 50);


    // hide some controls until sample is loaded
    noSamplesLeft();
    
    // hide deprecated buttons
    openFileButton->setVisible(false);

    // connect knobs to listener
    volumeKnob->addListener(this);
    glideKnob->addListener(this);

    // add GUI editor last
    // it ensures that gui overlay will work properly
    editor.initOverlay();

    processor.addSampleInfoListener(this);
    
    restoreMainPanelState();
}

MainPanel::~MainPanel()
{
    loopAttachment = nullptr;

    volumeKnob = nullptr;
    openFileButton = nullptr;
    aboutButton = nullptr;
    sampleViewer = nullptr;
    loadSampleTip = nullptr;
    pianoRoll = nullptr;
    reverseButton = nullptr;
    loopButton = nullptr;
}

//==============================================================================
void MainPanel::paint (Graphics& g)
{
    g.fillAll (Colour (0xff323e44));
    g.drawImage(backgroundImage, Rectangle<float>(0, 0, this->getWidth(), this->getHeight()));

    editor.paintBackOverlay(g);

    // draw menu panel
    g.setGradientFill(Settings::MAIN_MENU_GRADIENT);
    g.fillRect(0, 0, this->getWidth(), Settings::MAIN_MENU_HEIGHT);

}

void MainPanel::resized()
{
}

void MainPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    if (sliderThatWasMoved == volumeKnob->getSlider())
    {
        processor.setVolume(volumeKnob->getValue());
        return;
    } else if (sliderThatWasMoved == glideKnob->getSlider())
    {
        processor.setGlide(glideKnob->getValue());
    }

}

void MainPanel::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == openFileButton.get())
    {
        openFileButtonClicked();
    }
    else if (buttonThatWasClicked == aboutButton.get())
    {
        aboutButtonClicked();
    }
    else if (buttonThatWasClicked == loopButton.get())
    {
        loopButtonClicked();
    }
    else if (buttonThatWasClicked == reverseButton.get()) {
        reverseButtonClicked();
    }
}

float MainPanel::getVolume() const
{
    return volumeKnob.get()->getValue();
}

void MainPanel::sampleChanged(std::shared_ptr<SampleInfo> info)
{
    adsrPanel->sampleChanged(info);

    volumeKnob->setValue(info->getVolume());
    reverseButton->setToggled(info->reversed);
}

void MainPanel::noSamplesLeft()
{
    loadSampleTip->setVisible(true);
    sampleViewer->setVisible(false);
    adsrPanel->setVisible(false);
    volumeKnob->setVisible(false);
    glideKnob->setVisible(false);
    reverseButton->setVisible(false);
    loopButton->setVisible(false);
    pianoRoll->setVisible(false);
    pianoScroll->setVisible(false);
}

void MainPanel::openFileButtonClicked()
{
    FileChooser chooser(Strings::OPEN_FILE_DIALOG_TEXT,
                        File::getSpecialLocation(File::userHomeDirectory),
                        "*.mp3;*.wav;*.ogg", false);

    if (chooser.browseForFileToOpen())
    {
        File sampleFile (chooser.getResult());

        processor.loadSample(sampleFile);

        //playButton->setEnabled(true);

        repaint();
    }
}

void MainPanel::aboutButtonClicked()
{
    auto editor = static_cast<SoomplerAudioProcessorEditor*>(processor.getActiveEditor());
    editor->showAboutSplash();
}

void MainPanel::playSampleButtonClicked()
{
    processor.playSample();
}

void MainPanel::stopSampleButtonClicked()
{
    processor.stopSamplePlayback();
}

void MainPanel::timerCallback()
{
    repaint();
}

void MainPanel::transportStateChanged(TransportState state)
{
    switch (state) {
    case NewFile:
        reverseButton->setToggled(false);
        loopButton->setToggled(false);
    case Ready:
        loadSampleTip->setVisible(false);
        pianoRoll->setVisible(true);
        pianoScroll->setVisible(true);
        sampleViewer->setVisible(true);
        adsrPanel->setVisible(true);
        volumeKnob->setVisible(true);
        glideKnob->setVisible(true);
        reverseButton->setVisible(true);
        loopButton->setVisible(true);
        reverseButton->setToggled(processor.isSampleReversed());
        loopButton->setToggled(processor.isLoopModeOn());
        // start gui updates
        startTimer(40);
        break;
    case Starting:
        break;
    case Stopped:
        break;
    default:
        break;
    }
}

void MainPanel::loopButtonClicked() {
    processor.setLoopEnabled(loopButton->isToggled());
}

void MainPanel::reverseButtonClicked() {
    processor.setSampleReversed(reverseButton->isToggle());
}

bool MainPanel::isInterestedInFileDrag(const juce::StringArray &files) {
    for (auto fileName : files) {
        for (auto format : Settings::SUPPORTED_FILE_FORMATS) {
            if (fileName.endsWith(format)) {
                return true;
            }
        }
    }
    
    return false;
}


void MainPanel::filesDropped(const juce::StringArray &files, int x, int y) {
    // by now only first dropped file will be loaded
    auto fileName = files[0];
    
    processor.loadSample(File(fileName));
}

void MainPanel::restoreMainPanelState() {
    // check if sample already loaded
    // (plugin reopened)
    auto sample = processor.getSampleManager()->getActiveSample();
    if (sample != nullptr && sample->thumbnail->getNumChannels() > 0) {
        // load current sample
        sampleViewer->sampleChanged(processor.getCurrentSampleInfo());
        pianoRoll->sampleChanged(processor.getCurrentSampleInfo());
        
        bool reversed = processor.isSampleReversed();
        bool looped = processor.isLoopModeOn();
        
        reverseButton->setToggled(reversed);
        loopButton->setToggled(looped);
        
        
        this->transportStateChanged(TransportState::Ready);
    }
}




