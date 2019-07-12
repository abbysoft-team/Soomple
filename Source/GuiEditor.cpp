#include <../Source/GuiEditor.h>
#include <../Source/Settings.h>

/**
 *
 * @brief GuiEditor::GuiEditor
 * @param component
 */
GuiEditor::GuiEditor(juce::Component *component) :
    editableComponent(component),
    selectedComponent(nullptr),
    currentGridSize(Settings::GUI_EDITOR_GRID_SIZE),
    isEnabled(true)
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
    if (key == KeyPress::F5Key) {
        enableEditor(!isEnabled);
    }
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