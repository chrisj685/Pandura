#include "Filter.h"

double Filter::process(double inputValue)
{
    buf0 += cutoff * (inputValue - buf0);
    buf1 += cutoff * (buf0 - buf1);
    switch (mode)
    {
    case FILTER_MODE_LOWPASS:
        return buf1;
    case FILTER_MODE_HIGHPASS:
        return inputValue - buf0;
    case FILTER_MODE_BANDPASS:
        return buf0 - buf1;
    default:
        return 0.0;
    }
}