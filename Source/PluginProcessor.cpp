/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Strings.h"
#include "synth/ExtendedSampler.h"

// Private def
void createVoices(Synthesiser &synth, ChangeListener *listener);

//==============================================================================
SoomplerAudioProcessor::SoomplerAudioProcessor() : AudioProcessor (BusesProperties()
                                                   .withOutput ("Output", AudioChannelSet::stereo(), true)),
                                                   ChangeListener(),
                                                   stateManager(*this, nullptr, Identifier("SoomplerState"), createParametersLayout()),
                                                   currentSample(0),
                                                   thumbnailCache(5),
                                                   startSample(0),
                                                   endSample(0),
                                                   transportStateListener(nullptr),
                                                   previewSource(nullptr),
                                                   sampleManager(new SampleManager())
{
    UnitTestRunner testRunner;
    testRunner.runAllTests(2095382309);

    createVoices(synth, this);
    
    synth.setCurrentPlaybackSampleRate(44100);

    formatManager.registerBasicFormats();

    transportSource.addChangeListener(this);

    volume = *(stateManager.getRawParameterValue("volume"));

    ValueTree loadedSampleFullNameValue = ValueTree("loadedSample");
    ValueTree rootNoteValue = ValueTree("rootNote");
    ValueTree minNoteValue = ValueTree("minNote");
    ValueTree maxNoteValue = ValueTree("maxNote");

    stateManager.state.appendChild(loadedSampleFullNameValue, nullptr);
    stateManager.state.appendChild(rootNoteValue, nullptr);
    stateManager.state.appendChild(minNoteValue, nullptr);
    stateManager.state.appendChild(maxNoteValue, nullptr);

    addNewSaveableObject(sampleManager);
}

void createVoices(Synthesiser &synth, ChangeListener *listener) {
    for (int i = 0; i < Settings::DEFAULT_SYNTH_VOICES; i++) {
        synth.addVoice(new soompler::ExtendedVoice(listener));
    }
}

AudioProcessorValueTreeState::ParameterLayout SoomplerAudioProcessor::createParametersLayout()
{

    AudioProcessorValueTreeState::ParameterLayout parameterLayout = {
        std::make_unique<AudioParameterFloat>("volume", TRANS("Volume\n"), 0.0f, 1.0f, 0.5f),
        std::make_unique<AudioParameterFloat>("glide", TRANS("Glide\n"), 0.0f, 1.0f, 0.0f),
        std::make_unique<AudioParameterFloat>("attack", TRANS("Attack\n"), 0.0f, Settings::MAX_ATTACK_TIME, 0.0f),
        std::make_unique<AudioParameterFloat>("decay", TRANS("Decay\n"), 0.0f, Settings::MAX_DECAY_TIME, 0.0f),
        std::make_unique<AudioParameterFloat>("sustain", TRANS("Sustain\n"), 0.0f, 1.0f, 1.0f),
        std::make_unique<AudioParameterFloat>("release", TRANS("Release\n"), 0.0f, Settings::MAX_RELEASE_TIME, 0.0f),
        std::make_unique<AudioParameterBool>("reverse", TRANS("Reversed\n"), false),
        std::make_unique<AudioParameterBool>("loopMode", TRANS("Looped\n"), false)
    };
    
    return parameterLayout;
}

SoomplerAudioProcessor::~SoomplerAudioProcessor()
{
    transportSource.releaseResources();
    transportStateListener = nullptr;
}

//==============================================================================
const String SoomplerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SoomplerAudioProcessor::acceptsMidi() const
{
    return true;
}

bool SoomplerAudioProcessor::producesMidi() const
{
    return false;
}

bool SoomplerAudioProcessor::isMidiEffect() const
{
    return false;
}

double SoomplerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SoomplerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SoomplerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SoomplerAudioProcessor::setCurrentProgram (int index)
{
}

const String SoomplerAudioProcessor::getProgramName (int index)
{
    return {};
}

void SoomplerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SoomplerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    transportSource.prepareToPlay(samplesPerBlock, sampleRate);
    synth.setCurrentPlaybackSampleRate(sampleRate);
    if (previewSource != nullptr) {
        previewSource->prepareToPlay(samplesPerBlock, sampleRate);
    }
}

void SoomplerAudioProcessor::releaseResources()
{
    transportSource.releaseResources();
//    if (previewSource != nullptr) {
//        previewSource->releaseResources();
//    }
}

bool SoomplerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    return true;
}

void SoomplerAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    if (previewSource != nullptr && previewSource->isReady()) {
        previewSource->getNextAudioBlock(buffer);
        return;
    }
    
    if (readerSource.get() == nullptr) {
        buffer.clear();
        return;
    }

    if (midiMessages.isEmpty() && !synth.getVoice(0)->isVoiceActive()) {
        processTransport(buffer);
        return;
    }

    lastMidiEvents = filterMidiMessagesForChannel(midiMessages, 1);
    auto audioBusBuffer = getBusBuffer(buffer, false, 0);

    synth.renderNextBlock(audioBusBuffer, lastMidiEvents, 0, audioBusBuffer.getNumSamples());
}

void SoomplerAudioProcessor::processTransport(AudioBuffer<float>& buffer)
{
    AudioSourceChannelInfo audioSource;
    audioSource.buffer = &buffer;
    audioSource.startSample = 0;
    audioSource.numSamples = buffer.getNumSamples();

    transportSource.getNextAudioBlock(audioSource);
}

std::shared_ptr<File> SoomplerAudioProcessor::getLoadedSample() const {
    return loadedSample;
}

void SoomplerAudioProcessor::setSampleStartPosition(int64 sample)
{
    startSample = sample;
    transportSource.setNextReadPosition(startSample);

    auto voice = static_cast<soompler::ExtendedVoice*> (synth.getVoice(0));
    voice->setStartSample(sample);

    stateManager.state.setProperty("startSample", startSample, nullptr);
}

void SoomplerAudioProcessor::setSampleEndPosition(int64 sample)
{
    endSample = sample;

    auto voice = static_cast<soompler::ExtendedVoice*> (synth.getVoice(0));
    voice->setEndSample(sample);

    stateManager.state.setProperty("endSample", endSample, nullptr);
}

void SoomplerAudioProcessor::sampleChanged(std::shared_ptr<SampleInfo> info)
{
    sampleManager->sampleChanged(info);
    setSampleStartPosition(info->startSample);
    setSampleEndPosition(info->endSample);
}

void SoomplerAudioProcessor::setVolume(double volume)
{
    this->volume = volume;
    transportSource.setGain(volume);


//    auto voice = static_cast<soompler::ExtendedVoice*>(synth.getVoice(0));
//    voice->setVolume(volume);

    sampleManager->getActiveSample()->setVolume(volume);
}

void SoomplerAudioProcessor::setGlide(double glide)
{
    sampleManager->getActiveSample()->sound->setGlide(glide);
}

void SoomplerAudioProcessor::notifyTransportStateChanged(TransportState state)
{
    if (transportStateListener == nullptr) {
        return;
    }

    transportStateListener->transportStateChanged(state);
}

std::vector<int> SoomplerAudioProcessor::getActiveNotes()
{
    std::vector<int> result;

    // TODO replace when multi-voice synth is realized
    int noteNumber = synth.getVoice(0)->getCurrentlyPlayingNote();

    if (noteNumber >= 0) {
        result.push_back(noteNumber);
    }

    return result;
}

void SoomplerAudioProcessor::noteOn(int noteNumber)
{
    if (synth.getSound(0) == nullptr) {
        return;
    }

    synth.noteOn(0, noteNumber, 1.0f);
}

void SoomplerAudioProcessor::noteOff(int noteNumber)
{
    synth.noteOff(0, noteNumber, getFloatParameter("volume"), true);
}

void SoomplerAudioProcessor::playOrStopRootNote()
{
    auto activeSample = sampleManager->getActiveSample();
    if (activeSample == nullptr) {
        return;
    }
    if (synth.getVoice(0)->isVoiceActive()) {
        synth.allNotesOff(0, 0);
    } else {
        synth.noteOn(0, activeSample->rootNote, getFloatParameter("volume"));
    }
}

void SoomplerAudioProcessor::setRootNote(int rootNote)
{
    auto sound = static_cast<soompler::ExtendedSound*> (synth.getSound(0).get());
    sound->setRootNote(rootNote);

    stateManager.state.setProperty("rootNote", rootNote, nullptr);
}

void SoomplerAudioProcessor::setNoteRange(int minNote, int maxNote)
{
    auto range = BigInteger();
    range.setRange(minNote, maxNote - minNote + 1, true);

    auto sound = static_cast<soompler::ExtendedSound*> (synth.getSound(0).get());
    sound->setMidiRange(range);

    stateManager.state.setProperty("minNote", minNote, nullptr);
    stateManager.state.setProperty("maxNote", maxNote, nullptr);
}

MidiBuffer SoomplerAudioProcessor::filterMidiMessagesForChannel(const MidiBuffer &input, int channel)
{
    MidiBuffer output;
    int samplePosition;
    MidiMessage msg;

    for (MidiBuffer::Iterator it(input); it.getNextEvent(msg, samplePosition);)
    {
        if (msg.getChannel() == channel) output.addEvent(msg, samplePosition);
    }

    return output;
}

void SoomplerAudioProcessor::notifySampleInfoListeners()
{
    sampleManager->notifySampleInfoListeners();
}

//==============================================================================
bool SoomplerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SoomplerAudioProcessor::createEditor()
{
    return new SoomplerAudioProcessorEditor (*this);
}

//==============================================================================
void SoomplerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    saveState();

    // save state
    ValueTree loadedSampleNameValue = ValueTree("loadedSampleName");
    ValueTree loadedSampleLengthValue = ValueTree("loadedSampleLength");
    ValueTree startSampleValue = ValueTree("startSample");
    ValueTree endSampleValue = ValueTree("endSample");

    stateManager.state.appendChild(loadedSampleNameValue, nullptr);
    stateManager.state.appendChild(loadedSampleLengthValue, nullptr);
    stateManager.state.appendChild(startSampleValue, nullptr);
    stateManager.state.appendChild(endSampleValue, nullptr);

    if (loadedSample != nullptr) {
        stateManager.state.setProperty("loadedSampleName", loadedSample->getFileName(), nullptr);
    }
    
    stateManager.state.setProperty("startSample", startSample, nullptr);
    stateManager.state.setProperty("endSample", endSample, nullptr);

    auto state = stateManager.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SoomplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes - sizeof(bool)));

    if (xmlState.get() == nullptr) {
        return;
    }

    if (xmlState->hasTagName(stateManager.state.getType())) {
        stateManager.replaceState(ValueTree::fromXml(*xmlState));
    }

    auto bundle = StateBundle(stateManager.state);
    // Restore saveable objects
    for (auto saveable : objectsToSave) {
        saveable->getStateFromMemory(bundle);
    }

    // restore loaded samples
    for (auto sample : sampleManager->getAllSamples()) {
        loadThumbnailAndSoundFor(sample);
    }

    bool looping = stateManager.state.getPropertyAsValue("loopMode", nullptr).getValue();
    setLoopEnabled(looping);

//    // restore variables
//    String sampleName = stateManager.state.getPropertyAsValue("loadedSampleName", nullptr).getValue();
//    String fullSamplePath = stateManager.state.getPropertyAsValue("loadedSample", nullptr).getValue();
//    float sampleLength = stateManager.state.getPropertyAsValue("loadedSampleLength", nullptr).getValue();
//    int64 startSample = stateManager.state.getPropertyAsValue("startSample", nullptr).getValue();
//    int64 endSample = stateManager.state.getPropertyAsValue("endSample", nullptr).getValue();
//    bool reverse = stateManager.state.getPropertyAsValue("reverse", nullptr).getValue();
//    int rootNote = stateManager.state.getPropertyAsValue("rootNote", nullptr).getValue();
//    int minNote = stateManager.state.getPropertyAsValue("minNote", nullptr).getValue();
//    int maxNote = stateManager.state.getPropertyAsValue("maxNote", nullptr).getValue();

//    // sample wasn't loaded
//    if (fullSamplePath.isEmpty()) {
//        return;
//    }
    
//    if (!isSampleLoaded() && fullSamplePath.isNotEmpty()) {
//        loadSample(File(fullSamplePath), true);
//        setSampleReversed(reverse);
//        setLoopEnabled(looping);
//    }

//    auto sampleInfo = sampleManager.getActiveSample();
//    sampleInfo->startSample = startSample;
//    sampleInfo->endSample = endSample;
//    this->startSample = startSample;
//    this->endSample = endSample;

//    setSampleStartPosition(startSample);
//    setSampleEndPosition(endSample);

//    if (rootNote != 0) {
//        sampleInfo->rootNote = rootNote;
//        setRootNote(rootNote);
//    } else if (minNote != 0 && maxNote != 0) {
//        sampleInfo->minNote = minNote;
//        sampleInfo->maxNote = maxNote;

//        setNoteRange(minNote, maxNote);
//    }
    
//    restoreKnobParameters();

//    notifySampleInfoListeners();
}

void SoomplerAudioProcessor::restoreKnobParameters() {
//    if (synth.getSound(0) == nullptr) {
//        return;
//    }
//    for (auto sample : sampleManager.getAllSamples()) {
//        sample->sound->setAdsrParams(sample->getAdsr());
//        sample->sound-
//    }
//    // set current adsr params
//    auto adsr = ADSR::Parameters();
//    adsr.attack = getFloatParameter("attack");
//    adsr.decay = getFloatParameter("decay");
//    adsr.sustain = getFloatParameter("sustain");
//    adsr.release = getFloatParameter("release");
    
//    auto sound = static_cast<soompler::ExtendedSound*>(synth.getSound(0).get());
//    sound->setAdsrParams(adsr);
}

void SoomplerAudioProcessor::loadSample(const File& sampleFile, bool reload)
{
    auto sound = getSampleData(std::make_shared<File>(sampleFile));
    if (sound == nullptr) {
        return;
    }
    
    this->loadedSample = std::make_shared<File>(sampleFile);

    synth.addSound(sound);

    setSampleStartPosition(0);
    setSampleEndPosition(transportSource.getTotalLength());
    
    if (!reload) {
        setSampleReversed(false);
        setLoopEnabled(false);
    }
    
    notifyTransportStateChanged(TransportState::Ready);

    stateManager.state.setProperty("loadedSample", var(sampleFile.getFullPathName()), nullptr);

    auto voice = static_cast<soompler::ExtendedVoice*>(synth.getVoice(0));

    auto sampleInfo = std::make_shared<SampleInfo>(transportSource.getLengthInSeconds(), voice->getSampleRate(), sampleFile.getFileName(), sampleFile.getFullPathName());
    sampleInfo->thumbnail = thumbnail;
    sampleInfo->sound = sound;
    sampleManager->sampleChanged(sampleInfo);

    notifySampleInfoListeners();
}

bool SoomplerAudioProcessor::isSampleLoaded()
{
    return sampleManager->getActiveSample() != nullptr;
}

void SoomplerAudioProcessor::fileRecieved(const File &file)
{
    loadSample(file);
}

void SoomplerAudioProcessor::playSample()
{
    if (loadedSample == nullptr)
    {
        DBG("sample is not loaded and cannot be played");
        return;
    }

    changeTransportState(Starting);
}

void SoomplerAudioProcessor::stopSamplePlayback()
{
    changeTransportState(Stopping);
}

void SoomplerAudioProcessor::setTransportStateListener(TransportStateListener* listener)
{
    this->transportStateListener = listener;
}

double SoomplerAudioProcessor::getCurrentAudioPosition()
{
    if (synth.getVoice(0)->isVoiceActive()) {
        return getSynthCurrentPosition();
    }
    return transportSource.getCurrentPosition();
}

soompler::ExtendedSound* SoomplerAudioProcessor::getSampleData(std::shared_ptr<File> sampleFile)
{
    if (sampleFile == nullptr) {
        return nullptr;
    }

    auto formatReader = getAudioFormatReader(*(sampleFile.get()));
    
    // check sample is not too long
    auto length = formatReader->lengthInSamples / formatReader->sampleRate;
    if (length >= Settings::MAX_SAMPLE_LENGTH) {
        AlertWindow dialog("Warning", Strings::SAMPLE_LENGTH_TOO_LONG, AlertWindow::AlertIconType::WarningIcon, this->getActiveEditor());
        dialog.addButton("Ok", 0);
        dialog.runModalLoop();
        delete formatReader;
        
        return nullptr;
    }

    setTransportSource(formatReader);

    thumbnail = std::make_shared<SAudioThumbnail>(Settings::THUMBNAIL_RESOLUTION_SAMPLES, formatManager, thumbnailCache);
    thumbnail->setSource(new FileInputSource(*sampleFile));

    BigInteger midiNotes;
    midiNotes.setRange(Settings::DEFAULT_MIN_NOTE, Settings::DEFAULT_MAX_NOTE - Settings::DEFAULT_MIN_NOTE + 1, true);
    return new soompler::ExtendedSound(sampleFile->getFileName(), *formatReader, midiNotes, Settings::DEFAULT_ROOT_NOTE,
                                       Settings::DEFAULT_ATTACK_TIME, Settings::DEFAULT_RELEASE_TIME, Settings::MAX_SAMPLE_LENGTH);
}

void SoomplerAudioProcessor::setSamplePreviewSource(SamplePreviewSource* source)
{
    this->previewSource = source;
    previewSource->prepareToPlay(getBlockSize(), getSampleRate());
}

AudioFormatManager &SoomplerAudioProcessor::getFormatManager()
{
    return formatManager;
}

AudioFormatReader *SoomplerAudioProcessor::getAudioFormatReader(const File &file)
{
    AudioFormat* format = getFormatForFileOrNullptr(file);
    if (format == nullptr)
    {
        return nullptr;
    }

    return formatManager.createReaderFor(file);
}

void SoomplerAudioProcessor::addNewSaveableObject(std::shared_ptr<SaveableState> saveable)
{
    objectsToSave.push_back(saveable);
    StateBundle bundle(stateManager.state);
    saveable->getStateFromMemory(bundle);
}

void SoomplerAudioProcessor::saveStateAndReleaseObjects() {
    saveState();
    // delete objects from saveStateList cos they gonna be destroyed
    // for some reason std::shared_ptr doesn't resolve problem at all
    objectsToSave.clear();
}

void SoomplerAudioProcessor::addSampleInfoListener(SampleChangeListener* listener)
{
    sampleManager->addSampleInfoListener(listener);
}

std::shared_ptr<SampleManager> SoomplerAudioProcessor::getSampleManager()
{
    return sampleManager;
}

void SoomplerAudioProcessor::loadThumbnailAndSoundFor(std::shared_ptr<SampleInfo> sample)
{
    auto sound = getSampleData(std::make_shared<File>(sample->samplePath));
    if (sound == nullptr) {
        DBG("Cannot recover sample");
        DBG(sample->sampleName);
        sampleManager->removeSample(sample);
        return;
    }
    sample->sound = sound;
    sample->thumbnail = thumbnail;

    // reverse if needed
    if (sample->reversed) {
        sample->sound->setReversed(true);
        thumbnail->reverse();
    }

    synth.addSound(sound);
}

void SoomplerAudioProcessor::saveState()
{
    auto bundle = StateBundle(stateManager.state);
    // Save saveable objects
    for (auto saveable : objectsToSave) {
        saveable->saveStateToMemory(bundle);
    }
}

AudioFormat *SoomplerAudioProcessor::getFormatForFileOrNullptr(const File &sampleFile)
{
    AudioFormat* format = formatManager.findFormatForFileExtension(sampleFile.getFileExtension());

    if (format == nullptr) {
        DBG(sampleFile.getFileExtension());
        NativeMessageBox::showMessageBox(
                    AlertWindow::AlertIconType::WarningIcon,
                    Strings::UNSUPPORTED_FILE_EXTENSION_ERROR_TITLE,
                    Strings::UNSUPPORTED_FILE_EXTENSION_ERROR_MESSAGE);
    }

    return format;
}

void SoomplerAudioProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
        {
            changeTransportState(Playing);
        } else {
            changeTransportState(Stopped);
        }
    }
}

void SoomplerAudioProcessor::changeTransportState(TransportState newState)
{
    if (transportState == newState)
    {
        return;
    }


    transportState = newState;

    notifyTransportStateChanged(newState);

    switch (transportState)
    {
        case Playing:
            break;
        case Starting:
            synth.getVoice(0)->stopNote(0, false);
            transportSource.start();
            break;
        case Stopping:
        case Stopped:
            transportSource.stop();
            transportSource.setNextReadPosition(startSample);
            break;
        default:
            break;
   }
}


void SoomplerAudioProcessor::setTransportSource(AudioFormatReader* reader)
{
    auto newSource = std::make_unique<AudioFormatReaderSource>(reader, true);
    transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
    readerSource.reset(newSource.release());
}

double SoomplerAudioProcessor::getSynthCurrentPosition()
{
    auto voice = static_cast<soompler::ExtendedVoice*>(synth.getVoice(0));
    return voice->getCurrentPosition();
}

std::shared_ptr<TransportInfo> SoomplerAudioProcessor::getTransportInfo()
{
    auto info = std::make_shared<TransportInfo>(this->getSampleRate());
    info->setAudioPosition(getCurrentAudioPosition());

    return info;
}

void SoomplerAudioProcessor::setAdsrParams(ADSR::Parameters params)
{
    auto sound = static_cast<soompler::ExtendedSound*>(synth.getSound(0).get());
    if (sound == nullptr) {
        return;
    }

    sound->setAdsrParams(params);

    sampleManager->getActiveSample()->getAdsr() = params;
}

void SoomplerAudioProcessor::setLoopEnabled(bool loopEnable) {
    for (int i = 0; i < synth.getNumVoices(); i++) {
        auto voice = static_cast<soompler::ExtendedVoice*> (synth.getVoice(i));
        voice->enableLooping(loopEnable);
    }

    stateManager.state.setProperty("loopMode", loopEnable, nullptr);
}

void SoomplerAudioProcessor::setSampleReversed(bool reversed)
{
    auto sample = sampleManager->getActiveSample();
    if (sample == nullptr) {
        return;
    }
    auto sound = sample->sound;
    if (sound == nullptr) {
        return;
    }

    sound->setReversed(reversed);
    thumbnail->setReversed(reversed);

    sample->reversed = reversed;
}

AudioProcessorValueTreeState &SoomplerAudioProcessor::getStateManager()
{
    return stateManager;
}

float SoomplerAudioProcessor::getFloatParameter(const String &paramId)
{
    return *(stateManager.getRawParameterValue(paramId));
}

std::shared_ptr<SampleInfo> SoomplerAudioProcessor::getCurrentSampleInfo()
{
    return sampleManager->getActiveSample();
}

bool SoomplerAudioProcessor::isLoopModeOn() const {
    return stateManager.state.getProperty("loopMode", false);
}


bool SoomplerAudioProcessor::isSampleReversed() {
    return stateManager.state.getPropertyAsValue("reverse", nullptr).getValue();
}

void SoomplerAudioProcessor::removeSamplePreviewSource() {
    previewSource = nullptr;
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SoomplerAudioProcessor();
}
