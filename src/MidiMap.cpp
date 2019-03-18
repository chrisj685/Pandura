#include "MidiMap.h"

#include <algorithm>

MidiMap::MidiMap() : activeNotes()
{
    for (int i = 0; i++; i < 128)
    {
        midi[i] = nullptr;
    }
}

AmpEnv* MidiMap::getEnvState(int note)
{
    return midi[note];
}

void MidiMap::setActiveEnv(int note, AmpEnv* env)
{
    removeEnv(note);
    midi[note] = env;
    activeNotes.push_back(note);
}

void MidiMap::removeEnv(int note)
{
    delete midi[note];
    midi[note] = nullptr;
    activeNotes.erase(std::remove(activeNotes.begin(), activeNotes.end(), note), activeNotes.end());
}

std::vector<int>* MidiMap::getActiveNotes()
{
    return &activeNotes;
}