#pragma once
#include <map>
#include <vector>
#include "POscillator.h"
#include "AmpEnv.h"

class MidiMap
{
public:
    MidiMap();
    AmpEnv* getEnvState(int note);
    void setActiveEnv(int note, AmpEnv*);
    void removeEnv(int note);
    std::vector<int>* getActiveNotes();

private:
    std::map<int, AmpEnv*> midi;
    std::vector<int> activeNotes;
};