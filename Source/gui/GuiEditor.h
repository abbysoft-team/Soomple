#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class GuiEditor : public Component, private KeyListener
{
public:

    /**
     * Component pointer must be handled by external code
     *
     * @brief GuiEditor
     * @param component
     */
    GuiEditor(Component* component);
    ~GuiEditor() = default;

    void paintBackOverlay (Graphics& g);
    void paintFrontOverlay (Graphics& g);

    void addToGuiEditor(Component* component);
    void initOverlay();

private:

    void mouseDown(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseMove(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &event) override;
    Component* findComponentAt(Point<int> position);
    Point<int> getPositionWithRespectToGrid(Point<int> position, Rectangle<int> componentBounds);
    bool keyPressed(const KeyPress &key, Component *originatingComponent) override;
    void enableEditor(bool enabled);
    void printNewComponentPositions();
    void increaseSelectedComponentSize();
    void decreaseSelectedComponentSize();

    Component* selectedComponent;
    Component* editableComponent;
    std::vector<Component*> components;
    int currentGridSize;
    bool isEnabled;
};
