/*
  ==============================================================================

    SamplePreviewComponent.cpp
    Created: 9 Aug 2019 6:21:41pm
    Author:  artyom

  ==============================================================================
*/

#include "SamplePreviewComponent.h"

SamplePreviewComponent::SamplePreviewComponent(SoomplerAudioProcessor &processor) : FilePreviewComponent(), processor(processor),
    transportState(Stopped), autoplay(false)
{
    volume.reset(new SoomplerSlider(Slider::SliderStyle::LinearHorizontal, Slider::NoTextBox));
    autoplayButton.reset(new SoomplerToggleButton(TRANS("autoplay")));
    playButton.reset(new SoomplerImageButton(TRANS("playButton")));
    stopButton.reset(new SoomplerImageButton(TRANS("stopButton")));

    Image stopImage = ImageCache::getFromMemory(BinaryData::stop_png, BinaryData::stop_pngSize);

    playButton->setImages (false, true, true,
                           ImageCache::getFromMemory (BinaryData::play_png, BinaryData::play_pngSize),
                           1.000f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000));

    stopButton->setImages (false, true, true,
                           ImageCache::getFromMemory (BinaryData::stop_png, BinaryData::stop_pngSize),
                           1.000f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000));

    playButton->onClick = [this] { playButtonClicked(); };
    stopButton->onClick = [this] { stopButtonClicked(); };
    autoplayButton->onClick = [this] { setAutoplay(autoplayButton->isToggle());};
    volume->addListener(this);

    volume->setRange(0.0, 1.0);
    volume->setValue(0.5);

    FilePreviewComponent::addAndMakeVisible(volume.get());
    FilePreviewComponent::addAndMakeVisible(autoplayButton.get());
    FilePreviewComponent::addAndMakeVisible(stopButton.get());
    FilePreviewComponent::addAndMakeVisible(playButton.get());

    stopButton->setVisible(false);

    // transport source configuration
    transportSource.addChangeListener(this);
    transportSource.setGain(volume->getValue());

    // connect to processor
    processor.setSamplePreviewSource(this);
}

SamplePreviewComponent::~SamplePreviewComponent()
{
    processor.removeSamplePreviewSource();
    
    if (transportStopThread != nullptr) {
        transportStopThread->join();
    }
    if (readerSource != nullptr) {
        readerSource->releaseResources();
        //readerSource = nullptr;
    }
    transportSource.releaseResources();
}

void SamplePreviewComponent::selectedFileChanged(const File &newSelectedFile)
{
    // check if file is directory
    if (newSelectedFile.isDirectory()  || !newSelectedFile.exists()) {
        stopButtonClicked();
        return;
    }

    currentFile = std::make_unique<File>(File(newSelectedFile));

    if (autoplay) {
        playButtonClicked();
    }
}

void SamplePreviewComponent::resized()
{
    FlexBox fb;

    fb.flexDirection = FlexBox::Direction::row;

    fb.items.add(FlexItem(*(volume.get())).withFlex(3));
    if (stopButton->isVisible()) {
        fb.items.add(FlexItem(*(stopButton.get())).withFlex(1).withMaxHeight(30).withMaxWidth(30));
    }
    if (playButton->isVisible()) {
        fb.items.add(FlexItem(*(playButton.get())).withFlex(1).withMaxHeight(30).withMaxWidth(30));
    }
    fb.items.add(FlexItem(*(autoplayButton.get())).withFlex(1).withMargin(FlexItem::Margin(.0f, 20.0f, .0f, .0f)));

    fb.performLayout(FilePreviewComponent::getLocalBounds());
}

void SamplePreviewComponent::changeListenerCallback(ChangeBroadcaster *source)
{
    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
            changeState (Playing);
        else
            changeState (Stopped);
    }
}

void SamplePreviewComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void SamplePreviewComponent::releaseResources()
{
    //transportSource.releaseResources();
}

void SamplePreviewComponent::changeState (TransportState newState)
{
    if (transportState != newState)
    {
        transportState = newState;

        switch (transportState)
        {
            case Stopped:
                transportSource.setPosition(0.0);      
                stopButton->setVisible(false);
                playButton->setVisible(true);
                break;

            case Starting:
                playButton->setVisible(false);
                stopButton->setVisible(true);
                transportSource.start();
                break;

            case Playing:
                break;

            case Stopping:
                stopButton->setVisible(false);
                playButton->setVisible(true);
                stopTransportAsync();
                break;
        }

        resized();

    }
}

void SamplePreviewComponent::setFileAsTransportSource(File &file)
{
    auto reader = processor.getAudioFormatReader(file);

    if (reader == nullptr) {
        return;
    }

    auto newSource = std::make_unique<AudioFormatReaderSource>(reader, true);
    transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
    readerSource.reset(newSource.release());
}

void SamplePreviewComponent::sliderValueChanged(Slider *slider)
{
    if (slider == volume.get()) {
        transportSource.setGain(volume->getValue());
    }
}

void SamplePreviewComponent::stopTransportAsync()
{
    // join previous thread
    if (transportStopThread != nullptr) {
        transportStopThread->join();
    }
    transportStopThread = std::make_unique<std::thread>([this] {transportSource.stop();});
}

void SamplePreviewComponent::saveStateToMemory(StateBundle &bundle)
{
    bundle.addProperty(volume->getValue(), "previewVolume");
    bundle.addProperty(autoplayButton->isToggle(), "autoplayMode");
}

void SamplePreviewComponent::getStateFromMemory(StateBundle &bundle)
{
    if (bundle.hasProperty("previewVolume")) {
        auto volume = bundle.getProperty("previewVolume");
        this->volume->setValue(volume);
    }
    if (bundle.hasProperty("autoplayMode")) {
        auto autoplay = bundle.getProperty("autoplayMode");
        this->autoplayButton->setToggled(autoplay);
        this->autoplay = autoplay;
    }
}

void SamplePreviewComponent::setAutoplay(bool autoplay)
{
    this->autoplay = autoplay;
}

bool SamplePreviewComponent::isReady()
{
    return transportState == Starting || transportState == Playing;
}

void SamplePreviewComponent::getNextAudioBlock(AudioBuffer<float> &buffer)
{
    if (currentFile == nullptr || !transportSource.isPlaying()) {
        return;
    }

    AudioSourceChannelInfo audioSource;
    audioSource.buffer = &buffer;
    audioSource.startSample = 0;
    audioSource.numSamples = buffer.getNumSamples();

    transportSource.getNextAudioBlock (audioSource);
}

void SamplePreviewComponent::playButtonClicked()
{
    if (currentFile != nullptr) {
        setFileAsTransportSource(*(currentFile.get()));
        changeState(Starting);
    }
}

void SamplePreviewComponent::stopButtonClicked()
{
    changeState(Stopping);
}
