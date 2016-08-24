#include "math.h"

int exp_calc(int i)
{
    int value;

    value=(((1024.0*exp2(i/256.0))+0.5)-1024.0);

    return value;
}
