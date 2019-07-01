/*
  ==============================================================================

    ExtendedSampler.cpp
    Created: 27 Jun 2019 4:53:19pm
    Author:  artyom

  ==============================================================================
*/

#include "ExtendedSampler.h"

namespace soompler {

ExtendedSound::ExtendedSound (const String& soundName,
                            AudioFormatReader& source,
                            const BigInteger& notes,
                            int midiNoteForNormalPitch,
                            double attackTimeSecs,
                            double releaseTimeSecs,
                            double maxSampleLengthSeconds)
    : name (soundName),
      sourceSampleRate (source.sampleRate),
      midiNotes (notes),
      midiRootNote (midiNoteForNormalPitch)
{
    if (sourceSampleRate > 0 && source.lengthInSamples > 0)
    {
        length = jmin ((int) source.lengthInSamples,
                       (int) (maxSampleLengthSeconds * sourceSampleRate));

        data.reset (new AudioBuffer<float> (jmin (2, (int) source.numChannels), length + 4));

        source.read (data.get(), 0, length + 4, 0, true, true);

        params.attack  = static_cast<float> (attackTimeSecs);
        params.release = static_cast<float> (releaseTimeSecs);
    }
}

ExtendedSound::~ExtendedSound()
{
}

bool ExtendedSound::appliesToNote (int midiNoteNumber)
{
    return midiNotes[midiNoteNumber];
}

bool ExtendedSound::appliesToChannel (int /*midiChannel*/)
{
    return true;
}

//==============================================================================
ExtendedVoice::ExtendedVoice(std::shared_ptr<ChangeListener>listener) : eventListener(listener), volume(0)
{
}

ExtendedVoice::~ExtendedVoice() {}

bool ExtendedVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast<const ExtendedSound*> (sound) != nullptr;
}

void ExtendedVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound* s, int /*currentPitchWheelPosition*/)
{
    if (auto* sound = dynamic_cast<const ExtendedSound*> (s))
    {
        pitchRatio = std::pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
                        * sound->sourceSampleRate / getSampleRate();

        sourceSamplePosition = 0.0;
        lgain = velocity * volume;
        rgain = velocity * volume;

        adsr.setSampleRate (sound->sourceSampleRate);
        adsr.setParameters (sound->params);

        adsr.noteOn();
    }
    else
    {
        jassertfalse; // this object can only play SamplerSounds!
    }
}

void ExtendedVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        clearCurrentNote();
        adsr.reset();
    }
}

void ExtendedVoice::pitchWheelMoved (int /*newValue*/) {}
void ExtendedVoice::controllerMoved (int /*controllerNumber*/, int /*newValue*/) {}

//==============================================================================
void ExtendedVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (auto* playingSound = static_cast<ExtendedSound*> (getCurrentlyPlayingSound().get()))
    {
        auto& data = *playingSound->data;
        const float* const inL = data.getReadPointer (0);
        const float* const inR = data.getNumChannels() > 1 ? data.getReadPointer (1) : nullptr;

        float* outL = outputBuffer.getWritePointer (0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;

        while (--numSamples >= 0)
        {
            auto pos = (int64) sourceSamplePosition + this->firstSampleToPlay;
            auto alpha = (float) (sourceSamplePosition + this->firstSampleToPlay - pos);
            auto invAlpha = 1.0f - alpha;

            // just using a very simple linear interpolation here..
            float l = (inL[pos] * invAlpha + inL[pos + 1] * alpha);
            float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha)
                                       : l;

            auto envelopeValue = adsr.getNextSample();

            l *= lgain * envelopeValue;
            r *= rgain * envelopeValue;

            if (outR != nullptr)
            {
                *outL++ += l;
                *outR++ += r;
            }
            else
            {
                *outL++ += (l + r) * 0.5f;
            }

            sourceSamplePosition += pitchRatio;

            if (endSample == 0) {
                endSample = playingSound->length;
            }

            if (sourceSamplePosition + firstSampleToPlay > endSample)
            {
                stopNote (0.0f, false);
                break;
            }
        }
    }
}

void ExtendedVoice::setStartSample(int64 sample)
{
    this->firstSampleToPlay = sample;
}

void ExtendedVoice::setEndSample(int64 sample)
{
    this->endSample = sample;
}

double ExtendedVoice::getCurrentPosition() const
{
    if (isPlayingButReleased()) {
        return firstSampleToPlay / getSampleRate();
    }

    return (sourceSamplePosition + firstSampleToPlay) / getSampleRate();
}

void ExtendedVoice::setVolume(float volume)
{
    jassert(volume <= 1.0f && volume >= .0f);
    this->volume = volume;
}

}