#include "tpila.h"

// Constructor
TPila::TPila()
{
    tope = -1;
}

//--------------------------------------------------------------------
void TPila::push(int val)
{
    if (tope < DEPTH - 1)
    {
        tope++;
        data[tope] = val;
    }
}
//--------------------------------------------------------------------
int TPila::pop()
{
    if (tope >= 0)
        return data[tope--];
    else
        return -1; //data[0];
}
//--------------------------------------------------------------------
int TPila::get(int n)
{
    if (n >= 0 && n <= tope)
        return data[n];
    else
        return data[0];
}
//--------------------------------------------------------------------
bool TPila::empty()
{
    if (tope == -1)
        return true;
    return false;
}

