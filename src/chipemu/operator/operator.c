
#ifndef USE_EXP_ROM
#define USE_EXP_ROM 1
#endif

#if USE_EXP_ROM

#include "exp_rom.h"

signed int pcmconvert(unsigned int logval, unsigned int sign)
{
    unsigned int tmp = 0;

/*
    Use the 8 LSBs of log input value as index to 256-entry 10-bit exponential table,
    but invert the index bits to use the table backwards.

    Add the 11th "hidden bit" that is not stored in the table because it is always on
    to get a 11-bit result with range of 1024..2042

    Double the value to get 12-bit positive output range 2048..4084.

    Shift the value with MSBs of input value, as each input unit of 256 halves the value.

    The shifted range will be 0..4084. If shift value is 12 or larger, output is always 0.

    The chip just uses one's complement for negative values.
    If sign is negative, then invert the bits to get -4085..-1 negative output range.

*/
    if ( logval < 0x0C00 )
    {
        tmp = (1024 + exp_rom[ (~logval) & 0xFF]) << 1;
        tmp = tmp >> (logval >> 8);
    }

    if (sign) tmp = ~tmp;

    return tmp;
}

#endif
