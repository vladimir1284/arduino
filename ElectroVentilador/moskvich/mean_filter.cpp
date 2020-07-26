#include "mean_filter.h"

MeanFilter::MeanFilter()
{
    int i;
    cursor = 0;
    cumul = 0;
    for (i = 0; i < DEPTH; i++)
    {
        data[i] = 0;
    }
}

//-----------------------------------------------------------------------------------
void MeanFilter::insertValue(float val)
{
    float old = data[cursor];
    cumul = cumul + val - old;
    data[cursor] = val;
    cursor = (cursor + 1) % DEPTH;
}

//-----------------------------------------------------------------------------------
float MeanFilter::getValue()
{
    return cumul / DEPTH;
}