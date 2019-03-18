#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "../src/POscillator.h"
#include "../src/MidiMap.h"

#include "../src/Filter.h"

#define PITCH 440.
#define TABLE_SIZE 512

#ifndef M_PI
#define M_PI 3.14159265
#endif

const int kNumPrograms = 1;

float midiToFreq(int midi)
{
    return 440. * pow(2., (midi - 69.) / 12.);
}

enum EParams
{
    kGain = 0,
    kFrequency = 1,
    kCutoff,
    kAttack,
    kDecay,
    kSustain,
    kRelease,
    kResonance,
    kNumParams
};

class Pandura : public IPlug
{
public:
    Pandura(IPlugInstanceInfo instanceInfo);
    void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
    void ProcessMidiMsg(const IMidiMsg& msg) override;
    void OnParamChange(int paramIdx) override;

private:
    POscillator _osc;
    double _sampleRate;
    MidiMap _midiMap;
    Filter _filter;

    int _midiNote;
    double _frequency;
    bool on;

    double _gain;

    int _attack;
    int _decay;
    double _sustain;
    int _release;

    float _cutoff;
    float _resonance;


    int _state;
};