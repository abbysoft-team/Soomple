/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.3

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "SampleViewer.h"
#include "../Settings.h"

//[MiscUserDefs] You can add your own user definitions and misc code here...

void drawRangeLine(int xPos, Graphics& g);
// draw faded regions before and after range lines
void fadePreStartRegion(int startRangeBorderX, Graphics& g);
void fadePostEndRegion(int endRangeBorderX, Graphics& g);
bool isIntersectWithRangeLine(Point<int>& point, int rangeLinePos);

SampleViewer::SampleViewer (TransportInfoOwner& transportInfoOwner, SampleChangeListener& infoListener, std::shared_ptr<SampleManager> manager)
    : currentSample(new SampleInfo(0, 44100, "", "")),
    transportInfoOwner(transportInfoOwner),
    sampleInfoListener(infoListener),
    startRangeX(0),
    endRangeX(Settings::THUMBNAIL_BOUNDS.getWidth()),
    maxRangeX(Settings::THUMBNAIL_BOUNDS.getWidth()),
    draggedLine(NONE),
    header(manager),
    manager(manager)
{
    addAndMakeVisible(header);
}

SampleViewer::~SampleViewer()
{
}

//==============================================================================
void SampleViewer::paint (Graphics& g)
{
    drawThumbnail(g);
    drawPositionLine(g);

    // start range line with faded region
    drawRangeLine(startRangeX, g);
    fadePreStartRegion(startRangeX, g);
    // end range line
    drawRangeLine(endRangeX, g);
    fadePostEndRegion(endRangeX, g);
    //[/UserPaint]
}


void SampleViewer::drawThumbnail(Graphics &g)
{
    auto thumbnail = manager->getActiveSample()->thumbnail;
    if (thumbnail->getNumChannels() == 0)
    {
        return;
    }

    g.setColour(Settings::THUMBNAIL_BOUNDS_COLOR);
    g.drawRect(0, Settings::THUMBNAIL_HEADER_HEIGHT,
               Settings::THUMBNAIL_BOUNDS.getWidth(),
               Settings::THUMBNAIL_BOUNDS.getHeight());

    g.setColour(Settings::THUMBNAIL_COLOR);

    auto audioLength = thumbnail->getTotalLength();
    auto thumbnailRect = Rectangle<int>(0, Settings::THUMBNAIL_HEADER_HEIGHT,
                                        Settings::THUMBNAIL_BOUNDS.getWidth(),
                                        Settings::THUMBNAIL_BOUNDS.getHeight());

    thumbnail->drawChannels(g,
                           thumbnailRect,
                           0.0,
                           audioLength,
                           1.0f);
}

void SampleViewer::drawPositionLine(Graphics &g)
{
    g.setColour(Settings::POSITION_LINE_COLOR);

    // offset exist to ensure that line is visible even near start range line
    constexpr auto lineOffset = 2;

    auto audioPosition = transportInfoOwner.getTransportInfo()->getAudioPositionInSec();
    auto drawPosition = ((audioPosition / currentSample->lengthInSeconds)
                         * Settings::THUMBNAIL_BOUNDS.getWidth() + lineOffset);

    g.drawLine(drawPosition, Settings::THUMBNAIL_HEADER_HEIGHT, drawPosition, Settings::SAMPLE_VIEWER_BOUNDS.getHeight(), 3.0f);
}

void drawRangeLine(int xPos, Graphics& g) {
    g.setColour(Settings::RANGE_LINES_COLOR);
    g.drawLine(xPos, Settings::THUMBNAIL_HEADER_HEIGHT, xPos,
               Settings::SAMPLE_VIEWER_BOUNDS.getHeight(), Settings::RANGE_LINES_WIDTH);
}

void fadePreStartRegion(int startRangeBorderX, Graphics& g) {
    g.setColour(Settings::NOT_ACTIVE_SAMPLE_REGION_MASK_COLOR);
    g.fillRect(0, Settings::THUMBNAIL_HEADER_HEIGHT, startRangeBorderX, Settings::THUMBNAIL_BOUNDS.getHeight());
}

void fadePostEndRegion(int endRangeBorderX, Graphics& g) {
    g.setColour(Settings::NOT_ACTIVE_SAMPLE_REGION_MASK_COLOR);
    int postEndRegionWidth = jmax(0, Settings::THUMBNAIL_BOUNDS.getWidth() - endRangeBorderX);
    g.fillRect(endRangeBorderX, Settings::THUMBNAIL_HEADER_HEIGHT, postEndRegionWidth, Settings::THUMBNAIL_BOUNDS.getHeight());
}

void SampleViewer::mouseDrag(const MouseEvent &event)
{
    auto position = event.getPosition();

    if (draggedLine == LEFT) {

        int rightBorderX = endRangeX - ((int) (Settings::RANGE_LINES_WIDTH*4));
        int leftBorderX = 0;

        if (position.getX() < leftBorderX) {
            startRangeX = leftBorderX;
        } else if (position.getX() > rightBorderX) {
            startRangeX = rightBorderX;
        } else {
            startRangeX = position.getX();
        }

        currentSample->startSample = calculateSampleByCoords(startRangeX);
        notifySampleInfoListeners();

        repaint();
    } else if (draggedLine == RIGHT) {
        int leftBorderX = startRangeX + ((int) (Settings::RANGE_LINES_WIDTH*4));

        if (position.getX() < leftBorderX) {
            endRangeX = leftBorderX;
        } else if (position.getX() > maxRangeX) {
            endRangeX = maxRangeX;
        } else {
            endRangeX = position.getX();
        }

        currentSample->endSample = calculateSampleByCoords(endRangeX);
        notifySampleInfoListeners();

        repaint();
    }
}

void SampleViewer::mouseMove(const MouseEvent &event)
{
    Component::mouseMove(event);

    auto position = event.getPosition();
    if (isIntersectWithRangeLine(position, startRangeX) || isIntersectWithRangeLine(position, endRangeX)) {
        setMouseCursor(MouseCursor::PointingHandCursor);
    } else {
        setMouseCursor(MouseCursor::NormalCursor);
    }
}

void SampleViewer::mouseDown(const MouseEvent &event)
{
    auto position = event.getPosition();
    if (isIntersectWithRangeLine(position, startRangeX)) {
        draggedLine = LEFT;
    } else if (isIntersectWithRangeLine(position, endRangeX)) {
        draggedLine = RIGHT;
    }
}

void SampleViewer::mouseUp(const MouseEvent &event)
{
    draggedLine = NONE;
}

bool isIntersectWithRangeLine(Point<int>& point, int rangeLinePos)
{
    static Rectangle<int> rangeLine;
    rangeLine.setX(rangeLinePos - Settings::RANGE_LINES_WIDTH);
    rangeLine.setY(Settings::THUMBNAIL_HEADER_HEIGHT);
    rangeLine.setWidth(Settings::RANGE_LINES_WIDTH * 2);
    rangeLine.setHeight(Settings::THUMBNAIL_BOUNDS.getHeight());

    return rangeLine.contains(point);
}

void SampleViewer::sampleChanged(std::shared_ptr<SampleInfo> info)
{
    this->currentSample = info;
    endRangeX = calculateCoordBySample(currentSample->endSample);
    startRangeX = calculateCoordBySample(currentSample->startSample);

    calculateEndRangeX();

    header.sampleChanged(info);
}

void SampleViewer::calculateEndRangeX()
{
    auto sampleLength = currentSample->lengthInSeconds;
    if (sampleLength < Settings::MAX_SAMPLE_LENGTH) {
        // everything is fine already
        return;
    }

    auto partOfSampleAllowed = Settings::MAX_SAMPLE_LENGTH / sampleLength;

    // 513 samples to make sure we dont reach max sample length
    auto lastAllowedSample = Settings::MAX_SAMPLE_LENGTH * currentSample->sampleRate - 513;
    currentSample->endSample = lastAllowedSample;

    notifySampleInfoListeners();

    // end range of sample may be changed from end of sample allready,
    // so we need to respect these changes
    auto newEndRangeX = (partOfSampleAllowed * Settings::THUMBNAIL_BOUNDS.getWidth());
    endRangeX = std::min((int) newEndRangeX, endRangeX);
    maxRangeX = endRangeX;
}

void SampleViewer::notifySampleInfoListeners()
{
    this->sampleInfoListener.sampleChanged(currentSample);
}

int64 SampleViewer::calculateSampleByCoords(int coordOnThumbnail)
{
    auto percentOfLength = coordOnThumbnail * 1.0 / Settings::THUMBNAIL_BOUNDS.getWidth();
    return ((int64) (percentOfLength * currentSample->lengthInSamples));
}

int SampleViewer::calculateCoordBySample(int64 sample)
{
    auto percentOfLength = sample * 1.0 / currentSample->lengthInSamples;
    return getWidth() * percentOfLength;
}

void SampleViewer::resized()
{
    header.setBounds(0, 0, getWidth(), Settings::THUMBNAIL_HEADER_HEIGHT);
}


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SampleViewer" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

