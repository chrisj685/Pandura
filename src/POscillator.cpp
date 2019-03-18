#include "POscillator.h"
#include <cmath>

POscillator::POscillator(unsigned long int tableSize, WaveType type) :
    _tableSize(tableSize),
    _tableMask(tableSize - 1), _tableMaskF((double)_tableSize)
{
    _table = new double[_tableSize];

    for (int i = 0; i < _tableSize; i++)
    {
        if (type == SINE)
        {
            _table[i] = sin(i / double(_tableSize) * 2. * M_PI);
        }
        if (type == SAW)
        {
            _table[i] = i / _tableMaskF;
        }
    }
}

void POscillator::setTable(unsigned long int tableSize)
{
    _tableSize = tableSize;
    _tableMask = tableSize - 1;
    _tableMaskF = (double)tableSize;
}

POscillator::~POscillator()
{
    delete[] _table;
}