#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../MidiEventSupplier.h"
#include "../MidiEventConsumer.h"
#include <array>
#include "../SampleInfo.h"
#include "SoomplerImageButton.h"

class KeyInfo
{
public:
    float x;
    float width;
    float height;
    bool isWhite;
    int number;

    bool contains(Point<int> point)
    {
        Rectangle<int> rect(x, Settings::PIANO_ROLL_RANGE_MARKERS_HEIGHT, width, height);
        return rect.contains(point);
    }
};

enum MarkerType
{
    NO_MARKER,
    MIN_NOTE,
    MAX_NOTE,
    ROOT_NOTE
};

//==============================================================================
/**

*/
class PianoRoll  : public Component, public SampleChangeListener, public ScrollBar::Listener
{
public:
    PianoRoll (MidiEventSupplier& midiSupplier, MidiEventConsumer& midiConsumer);
    ~PianoRoll();

    void paint (Graphics& g) override;
    void resized() override;

    void sampleChanged(std::shared_ptr<SampleInfo> info) override;
    void noSamplesLeft();

    void scrollBarMoved(ScrollBar *scrollBarThatHasMoved, double newRangeStart);

private:
    static constexpr auto MAX_KEYS = 120;
    static constexpr auto FIRST_C_INDEX = 48;
    static constexpr auto C_0_INDEX = 12;

    MidiEventSupplier& midiSupplier;
    MidiEventConsumer& midiConsumer;

    // stores some metadata for keys
    std::array<KeyInfo, MAX_KEYS> keysInfo;

    std::shared_ptr<SampleInfo> sample;

    Path rootMarker;
    Path minMarker;
    Path maxMarker;

    // Which marker is dragged now
    MarkerType draggedMarker;

    // Keys from DEFAULT_KEY
    int offset;

    // Pianoroll window controls
//    std::unique_ptr<SoomplerImageButton> leftArrow;
//    std::unique_ptr<SoomplerImageButton> rightArrow;

    std::vector<int> getActiveMidiNotes();
    void calculateKeysInfo();
    void drawActiveNotes(Graphics& g, std::vector<int> activeNotes);
    int getKeyClicked(Point<int> point);

    void mouseDown(const MouseEvent& event) override;
    void mouseUp(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;
    void mouseMove(const MouseEvent& event) override;

    void drawBlackNotes(Graphics &g);
    void drawNoteTips(Graphics& g);

    void fireNoteOn(int noteNumber);
    void fireNoteOff(int noteNumber);

    void drawNoteRangeAndRoot(Graphics& g);
    void createMarkers(std::shared_ptr<SampleInfo> info);
    Path createMarker(int noteNum, bool root);

    void drawDisabledNotesMask(Graphics& g);
    void drawNoteDelimiters(Graphics& g);

    void rootMarkerDragged(Point<int> position);
    void minMarkerDragged(Point<int> position);
    void maxMarkerDragged(Point<int> position);

    int calculateIndexInPattern();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRoll)
};

