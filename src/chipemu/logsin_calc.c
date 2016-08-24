#include "math.h"

int logsin_calc(int i)
{
    int value;

    value=((-256.0*log2(sin((i+0.5)*2.0*M_PI/1024.0)))+0.5);

    return value;
}
