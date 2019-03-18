#include "AmpEnv.h"

AmpEnv::AmpEnv(int a, int d, double s, int r, 
               POscillator* osc, double sampleRate, double frequency) : 
    attack(a), decay(d), sustain(s), release(r), _sampleRate(sampleRate), 
    _osc(osc), _oscState((1/_sampleRate) * frequency)
{
    envState = ATTACK;
    stateTime = 0;
    aToS = 1. - sustain;

    _oscState._phaseInc = (1 / _sampleRate) * frequency;
}


AmpEnv::~AmpEnv()
{
}

double AmpEnv::process()
{
    double mod = 0.;
    switch (envState)
    {
    case ATTACK:
        mod = ((double)stateTime / attack);
        if (stateTime >= attack)
        {
            envState = DECAY;
            stateTime = 0;
        }
        stateTime++;
        break;
    case DECAY:
        mod = 1 - (((double)stateTime / decay) * aToS);
        if (stateTime >= decay)
        {
            envState = SUSTAIN;
            stateTime = 0;
        }
        stateTime++;
        break;
    case SUSTAIN:
        mod = sustain;
        break;
    case RELEASE:
        mod = sustain - (((float)stateTime / release) * sustain);
        if (stateTime >= release)
        {
            envState = DONE;
        }
        stateTime++;
        break;
    default:
        break;
    }

    return _osc->process(&_oscState) * mod;
}

void AmpEnv::setFrequency(double freq)
{
    envState = ATTACK;
    stateTime = 0;
    _oscState._phase = 0;
    _oscState._phaseInc = (1 / _sampleRate) * freq;
}

void AmpEnv::reset()
{
    envState = ATTACK;
    stateTime = 0;
}

void AmpEnv::releaseEnv()
{
    envState = RELEASE;
    stateTime = 0;
}