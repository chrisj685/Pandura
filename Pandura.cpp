#include "Pandura.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

#include <iostream>

Pandura::Pandura(IPlugInstanceInfo instanceInfo)
    : _osc(512, SAW), _filter(),
    IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
    _frequency = 440;

    _sampleRate = GetSampleRate();

    //on = true;
    //_midiNote = 65;
    //_frequency = midiToFreq(_midiNote);

    GetParam(kGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
    GetParam(kFrequency)->InitDouble("Frequency", 440., 110., 1760., 0.01, "%");
    GetParam(kCutoff)->InitFrequency("Cutoff", 0.99, 0.01, 0.99, 0.001);
    GetParam(kResonance)->InitDouble("Resonance", 0.01, 0.01, 1.0, 0.001);
    mMakeGraphicsFunc = [&]() {
        return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
    };

    mLayoutFunc = [&](IGraphics* pGraphics) {
        pGraphics->AttachCornerResizer(kUIResizerScale, false);
        pGraphics->AttachPanelBackground(COLOR_GRAY);
        pGraphics->LoadFont(ROBOTTO_FN);
        const IRECT b = pGraphics->GetBounds();
        pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100), kGain));
        pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(100), kFrequency));
        pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100)
                                 .GetVShifted(200)
                                 .GetHShifted(-50), kCutoff));
        pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100)
                                 .GetVShifted(200)
                                 .GetHShifted(50), kResonance));
    };

    _state = -1;
    _attack = 750;
    _decay = 10000;
    _sustain = 0;
    _release = 5000; 
}

void Pandura::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
    const int nChans = NOutChansConnected();
    double output = 0.;
    double unfiltered = 0.;

    /*
    int newState = frequency > 880 ? 0 : 1;
    if (_state != newState)
    {
        if (_state == 1)
        {
            _midiMap.removeOsc(65);
            _midiMap.removeOsc(72);
        }

        if (newState == 1)
        {
            _midiMap.setActiveOsc(65, new OscState((1 / _sampleRate) * 440.));
            _midiMap.setActiveOsc(72, new OscState((1 / _sampleRate) * 523));
        }

        _state = newState;
    } */

    for (int s = 0; s < nFrames; s++)
    {
        output = 0.;
        auto notes = *_midiMap.getActiveNotes();
        for (int note : notes)
        {
            AmpEnv* env = _midiMap.getEnvState(note);
            unfiltered += env->process();

            if (env->envState == DONE)
            {
                _midiMap.removeEnv(note);
            }

            output = _filter.process(unfiltered) * _gain;

            
        }
        for (int c = 0; c < nChans; c++)
        {
            outputs[c][s] = output;
        }
    }
}

void Pandura::ProcessMidiMsg(const IMidiMsg & msg)
{
    TRACE;

    int status = msg.StatusMsg();

    AmpEnv* env = nullptr;

    switch (status)
    {
    case IMidiMsg::kNoteOn:
        _midiNote = msg.NoteNumber();
        _frequency = midiToFreq(_midiNote);
        on = true;
        _midiMap.setActiveEnv(
            _midiNote, 
            new AmpEnv(
                _attack, _decay, _sustain, _release, 
                &_osc, _sampleRate, _frequency));
        break;
    case IMidiMsg::kNoteOff:
        _midiNote = msg.NoteNumber();
        env = _midiMap.getEnvState(_midiNote);
        if (env != nullptr)
        {
            _midiMap.getEnvState(_midiNote)->releaseEnv();
        }
        break;
    case IMidiMsg::kPolyAftertouch:
    case IMidiMsg::kControlChange:
    case IMidiMsg::kProgramChange:
    case IMidiMsg::kChannelAftertouch:
    case IMidiMsg::kPitchWheel:
    default:
        break;
    }
}

void Pandura::OnParamChange(int paramIdx)
{
    switch (paramIdx)
    {
    case kGain:
        _gain = GetParam(kGain)->Value() / 100.;
    case kCutoff:
        _cutoff = GetParam(kCutoff)->Value();
        _filter.setCutoff(_cutoff);
    case kResonance:
        _resonance = GetParam(kResonance)->Value();
        _filter.setResonance(_resonance);

    default:
        break;
    }
}