#include "GuiEditor.h"
#include "../Settings.h"

/**
 *
 * @brief GuiEditor::GuiEditor
 * @param component
 */
GuiEditor::GuiEditor(juce::Component *component) :
    selectedComponent(nullptr),
    editableComponent(component),
    currentGridSize(Settings::GUI_EDITOR_GRID_SIZE),
    isEnabled(false)
{
    //editableComponent->addMouseListener(this, true);
    editableComponent->addKeyListener(this);
}

void GuiEditor::paintBackOverlay(Graphics &g)
{
    if (!isEnabled) {
        return;
    }

    static constexpr auto netThickness = 2;
    static constexpr auto selectionThickness = 4;

    g.setColour(Settings::GUI_EDITOR_COLOR);
    // vertical lines
    for (int i = 0; i < editableComponent->getWidth(); i += currentGridSize) {
        g.drawLine(i, 0, i, editableComponent->getHeight(), netThickness);
    }

    // horisontal lines
    for (int i = 0; i < editableComponent->getHeight(); i += currentGridSize) {
        g.drawLine(0, i, editableComponent->getWidth(), i, netThickness);
    }

    if (selectedComponent == nullptr) {
        return;
    }
    // selected component
    g.setColour(Settings::GUI_EDITOR_SELECTION_COLOR);
    g.drawRect(selectedComponent->getBounds(), selectionThickness);
}

void GuiEditor::paintFrontOverlay(Graphics &g)
{
}

void GuiEditor::addToGuiEditor(Component *component)
{
    components.push_back(component);
}

void GuiEditor::initOverlay()
{
    setBounds(0, 0, editableComponent->getWidth(), editableComponent->getHeight());
}

void GuiEditor::mouseDown(const MouseEvent &event)
{
    if (!isEnabled) {
        return;
    }

    auto position = event.getPosition();
    auto component = findComponentAt(position);

    selectedComponent = component;
}

void GuiEditor::mouseUp(const MouseEvent &event)
{
    selectedComponent = nullptr;
    editableComponent->repaint();
}

void GuiEditor::mouseMove(const MouseEvent &event)
{
    // hover on element
}

void GuiEditor::mouseDrag(const MouseEvent &event)
{
    if (!isEnabled) {
        return;
    }

    auto position = event.getPosition();
    if (selectedComponent == nullptr) {
        return;
    }

    // some components have wrong width and height
    if (selectedComponent->getWidth() > editableComponent->getWidth()) {
        selectedComponent = nullptr;
        return;
    }
    if (selectedComponent->getHeight() > editableComponent->getHeight()) {
        selectedComponent = nullptr;
        return;
    }

    // set bounds of center of component, not of upper left corner
    auto width = selectedComponent->getWidth();
    auto height = selectedComponent->getHeight();

    auto newXOffset = width / 2;
    auto newYOffset = height / 2;
    auto newX = position.x - newXOffset;
    auto newY = position.y - newYOffset;

    auto newPosition = getPositionWithRespectToGrid(Point<int>(newX, newY), selectedComponent->getBounds());

    selectedComponent->setBounds(newPosition.x, newPosition.y, width, height);
    editableComponent->repaint();
}

Component *GuiEditor::findComponentAt(Point<int> position)
{
    for (auto component : components) {
        auto relativePosition = Point<int>(position.getX() - component->getX(), position.getY() - component->getY());
        if (component->contains(relativePosition)) {
            return component;
        }
    }
    
    return nullptr;
}

Point<int> GuiEditor::getPositionWithRespectToGrid(Point<int> position, Rectangle<int> componentBounds)
{
    // all coords is upper-left corner of component
    auto xCellIndex = position.x / currentGridSize;
    auto yCellIndex = position.y / currentGridSize;

    auto newX = xCellIndex * currentGridSize;
    auto newY = yCellIndex * currentGridSize;

    // right border restriction
    if (newX + componentBounds.getWidth() >= editableComponent->getWidth()) {
        newX = editableComponent->getWidth() - componentBounds.getWidth();
    } else if (newX <= 0) {
        newX = 0;
    }

    // upper border restriction
    if (newY <= 0) {
        newY = 0;
    } else if (newY + componentBounds.getHeight() >= editableComponent->getHeight()) {
        // bottom
        newY = editableComponent->getHeight() - componentBounds.getHeight();
    }

    return Point<int>(newX, newY);
}

bool GuiEditor::keyPressed(const KeyPress &key, Component *originatingComponent)
{
    if (key == KeyPress::F8Key) {
        enableEditor(!isEnabled);
    } else if (key == KeyPress::F2Key) {
        printNewComponentPositions();
    } else if (key == KeyPress::upKey) {
        increaseSelectedComponentSize();
        editableComponent->repaint();
    } else if (key == KeyPress::downKey) {
        decreaseSelectedComponentSize();
        editableComponent->repaint();
    }
    
    return false;
}

void GuiEditor::enableEditor(bool enabled)
{
    isEnabled = enabled;

    if (!enabled) {
        editableComponent->removeChildComponent(this);
    } else {
        editableComponent->addAndMakeVisible(this);
    }
}

void GuiEditor::printNewComponentPositions()
{
    DBG("=====================================");
    DBG("");

    for (auto component : components) {
        DBG(component->getName());
        DBG("position");
        DBG(component->getX());
        DBG(component->getY());
        DBG("size");
        DBG(component->getWidth());
        DBG(component->getHeight());
    }

    DBG("=====================================");
}

void GuiEditor::increaseSelectedComponentSize()
{
    if (selectedComponent == nullptr) {
        return;
    }

    auto newWidth = selectedComponent->getWidth() * Settings::GUI_EDITOR_SIZE_STEP_COEFF_INCR;
    auto newHeight = selectedComponent->getHeight() * Settings::GUI_EDITOR_SIZE_STEP_COEFF_INCR;

    // check for out of bounds situation
    Rectangle<int> newRect = Rectangle<int>(selectedComponent->getX(), selectedComponent->getY(), newWidth, newHeight);
    if (!editableComponent->contains(newRect.getTopLeft())) {
        // out of bounds
        return;
    }
    if (!editableComponent->contains(newRect.getBottomRight())) {
        // out of bounds
        return;
    }

    selectedComponent->setSize(newWidth, newHeight);
}

void GuiEditor::decreaseSelectedComponentSize()
{
    if (selectedComponent == nullptr) {
        return;
    }

    auto newWidth = selectedComponent->getWidth() * Settings::GUI_EDITOR_SIZE_STEP_COEFF_DECR;
    auto newHeight = selectedComponent->getHeight() * Settings::GUI_EDITOR_SIZE_STEP_COEFF_DECR;

    if (newWidth <= 0 && newHeight <= 0) {
        return;
    }

    selectedComponent->setSize(newWidth, newHeight);
}
