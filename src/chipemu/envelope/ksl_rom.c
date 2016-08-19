/*

    Key Scale Level look up table from actual chip.

    Table implemented with reference to decapsulation
    pictures of YM3812 (OPL2) chip. The values are same in
    YMF262 (OPL3) chip.

    Thanks to
        -Decapsulation pictures taken by Matthew Gambrell and Olli Niemitalo
        -Steffen Ohrendorf for helping how to interpret the ROM values

    KSL ROM is 16-entry table of 7 bits.

    ROM bits:

    0000000 = 0x00
    1000000 = 0x40
    1010000 = 0x50
    1011010 = 0x5A
    1100000 = 0x60
    1100110 = 0x66
    1101010 = 0x6A
    1101110 = 0x6E
    1110000 = 0x70
    1110100 = 0x74
    1110110 = 0x76
    1111000 = 0x78
    1111010 = 0x7A
    1111100 = 0x7C
    1111110 = 0x7E
    0000001 = 0x01

    It appears the bit order is 5432106.
    After moving the rightmost bit b6 to the leftmost position:

    0000000 = 0x00
    0100000 = 0x20
    0101000 = 0x28
    0101101 = 0x2D
    0110000 = 0x30
    0110011 = 0x33
    0110101 = 0x35
    0110111 = 0x37
    0111000 = 0x38
    0111010 = 0x3A
    0111011 = 0x3B
    0111100 = 0x3C
    0111101 = 0x3D
    0111110 = 0x3E
    0111111 = 0x3F
    1000000 = 0x40

*/

#include "ksl_rom.h"

unsigned short int ksl_rom[16] = {
    0, 32, 40, 45, 48, 51, 53, 55, 56, 58, 59, 60, 61, 62, 63, 64
};
