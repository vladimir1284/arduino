#include "mean_filter.h"

MeanFilter::MeanFilter()
{
    int i;
    cursor = 0;
    for (i = 0; i < DEPTH; i++)
    {
        data[i] = 0;
    }
}

//-----------------------------------------------------------------------------------
void MeanFilter::insertValue(int val)
{
    int old = data[cursor];
    cumul = cumul + val - old;
    data[cursor] = val;
    cursor = (++cursor) % DEPTH;
}

//-----------------------------------------------------------------------------------
int MeanFilter::getValue()
{
    return cumul / DEPTH;
}