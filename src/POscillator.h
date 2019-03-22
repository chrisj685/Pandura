#pragma once


const double M_PI = 3.14159265;

enum WaveType
{
    SINE,
    SAW,
};

inline double wrap(double x, double low = 0., double high = 1.)
{
    while (x >= high) x -= high;
    while (x < low)  x += high - low;

    return x;
}

inline double lerp(double phase, const double* buffer, unsigned long int mask)
{
	return buffer[(int)phase];
	/*
    const int intPart = (int)phase;
    const double fracPart = phase - intPart;

    const double a = buffer[intPart & mask];
    const double b = buffer[(intPart + 1) & mask];

    return a + (b - a) * fracPart;
	*/
}

class OscState
{
public:
    double _phase;
    double _phaseInc;
    OscState(double phase) : _phase(0.), _phaseInc(phase)
    { }
};

class POscillator
{
protected:
    double* _table;           // pointer to waveform lookup table, const because the oscilator doesn't change the table data
    unsigned long int _tableSize;   // wavetable size
    unsigned long int _tableMask;  // wavetable Mask (size -1)
    double _tableMaskF;             // float version

public:

    POscillator() : POscillator(512, SINE) {}
    POscillator(unsigned long int tableSize, WaveType type);

    ~POscillator();

    void setTable(unsigned long int tableSize);

    inline double process(OscState* pState)
    {
        pState->_phase = wrap(pState->_phase, 0., 1.);
        const double output = lerp(pState->_phase * _tableMaskF, _table, _tableMask);
        pState->_phase += pState->_phaseInc;

        return output;
    }
};