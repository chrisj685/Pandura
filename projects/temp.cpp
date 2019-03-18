/*#include "Pandura.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

#include <iostream>

Pandura::Pandura(IPlugInstanceInfo instanceInfo)
    : _osc(512), _oscState(),
    IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
    GetParam(kGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
    mMakeGraphicsFunc = [&]() {
        return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
    };

    mLayoutFunc = [&](IGraphics* pGraphics) {
        pGraphics->AttachCornerResizer(kUIResizerScale, false);
        pGraphics->AttachPanelBackground(COLOR_GRAY);
        pGraphics->LoadFont(ROBOTTO_FN);
        const IRECT b = pGraphics->GetBounds();
        pGraphics->AttachControl(new ITextControl(b, "Hello iPlug 2!", IText(50)));
        pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100), kGain));
    };
}

void Pandura::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
    const double gain = GetParam(kGain)->Value() / 100.;
    const int nChans = NOutChansConnected();
    double output = 0.;
    for (int s = 0; s < nFrames; s++)
    {
        output = _osc.process(&_oscState);
        for (int c = 0; c < nChans; c++)
        {
            outputs[c][s] = output * gain;
        }
    }
}
*/