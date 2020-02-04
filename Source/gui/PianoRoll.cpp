#include "PianoRoll.h"
#include "../Settings.h"

void drawNoteTip(Graphics &g, int cNumber, int x);
//void drawBlackNotes(Graphics& g);
void drawActiveNoteMask(int x, int width, int height, Graphics &g);
void drawBlackNote(int coord, Graphics& g);

PianoRoll::PianoRoll (MidiEventSupplier& midiSupplier, MidiEventConsumer& midiConsumer)
    : midiSupplier(midiSupplier), midiConsumer(midiConsumer), offset(0)
{
    draggedMarker = NO_MARKER;

    calculateKeysInfo();
}

void PianoRoll::calculateKeysInfo()
{
    constexpr auto blackKeyOffset = Settings::PIANO_ROLL_BLACK_NOTE_WIDTH / 2.0;
    constexpr auto whiteKeyOffset = Settings::PIANO_ROLL_WHITE_NOTE_WIDTH;

    // structure of keys 0 - white key 1 - black
    bool blackKeyPattern[] = {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0};

    auto currentIndex = 0;
    auto nextWhiteKeyX = 0;
    // init only keys after first key on the screen
    for (int i = 0; i < MAX_KEYS; i++) {
        // cycling through pattern array
        if (currentIndex == 12) {
            currentIndex = 0;
        }


        if (blackKeyPattern[currentIndex]) {
            KeyInfo nextKey;
            nextKey.height = Settings::PIANO_ROLL_BLACK_NOTE_HEIGHT;
            nextKey.width = Settings::PIANO_ROLL_BLACK_NOTE_WIDTH;
            nextKey.x = nextWhiteKeyX - blackKeyOffset;
            nextKey.isWhite = false;
            nextKey.number = i;

            keysInfo[i] = nextKey;
        } else {
            KeyInfo nextKey;
            nextKey.height = Settings::PIANO_ROLL_HEIGHT;
            nextKey.width = whiteKeyOffset;
            nextKey.isWhite = true;
            nextKey.number = i;
            nextKey.x = nextWhiteKeyX;

            keysInfo[i] = nextKey;

            nextWhiteKeyX += whiteKeyOffset;
        }

        currentIndex++;
    }
}

int PianoRoll::calculateIndexInPattern()
{
    if (offset == 0) {
        return 0;
    }

    if (offset > 0) {
        return offset % 12;
    }

    int index = 0;
    if (offset < 0) {
        for (int i = 0; i < abs(offset); i++) {
            index--;
            if (index < 0) {
                index == 11;
            }
        }
        return index;
    }
}

PianoRoll::~PianoRoll()
{
}

//==============================================================================
void PianoRoll::paint (Graphics& g)
{
    // move start Y for few pixels so we can draw markers at the end
    auto transform = AffineTransform();
    transform = transform.translated(0, Settings::PIANO_ROLL_RANGE_MARKERS_HEIGHT);
    g.addTransform(transform);

    g.setColour(Settings::PIANO_ROLL_WHITE_COLOR);
    g.setGradientFill(Settings::PIANO_ROLL_GRADIENT);
    g.fillRect(0, 0, Settings::PIANO_ROLL_WIDTH, Settings::PIANO_ROLL_HEIGHT);

    drawNoteDelimiters(g);
    drawBlackNotes(g);
    drawActiveNotes(g, getActiveMidiNotes());
    drawNoteTips(g);

    // no sample loaded yet, dont draw markers
    if (sample == nullptr) {
        return;
    }

    drawDisabledNotesMask(g);

    // restore start Y
    transform = transform.translated(0, -1 * Settings::PIANO_ROLL_RANGE_MARKERS_HEIGHT);
    g.addTransform(transform);
    drawNoteRangeAndRoot(g);
}

void PianoRoll::drawNoteDelimiters(Graphics& g)
{
    g.setColour(Settings::PIANO_ROLL_DELIMITER_COLOR);

    // end of all keys
    g.drawLine(0, 0, Settings::PIANO_ROLL_WIDTH - 1, 0, 1);

    auto firstWhiteIndex = FIRST_C_INDEX + offset;
    while (!keysInfo[firstWhiteIndex].isWhite) {
        firstWhiteIndex++;
    }

    auto currentDelimiterX = keysInfo[firstWhiteIndex].x - keysInfo[FIRST_C_INDEX + offset].x;
    while (currentDelimiterX < Settings::PIANO_ROLL_WIDTH) {
        g.setColour(Settings::PIANO_ROLL_DELIMITER_COLOR);
        g.drawLine(currentDelimiterX, 0, currentDelimiterX, Settings::PIANO_ROLL_HEIGHT-1, 1);
        // piano shadow line color 1
        g.setColour(Colour(230, 230, 230));
        g.drawLine(currentDelimiterX - 1, 0, currentDelimiterX - 1, Settings::PIANO_ROLL_HEIGHT - 1, 1);
        // piano shadow line color 2
        g.setColour(Colour(215, 215, 215));
        g.drawLine(currentDelimiterX + 1, 0, currentDelimiterX + 1, Settings::PIANO_ROLL_HEIGHT - 1, 1);

        currentDelimiterX += Settings::PIANO_ROLL_WHITE_NOTE_WIDTH;
    }
}

void PianoRoll::drawBlackNotes(Graphics &g)
{
//    // pattern from C#
//    int blackKeyPattern[] = { 2, 1, 2, 1, 1 };
//    static constexpr auto blackKeyOffset = Settings::PIANO_ROLL_WHITE_NOTE_WIDTH;

//    //g.setGradientFill(Settings::PIANO_ROLL_BLACK_KEY_GRADIENT);

//    auto currentBlackKeyCoord = Settings::PIANO_ROLL_WHITE_NOTE_WIDTH - Settings::PIANO_ROLL_BLACK_NOTE_WIDTH / 2;
//    auto currentPatternIndex = 0;
//    while (currentBlackKeyCoord < Settings::PIANO_ROLL_WIDTH) {
//        drawBlackNote(currentBlackKeyCoord, g);

//        // cycling through pattern array
//        // currentIndex+1 used, so index 4 is out of bounds
//        if (currentPatternIndex == 4) {
//            currentPatternIndex = -1;
//        }

//        currentBlackKeyCoord += blackKeyOffset * blackKeyPattern[++currentPatternIndex];
//    }

    auto firstKeyIndex = Settings::DEFAULT_FIRST_KEY + offset;
    auto xOffset = keysInfo[firstKeyIndex].x;
    for (int i = firstKeyIndex; i < keysInfo.size(); i++) {
        auto key = keysInfo[i];
        auto x = key.x - xOffset;

        if (key.isWhite) {
            continue;
        }
        if (x > getWidth()) {
            return;
        }

        drawBlackNote(x, g);
    }
}

void drawBlackNote(int coord, Graphics &g)
{
    static auto shapeOffsetX = Settings::PIANO_ROLL_BLACK_NOTE_WIDTH * 0.12;
    static auto shapeOffsetWidth = Settings::PIANO_ROLL_BLACK_NOTE_WIDTH - shapeOffsetX * 2;
    static auto shapeOffsetY = Settings::PIANO_ROLL_BLACK_NOTE_HEIGHT * 0.08;
    static auto shapeOffsetHeight = Settings::PIANO_ROLL_BLACK_NOTE_HEIGHT - shapeOffsetY;

    g.setColour(Settings::PIANO_ROLL_BLACK_NOTE_COLOR);
    g.fillRect(coord, 0, Settings::PIANO_ROLL_BLACK_NOTE_WIDTH, Settings::PIANO_ROLL_BLACK_NOTE_HEIGHT);
    g.setColour(Settings::PIANO_ROLL_DELIMITER_COLOR);

    // small spaces between black and white keys
    g.drawLine(coord - 1, 0, coord -1, Settings::PIANO_ROLL_BLACK_NOTE_HEIGHT + 1);
    g.drawLine(coord + Settings::PIANO_ROLL_BLACK_NOTE_WIDTH + 1, 0,
               coord + Settings::PIANO_ROLL_BLACK_NOTE_WIDTH + 1, Settings::PIANO_ROLL_BLACK_NOTE_HEIGHT + 1);

    // 3d shape of black keys
    g.setColour(Settings::PIANO_ROLL_BLACK_NOTE_TOP_COLOR);
    g.fillRect((float) coord + shapeOffsetX, 0.f, (float) shapeOffsetWidth, (float) shapeOffsetHeight);

    g.setColour(Settings::PIANO_ROLL_BLACK_NOTE_HIGHLIGHTS_COLOR);
    auto shapeLineX = coord + Settings::PIANO_ROLL_BLACK_NOTE_WIDTH - shapeOffsetX;

    g.drawLine(shapeLineX, 0, shapeLineX, Settings::PIANO_ROLL_BLACK_NOTE_HEIGHT - shapeOffsetY);
    g.drawLine(coord + shapeOffsetX, Settings::PIANO_ROLL_BLACK_NOTE_HEIGHT - shapeOffsetY,
               coord + shapeOffsetX + shapeOffsetWidth, Settings::PIANO_ROLL_BLACK_NOTE_HEIGHT - shapeOffsetY);


}

void PianoRoll::drawActiveNotes(Graphics& g, std::vector<int> activeNotes)
{
    auto minNoteNumber = FIRST_C_INDEX + offset;

    float keyCoordX = 0;
    for (auto noteNumber : activeNotes) {
        if (noteNumber < minNoteNumber) {
            continue;
        }

        keyCoordX = keysInfo[noteNumber].x - keysInfo[minNoteNumber].x;
        if (keyCoordX > Settings::PIANO_ROLL_WIDTH) {
            continue;
        }

        drawActiveNoteMask(keyCoordX, keysInfo[noteNumber].width, keysInfo[noteNumber].height, g);
    }
}

void PianoRoll::mouseDown(const MouseEvent &event)
{
    auto position = event.getPosition();
    auto keyNumber = getKeyClicked(position);
    if (keyNumber != -1) {
        midiConsumer.noteOn(keyNumber);
    }

    if (sample == nullptr) {
        return;
    }

    // markers have negative Y because of render way, so here we compensate it
    auto positionForMarkers = Point<float>(position.x, position.y - Settings::PIANO_ROLL_RANGE_MARKERS_HEIGHT);

    if (rootMarker.contains(positionForMarkers)) {
        draggedMarker = ROOT_NOTE;
        DBG("root");
    } else if (minMarker.contains(positionForMarkers)) {
        DBG("min");
        draggedMarker = MIN_NOTE;
    } else if (maxMarker.contains(positionForMarkers)) {
        DBG("max");
        draggedMarker = MAX_NOTE;
    }
}

void PianoRoll::mouseUp(const MouseEvent &event)
{
    auto position = event.getPosition();
    auto keyNumber = getKeyClicked(position);
    midiConsumer.noteOff(keyNumber);

    draggedMarker = NO_MARKER;
}

void PianoRoll::mouseDrag(const MouseEvent &event)
{
    auto position = event.getPosition();
    if (draggedMarker == NO_MARKER) {
        return;
    }

    position.y += Settings::PIANO_ROLL_RANGE_MARKERS_HEIGHT;

    if (draggedMarker == ROOT_NOTE) {
        rootMarkerDragged(position);
    } else if (draggedMarker == MIN_NOTE) {
        minMarkerDragged(position);
    } else if (draggedMarker == MAX_NOTE) {
        maxMarkerDragged(position);
    }
}

void PianoRoll::mouseMove(const MouseEvent &event)
{
    auto position = event.getPosition();

    // markers have negative Y because of render way, so here we compensate it
    auto positionForMarkers = Point<float>(position.x, position.y - Settings::PIANO_ROLL_RANGE_MARKERS_HEIGHT);

    if (rootMarker.contains(positionForMarkers)) {
        setMouseCursor(MouseCursor::PointingHandCursor);
    } else if (minMarker.contains(positionForMarkers)) {
        setMouseCursor(MouseCursor::PointingHandCursor);
    } else if (maxMarker.contains(positionForMarkers)) {
        setMouseCursor(MouseCursor::PointingHandCursor);
    } else if (position.y > Settings::PIANO_ROLL_RANGE_MARKERS_HEIGHT){
        setMouseCursor(MouseCursor::PointingHandCursor);
    } else {
        setMouseCursor(MouseCursor::NormalCursor);
    }
}

int PianoRoll::getKeyClicked(Point<int> point)
{
    point.x += keysInfo[FIRST_C_INDEX + offset].x;

    for (int i = FIRST_C_INDEX + offset; i < keysInfo.size(); i++) {
        // translate to abs coords

        if (keysInfo[i].contains(point)) {
            return keysInfo[i].number;
        }
    }
    
    return -1;
}

void drawActiveNoteMask(int x, int width, int height, Graphics& g)
{
    g.setColour(Settings::PIANO_ROLL_ACTIVE_KEY_MASK_COLOR);
    g.fillRect((float) x, (float) 0, (float) width, (float) height);
}

void PianoRoll::drawNoteTips(Graphics& g)
{
    static constexpr auto spaceBetweenCNotes = Settings::PIANO_ROLL_WHITE_NOTE_WIDTH * 7;

    // produces a lot of leaks =(
    //g.setFont(Settings::PIANO_ROLL_TIPS_FONT);
    g.setColour(Settings::PIANO_ROLL_NOTE_TIPS_COLOR);

    auto firstKeyOnScreen = Settings::DEFAULT_FIRST_KEY + offset;
    for (int i = firstKeyOnScreen; i < keysInfo.size(); i++) {
        auto noteCoord = keysInfo[i].x - keysInfo[firstKeyOnScreen].x;

        if (noteCoord > getWidth()) {
            return;
        }
        auto isCNote = (abs(FIRST_C_INDEX - i) % 12) == 0;
        if (!isCNote) {
            continue;
        }
        auto cNoteNum = (i - C_0_INDEX) / 12;

        auto tipX = noteCoord + 4;
        drawNoteTip(g, cNoteNum, tipX);
    }
}

void drawNoteTip(Graphics &g, int cNumber, int x) {
    auto noteName = String("C");
    noteName.append(std::to_string(cNumber), 3);
    g.drawSingleLineText(noteName, x, Settings::PIANO_ROLL_TIPS_OFFSET_Y);
}

void PianoRoll::drawNoteRangeAndRoot(Graphics& g)
{
    g.setColour(Settings::NOTE_ROOT_MARKER_COLOR);
    g.fillPath(rootMarker);
    g.setColour(Settings::NOTE_RANGE_MARKER_COLOR);
    g.fillPath(minMarker);
    g.fillPath(maxMarker);
}

void PianoRoll::createMarkers(std::shared_ptr<SampleInfo> info)
{
    if (info == nullptr) {
      return;
    }

    rootMarker = createMarker(info->rootNote, true);
    minMarker = createMarker(info->minNote, false);
    maxMarker = createMarker(info->maxNote, false);
}

Path PianoRoll::createMarker(int noteNum, bool root)
{
    auto noteX = keysInfo[noteNum].x - keysInfo[FIRST_C_INDEX + offset].x;
    auto x = noteX + (keysInfo[noteNum].width / 2);
    auto y = -1 * Settings::PIANO_ROLL_RANGE_MARKERS_HEIGHT;
    if (!root) {
        // root marker must be bigger
        y *= 0.8;
    }

    auto marker = Path();
    auto halfSize = Settings::PIANO_ROLL_RANGE_MARKERS_HEIGHT / 2.0;

    marker.addTriangle(Point<float>(x - halfSize, y), Point<float>(x+halfSize, y), Point<float>(x, 0));

    return marker;
}

void PianoRoll::drawDisabledNotesMask(Graphics& g)
{
    auto leftKey = keysInfo[sample->minNote];
    auto leftKeyX = leftKey.x - keysInfo[FIRST_C_INDEX + offset].x;
    auto maskLeftX = 0;
    auto maskRightX = leftKeyX;

    if (leftKeyX > 0 && maskLeftX != maskRightX) {
        g.setColour(Settings::PIANO_ROLL_DISABLED_MASK_COLOR);
        g.fillRect((float) maskLeftX, .0f, (float) maskRightX, (float) getHeight());
    }

    auto rightKey = keysInfo[sample->maxNote];
    auto rightKeyX = rightKey.x - keysInfo[FIRST_C_INDEX + offset].x;
    maskLeftX = rightKeyX + rightKey.width;
    maskRightX = getWidth();

    if (rightKeyX < getWidth() && maskLeftX != maskRightX) {
        g.setColour(Settings::PIANO_ROLL_DISABLED_MASK_COLOR);
        g.fillRect((float) maskLeftX, .0f, (float) maskRightX, (float) getHeight());
    }
}

void PianoRoll::sampleChanged(std::shared_ptr<SampleInfo> info)
{
    this->sample = info;

    createMarkers(info);
}

void PianoRoll::noSamplesLeft()
{
    this->sample = nullptr;
}

void PianoRoll::scrollBarMoved(ScrollBar *scrollBarThatHasMoved, double newRangeStart)
{
    offset = newRangeStart - Settings::PIANO_ROLL_RANGE_START;
    createMarkers(sample);
}

void PianoRoll::rootMarkerDragged(Point<int> position)
{
    auto keyClicked = getKeyClicked(position);
    if (keyClicked == sample->rootNote) {
        // no need to update
        return;
    }
    if (keyClicked > sample->maxNote) {
        // cannot go right
        draggedMarker = MAX_NOTE;
        return;
    }
    if (keyClicked < sample->minNote) {
        // cannot go left
        draggedMarker = MIN_NOTE;
        return;
    }

    rootMarker = createMarker(keyClicked, true);
    draggedMarker = ROOT_NOTE;
    sample->rootNote = keyClicked;

    midiConsumer.setRootNote(keyClicked);
}

void PianoRoll::minMarkerDragged(Point<int> position)
{
    auto keyClicked = getKeyClicked(position);
    if (keyClicked == sample->minNote) {
        // no need to update
        return;
    }
    if (keyClicked < Settings::DEFAULT_FIRST_KEY) {
        return;
    }
    if (keyClicked > sample->rootNote) {
        return;
    }

    minMarker = createMarker(keyClicked, false);
    draggedMarker = MIN_NOTE;
    sample->minNote = keyClicked;

    midiConsumer.setNoteRange(sample->minNote, sample->maxNote);
}

void PianoRoll::maxMarkerDragged(Point<int> position)
{
    auto keyClicked = getKeyClicked(position);
    if (keyClicked == sample->maxNote) {
        // no need to update
        return;
    }
    if (position.x >= this->getWidth()) {
        return;
    }
    if (keyClicked < sample->rootNote) {
        return;
    }

    maxMarker = createMarker(keyClicked, false);
    draggedMarker = MAX_NOTE;
    sample->maxNote = keyClicked;

    midiConsumer.setNoteRange(sample->minNote, sample->maxNote);
}

void PianoRoll::resized()
{
//    auto bounds = getLocalBounds();
//    leftArrow->setBounds(5, bounds.getHeight() / 2 - Settings::PIANO_ROLL_ARROW_HEIGHT / 2, Settings::PIANO_ROLL_ARROW_WIDTH, Settings::PIANO_ROLL_ARROW_HEIGHT);
//    rightArrow->setBounds(bounds.getWidth() - Settings::PIANO_ROLL_ARROW_WIDTH - 5, bounds.getHeight() / 2 - Settings::PIANO_ROLL_ARROW_HEIGHT / 2, Settings::PIANO_ROLL_ARROW_WIDTH, Settings::PIANO_ROLL_ARROW_HEIGHT);
}

std::vector<int> PianoRoll::getActiveMidiNotes()
{
    return midiSupplier.getActiveNotes();
}

