/*
  ==============================================================================

    ViewerHeader.cpp
    Created: 9 Sep 2019 1:53:02pm
    Author:  artyom

  ==============================================================================
*/

#include "ViewerHeader.h"
#include "../Settings.h"

ViewerHeader::ViewerHeader(std::shared_ptr<SampleManager> manager) : manager(manager)
{
    closeImage = ImageCache::getFromMemory(BinaryData::lose_png, BinaryData::lose_pngSize);
}

void ViewerHeader::paint(Graphics &g)
{
    auto samples = manager->getAllSamples().size();
    auto sampleHeaderWidth = getWidth() / samples;

    g.setColour(Settings::PANEL_BACKGROUND_COLOR);
    g.fillRect(0, 0, getWidth(), getHeight());

    for (int i = samples - 1; i >= 0; i--) {
        paintNextSampleHeader(i, sampleHeaderWidth, manager->getAllSamples().at(i), g);
    }
}


void ViewerHeader::paintNextSampleHeader(int index, float width, std::shared_ptr<SampleInfo> info, Graphics &g)
{
    auto leftBorderX = index * width;
    auto rightBorderX = leftBorderX + width;

    g.setColour(Settings::PIANO_ROLL_WHITE_COLOR);
    if (leftBorderX > 0) {
        g.drawLine(leftBorderX, 0, leftBorderX, getHeight());
    }
    if (rightBorderX < getWidth()) {
        g.drawLine(rightBorderX, 0, rightBorderX, getHeight());
    }

    // mark active sample header
    if (info == sample) {
        g.setColour(Settings::THUMBNAIL_HEADER_COLOR);
        g.fillRect(leftBorderX, 0.0, width, getHeight() * 1.0);
    }

    g.setColour(Settings::SAMPLE_NAME_COLOR);
    g.setFont(Settings::SAMPLE_NAME_FONT_SIZE);
    g.drawSingleLineText(info->getCroppedName(width, Settings::SAMPLE_NAME_FONT_SIZE),
                         leftBorderX + width / 2,
                         (getHeight() + Settings::SAMPLE_NAME_FONT_SIZE) / 2 - 2,
                         Justification::horizontallyCentred);

    // paint close Button
    g.drawImage(closeImage, Rectangle<float>(rightBorderX - Settings::CLOSE_SAMPLE_BUTTON_SIZE - 2, 2, Settings::CLOSE_SAMPLE_BUTTON_SIZE, Settings::CLOSE_SAMPLE_BUTTON_SIZE));
}

void ViewerHeader::sampleChanged(std::shared_ptr<SampleInfo> info)
{
    sample = info;
}

void ViewerHeader::mouseMove(const MouseEvent &event)
{
    setMouseCursor(MouseCursor::PointingHandCursor);
}

void ViewerHeader::mouseDown(const MouseEvent &event)
{
    if (closeButtonPressed(event.getPosition())) {
        manager->removeSample(getSampleUnderMouse(event.getPosition()));
        return;
    }

    auto sampleUnderMouse = getSampleUnderMouse(event.getPosition());
    if (sampleUnderMouse != manager->getActiveSample()) {
        manager->sampleChanged(sampleUnderMouse);
    }
}

std::shared_ptr<SampleInfo> ViewerHeader::getSampleUnderMouse(Point<int> position)
{
    auto samples = manager->getAllSamples();
    auto sampleTabWidth = getWidth() / samples.size();

    auto index = position.getX() / sampleTabWidth;
    return samples.at(index);
}

bool ViewerHeader::closeButtonPressed(Point<int> position)
{
    auto samples = manager->getAllSamples().size();
    auto sampleTabWidth = getWidth() / samples;
    int index = position.getX() / sampleTabWidth;
    auto xInsideTab = position.getX() - index * sampleTabWidth;

    Rectangle<float> closeButtonBounds = Rectangle<float>(sampleTabWidth - Settings::CLOSE_SAMPLE_BUTTON_SIZE + 2, 2, Settings::CLOSE_SAMPLE_BUTTON_SIZE, Settings::CLOSE_SAMPLE_BUTTON_SIZE);
    return closeButtonBounds.contains(xInsideTab, position.getY());
}

