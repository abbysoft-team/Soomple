/*
  ==============================================================================

    SoomplerSynth.cpp
    Created: 9 Sep 2019 12:29:29pm
    Author:  artyom

  ==============================================================================
*/

#include "SoomplerSynth.h"
#include "ExtendedSampler.h"

SoomplerSynth::SoomplerSynth() : Synthesiser()
{

}

void SoomplerSynth::noteOn(int midiChannel, int midiNoteNumber, float velocity)
{
    const ScopedLock sl (lock);

    // If hitting a note that's still ringing, stop it first (it could be
    // still playing because of the sustain or sostenuto pedal).
    for (auto* voice : voices)
        if (voice->getCurrentlyPlayingNote() == midiNoteNumber && voice->isPlayingChannel (midiChannel))
            stopVoice (voice, 1.0f, true);

    for (auto* sound : sounds)
    {
        if (sound->appliesToNote (midiNoteNumber) && sound->appliesToChannel (midiChannel))
        {

            auto soomplerSound = dynamic_cast<soompler::ExtendedSound*> (sound);

             startVoice (findFreeVoice (sound, midiChannel, midiNoteNumber, true),
                        sound, midiChannel, midiNoteNumber, soomplerSound->getVolume());
        }
    }
}
