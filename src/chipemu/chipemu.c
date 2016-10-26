#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "chipemu.h"

#if CHIPEMU_DEBUG == 1
#include <stdio.h>
#endif

struct CHIPEMU
{
    uint8_t ioport;
    uint8_t iodata;
    uint16_t *buf;
    int samples;

    uint8_t regbank;
    uint8_t regindex8;
    uint16_t regindex9;
    uint8_t regvalue;

    uint8_t regdata[2][256];

    uint8_t new_flag;

};

void chipemu_write(struct CHIPEMU *instance);

struct CHIPEMU * chipemu_create(void)
{
    return malloc(sizeof(struct CHIPEMU));
}

void chipemu_destroy(struct CHIPEMU * instance)
{
    free(instance);
}

void chipemu_reset(struct CHIPEMU *instance)
{
    memset( instance, 0, sizeof(struct CHIPEMU) );
}

void chipemu_writeport(struct CHIPEMU *instance, int ioport, int iodata)
{
    iodata &= 0xFF;

    instance->ioport=ioport;
    instance->iodata=iodata;

    switch (ioport & 3)
    {
        case 0:
            instance->regbank = 0;
            instance->regindex8 = iodata;
            instance->regindex9 = iodata;
            break;

        case 1:
            instance->regvalue = iodata;
            chipemu_write(instance);
            break;

        case 2:
            instance->regbank = 1;
            instance->regindex8 = iodata;
            instance->regindex9 = 0x100+iodata;
            break;

        case 3:
            instance->regvalue = iodata;
            chipemu_write(instance);
            break;

    }

}

void chipemu_render(struct CHIPEMU *instance, uint16_t *stereobuf, int samples)
{
    instance->buf=stereobuf;
    instance->samples=samples;

    memset( stereobuf, 0, samples * 4);
}

#if CHIPEMU_DEBUG == 1
void chipemu_dumpregs(struct CHIPEMU *instance)
{
    int bank;
    int index;

    for (bank=0; bank<2; bank++)
    {
        printf("Bank %d registers :\r\n",bank);
        for (index=0; index<256; index++)
        {
            if ((index & 0x0F) == 0x00) printf("%02x : ",index);
            if ((index & 0x0F) == 0x08) printf("- ");
            printf("%02x ",instance->regdata[bank][index]);
            if ((index & 0x0F) == 0x0F) printf("\r\n");
        }
    }

}
#endif

void chipemu_write(struct CHIPEMU *instance)
{
    uint8_t bank = instance->regbank;
    uint16_t index = instance->regindex9;
    uint8_t value = instance->regvalue;

    printf("CEMU REGW: %03X = %02X\r\n",index,value);

    if (index == 0x105)
    {
        instance->new_flag = value & 0x01;
        printf("CEMU INFO: set NEW=%d\r\n",value & 0x01);
        return;
    }

    if (!instance->new_flag)
    {
        // not in NEW mode, bank 1 writes fall back to bank 0

        if (bank)
        {
            printf("CEMU INFO: NEW=0 writing register %03X\r\n",index);
        }

        index=index & 0xFF;
        bank=0;
    }

    instance->regdata[bank&1][index&0xFF] = value;

}
