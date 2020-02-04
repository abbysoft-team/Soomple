/*
  ==============================================================================

    LinearPanel.cpp
    Created: 16 Aug 2019 8:01:25pm
    Author:  MacPro

  ==============================================================================
*/

#include "LinearPanel.h"
#include "../Settings.h"
#include <assert.h>

LinearPanel::LinearPanel(Orientation orientation, const String &name) : Component(), orientation(orientation), paddingX(10), paddingY(15) {
    panelLabel.reset(new Label());
    panelLabel->setText(name, NotificationType::dontSendNotification);
    addAndMakeVisible(panelLabel.get());
}

void LinearPanel::resized() {
    
    FlexBox fb;
    
    if (orientation == HORIZONTAL) {
        fb.flexDirection = FlexBox::Direction::row;
    } else {
        fb.flexDirection = FlexBox::Direction::column;
    }
    
    for (auto child : getChildren()) {
        if (child == panelLabel.get()) {
            continue;
        }
        fb.items.add(FlexItem(*child).withMinWidth(child->getWidth()).withMinHeight(child->getHeight()).withMargin(FlexItem::Margin(paddingY + Settings::PANEL_HEADER_HEIGHT / 2, Settings::PANEL_BORDER_THICKNESS, 0, 0)));;
    }
    
    fb.performLayout(getLocalBounds());
    
    panelLabel->setBounds(0, 0, panelLabel->getText().length() * 6, 20);
}

void LinearPanel::paint(Graphics &g) {
    g.setColour(Settings::PANEL_BACKGROUND_COLOR);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), Settings::PANEL_BORDER_CORNER_RADIUS);
    g.setColour(Settings::THUMBNAIL_BOUNDS_COLOR);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), Settings::PANEL_BORDER_CORNER_RADIUS, Settings::PANEL_BORDER_THICKNESS);
    g.fillRoundedRectangle(Rectangle<float>(0, 0, getWidth(), Settings::PANEL_HEADER_HEIGHT), Settings::PANEL_BORDER_CORNER_RADIUS);
    g.fillRect(Rectangle<int>(0, Settings::PANEL_BORDER_CORNER_RADIUS / 2, getWidth(), Settings::PANEL_HEADER_HEIGHT - Settings::PANEL_BORDER_CORNER_RADIUS / 2));
    
}

void LinearPanel::setPosition(int x, int y) {
    setBounds(x, y, getWidth(), getHeight());
}

void LinearPanel::addAndMakeVisible(Component *component) {
    Component::addAndMakeVisible(component);
    
    pack();
}

void LinearPanel::pack() {
    auto width = paddingX + Settings::PANEL_BORDER_THICKNESS * 2;
    auto height = paddingY + Settings::PANEL_BORDER_THICKNESS * 2;
    
    for (auto child : getChildren()) {
        width += child->getWidth();
        height = std::max(height, child->getHeight() + paddingY);
    }
    
    setSize(width - panelLabel->getWidth(), height);
}

void LinearPanel::setPadding(int padding) {
    assert(padding >= 0);
    
    paddingX = padding;
    paddingY = padding;
}
