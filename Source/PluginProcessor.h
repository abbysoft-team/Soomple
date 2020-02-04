/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "TransportStateListener.h"
#include "MidiEventSupplier.h"
#include "MidiEventConsumer.h"
#include "TransportInfo.h"
#include "SampleInfo.h"
#include "gui/SAudioThumbnail.h"
#include "FileListener.h"
#include "browser/SamplePreviewSource.h"
#include "SaveableState.h"
#include "synth/SoomplerSynth.h"
#include "SampleManager.h"
#include "synth/ExtendedSampler.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

//==============================================================================
/**
*/
class SoomplerAudioProcessor  :
        public AudioProcessor,
        ChangeListener,
        public MidiEventSupplier,
        public MidiEventConsumer,
        public TransportInfoOwner,
        public SampleChangeListener,
        public FileListener
{
public:
    
    //==============================================================================
    SoomplerAudioProcessor();
    ~SoomplerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    std::shared_ptr<File> getLoadedSample() const;

    void loadSample(const File& file, bool reload = false);

    void playSample();

    void stopSamplePlayback();

    void setTransportStateListener(TransportStateListener*);

    double getCurrentAudioPosition();

    int64 getTotalLengthOfSample() const {
        return transportSource.getTotalLength();
    }

//    void updateTransportState();

    void processTransport(AudioBuffer<float>& buffer);

    // percent of a sample length
    void setSampleStartPosition(int64 sample);
    void setSampleEndPosition(int64 sample);
    void sampleChanged(std::shared_ptr<SampleInfo> info) override;

    void setVolume(double volume);
    void setGlide(double glide);

    void notifyTransportStateChanged(TransportState state);

    std::vector<int> getActiveNotes() override;

    void noteOn(int noteNumber) override;
    void noteOff(int noteNumber) override;
    void playOrStopRootNote();
    void setRootNote(int rootNote) override;
    void setNoteRange(int minNote, int maxNote) override;

    std::shared_ptr<TransportInfo> getTransportInfo() override;

    void setAdsrParams(ADSR::Parameters params);
    
    void setLoopEnabled(bool loopEnable);

    void setSampleReversed(bool reverse);

    AudioProcessorValueTreeState& getStateManager();

    float getFloatParameter(const String& paramId);

    std::shared_ptr<SampleInfo> getCurrentSampleInfo();
    
    bool isSampleReversed();
    bool isLoopModeOn() const;

    bool isSampleLoaded();

    void fileRecieved(const File& file);

    void setFileAsTransportSource(AudioTransportSource &source, File &file);

    void setSamplePreviewSource(SamplePreviewSource* source);
    void removeSamplePreviewSource();

    AudioFormatManager& getFormatManager();
    AudioFormat* getFormatForFileOrNullptr(const File &sampleFile);
    AudioFormatReader* getAudioFormatReader(const File &file);

    void addNewSaveableObject(std::shared_ptr<SaveableState> saveable);
    void saveState();
    void saveStateAndReleaseObjects();
    
    void addSampleInfoListener(SampleChangeListener* listener);

    std::shared_ptr<SampleManager> getSampleManager();

    void loadThumbnailAndSoundFor(std::shared_ptr<SampleInfo> sample);
private:
    //==============================================================================

    std::vector<std::shared_ptr<SaveableState>> objectsToSave;

    AudioProcessorValueTreeState stateManager;

    std::shared_ptr<File> loadedSample;
    SoomplerSynth synth;
    int currentSample;

    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    TransportState transportState;
    TransportStateListener* transportStateListener;

    AudioThumbnailCache thumbnailCache;
    std::shared_ptr<SAudioThumbnail> thumbnail;

    int64 startSample;
    int64 endSample;

    MidiBuffer lastMidiEvents;

    float volume;

    std::shared_ptr<SampleManager> sampleManager;
//    std::vector<std::shared_ptr<SampleInfoListener>> sampleInfoListeners;

    SamplePreviewSource *previewSource;

    AudioProcessorValueTreeState::ParameterLayout createParametersLayout();

    soompler::ExtendedSound* getSampleData(std::shared_ptr<File> sampleFile);
    void changeListenerCallback(ChangeBroadcaster* source) override;
    void changeTransportState(TransportState newState);
    void setTransportSource(AudioFormatReader*);
    double getSynthCurrentPosition();
    MidiBuffer filterMidiMessagesForChannel(const MidiBuffer &input, int channel);
    void notifySampleInfoListeners();
    void restoreKnobParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoomplerAudioProcessor)
};
