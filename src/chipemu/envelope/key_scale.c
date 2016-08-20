
#ifndef USE_KSL_ROM
#define USE_KSL_ROM 1
#endif

#if USE_KSL_ROM

#include "ksl_rom.h"

/*
    eg_ksl() calculates how much attenuation from Key Scale Level.
    Returns amount of attenuation in EG values, 32 units = 6dB.
    Needs *8 to get 256 units = 6dB
*/

unsigned int eg_ksl(unsigned int f_num, unsigned int block, unsigned int ksl)
{
    signed int level;

    if (ksl==0) return 0;

    level = ksl_rom[(f_num>>6)&0x0f];
    level = level + (8*(block&7)) - 64;

    if (level < 0) return 0;

    switch (ksl)
    {
        case 0:
        default:
            // no scaling
            return 0;
        case 1:
            // 3dB / octave
            return level<<1;
        case 2:
            // 1.5dB / octave
            return level;
        case 3:
            // 6dB / octave
            return level<<2;
    }

    return 0;

}

#endif
