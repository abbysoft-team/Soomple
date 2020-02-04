/*
  ==============================================================================

    SampleBrowser.cpp
    Created: 9 Aug 2019 5:05:47pm
    Author:  artyom

  ==============================================================================
*/

#include "SampleBrowser.h"
#include "../Settings.h"
#include "SamplePreviewComponent.h"

SampleBrowser::SampleBrowser(FileListener& listener, SoomplerAudioProcessor &processor) : Component("SampleBrowser"),
    FileBrowserListener(), fileListener(listener)
{
    background = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    previewComponent.reset(new SamplePreviewComponent(processor));
    browser.reset(new FileBrowserComponent(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                                           Settings::INITIAL_DIRECTORY, Settings::BROWSER_FILE_FILTER.get(), nullptr));
    browser->addListener(this);

    // configure preview component persistent state
    processor.addNewSaveableObject(previewComponent);

    addAndMakeVisible(browser.get());
    addAndMakeVisible(previewComponent.get());
}

void SampleBrowser::resized()
{
    FlexBox fb;

    fb.flexDirection = FlexBox::Direction::column;

    fb.items.add(FlexItem(*(browser.get())).withFlex(3));
    fb.items.add(FlexItem(*(previewComponent.get())).withMinHeight(Settings::SAMPLE_PREVIEW_HEIGHT));

    fb.performLayout(getLocalBounds().toFloat());
}

void SampleBrowser::selectionChanged()
{
    auto selected = browser->getSelectedFile(0);
    previewComponent->selectedFileChanged(selected);
}

void SampleBrowser::fileClicked(const File &file, const MouseEvent &e)
{
}

void SampleBrowser::fileDoubleClicked(const File &file)
{
    fileListener.fileRecieved(file);
}

void SampleBrowser::browserRootChanged(const File &newRoot)
{
}

void SampleBrowser::paint(Graphics &g)
{
   //g.drawImage(background, Rectangle<float>(this->getX(), this->getY(), this->getWidth(), this->getHeight()));
    Component::paint(g);
}

void SampleBrowser::saveStateToMemory(StateBundle &bundle)
{
    bundle.addProperty(var(browser->getRoot().getFullPathName()), "currentRoot");
}

void SampleBrowser::getStateFromMemory(StateBundle &bundle)
{
    String restoredRoot = bundle.getProperty("currentRoot");
    if (restoredRoot.isEmpty()) {
        return;
    }

    browser->setRoot(File(restoredRoot));
}
