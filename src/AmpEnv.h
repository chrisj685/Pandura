#pragma once
#include "POscillator.h"

enum EnvState
{
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE,
    DONE
};


/*
    Envelope needs to transition smoothly from midrelease to attack
    Currently restarts from 0 even if release is full
*/
class AmpEnv
{
public:
    AmpEnv(int, int, double, int, POscillator*, double, double);
    ~AmpEnv();

    int attack;
    int decay;
    double sustain;
    int release;

    EnvState envState;
    int stateTime;

    double process();
    void setFrequency(double freq);
    void reset();
    void releaseEnv();

private:
    POscillator* _osc;
    OscState _oscState;
    double _sampleRate;

    double aToS;
    double _mod;
};

