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
	uint8_t regindex;
	uint8_t regvalue;

	uint8_t regdata[2][256];
};

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
	instance->ioport=ioport;
	instance->iodata=iodata;

	switch (ioport & 3)
	{
		case 0:
			instance->regbank = 0;
			instance->regindex = iodata;
			break;

		case 1:
			instance->regvalue = iodata;
			instance->regdata[instance->regbank&1][instance->regindex] = iodata;
			break;

		case 2:
			instance->regbank = 1;
			instance->regindex = iodata;
			break;

		case 3:
			instance->regvalue = iodata;
			instance->regdata[instance->regbank&1][instance->regindex] = iodata;
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
