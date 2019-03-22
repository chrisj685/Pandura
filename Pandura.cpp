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

    _filter.setFilterMode(_filter.FILTER_MODE_LOWPASS);

    //on = true;
    //_midiNote = 65;
    //_frequency = midiToFreq(_midiNote);

    GetParam(kGain)->InitDouble("Gain", 10., 0., 100.0, 0.01, "%");

    GetParam(kAttack)->InitDouble("Attack", 750, 250, 100000, 250, "Steps", 0, "", IParam::ShapeExp());
    GetParam(kDecay)->InitDouble("Decay", 750, 250, 100000, 250, "Steps", 0, "", IParam::ShapeExp());
    GetParam(kSustain)->InitDouble("Sustain", 1., 0., 1., 0.01, "%");
    GetParam(kRelease)->InitDouble("Release", 750, 250, 100000, 250, "Steps", 0, "", IParam::ShapeExp());

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

        // Volume envelope controls
        pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100)
          .GetVShifted(100)
          .GetHShifted(-150), kAttack));
        pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100)
          .GetVShifted(100)
          .GetHShifted(-50), kDecay));
        pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100)
          .GetVShifted(100)
          .GetHShifted(50), kSustain));
        pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100)
          .GetVShifted(100)
          .GetHShifted(150), kRelease));

        // Filter controls
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

    for (int s = 0; s < nFrames; s++)
    {
        output = 0.;
        auto notes = *_midiMap.getActiveNotes();
        for (int note : notes)
        {
            AmpEnv* env = _midiMap.getEnvState(note);
            output += env->process() * _gain;

            if (env->envState == DONE)
            {
                _midiMap.removeEnv(note);
            }
        }

        output = _filter.process(output);
        for (int c = 0; c < nChans; c++)
        {
            outputs[c][s] = output;
        }
    }
}

void Pandura::ProcessMidiMsg(const IMidiMsg & msg)
{
    int status = msg.StatusMsg();

    AmpEnv* env = nullptr;

    switch (status)
    {
    case IMidiMsg::kNoteOn:
        _midiNote = msg.NoteNumber();
        _frequency = midiToFreq(_midiNote);
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
        break;
    case kCutoff:
        _cutoff = GetParam(kCutoff)->Value();
        _filter.setCutoff(_cutoff);
        break;
    case kResonance:
        _resonance = GetParam(kResonance)->Value();
        _filter.setResonance(_resonance);
        break;
    case kAttack:
        _attack = GetParam(kAttack)->Value();
        break;
    case kDecay:
        _decay = GetParam(kDecay)->Value();
        break;
    case kSustain:
        _sustain = GetParam(kSustain)->Value();
        break;
    case kRelease:
        _release = GetParam(kRelease)->Value();
        break;

    default:
        break;
    }
}