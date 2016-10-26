#include <stdio.h>
#include "envelope/key_scale.h"
#include "operator/operator.h"
#include "operator/logsin_rom.h"
#include "operator/exp_rom.h"
#include "phasegen/phasegen.h"
#include "chipemu.h"

extern int logsin_calc(int i);
extern int exp_calc(int i);

void print_ksl_db(int ksl)
 {
    int fnum;
    int block;

    printf("KSL table with ksl=%d : \r\n",ksl);

    for (fnum=0; fnum<16; fnum++)
    {
        for (block=0; block<8; block++)
        {
            double d;
            d=6.0*8.0*eg_ksl(fnum<<6,block,ksl)/256.0;
            printf("%f ",d);
        }
        printf("\r\n");
    }
    printf("\r\n");
 }

void print_pos_neg(int min, int max)
{
    int attn;

    printf("PCM pos and neg\r\n");
    for (attn=min; attn<=max; attn++)
    {
        printf("%i: %i %i\r\n",attn, pcmconvert(attn,0), pcmconvert(attn,1));
    }
    printf("\r\n");
}

void compare_logsin_rom(void)
{
    int i;
    int total=0;

    for (i=0; i<256; i++)
    {
        if (logsin_calc(i) == logsin_rom[i]) total++;
    }

    if (total == 256)
    {
        printf("logsin calculation match\r\n");
    }
    else
    {
        printf("logsin calculation fail\r\n");
    }
}

void compare_exp_rom(void)
{
    int i;
    int total=0;

    for (i=0; i<256; i++)
    {
        if (exp_calc(i) == exp_rom[i]) total++;
    }

    if (total == 256)
    {
        printf("exp calculation match\r\n");
    }
    else
    {
        printf("exp calculation fail\r\n");
    }
}

void print_phaseinc(int fnum)
{
	int multi;
	int block;

    printf("FNUM table for %d : \r\n",fnum);

    for (multi=0; multi<16; multi++)
    {
        for (block=0; block<8; block++)
        {
            int phinc = calcphaseinc(fnum, block, multi);
            printf("%d ",phinc);
        }
        printf("\r\n");
    }
    printf("\r\n");
}

struct CHIPEMU *dbgchip = NULL;

void debug_writereg(int reg, int data)
{
    if (dbgchip == NULL)
    {
        dbgchip = chipemu_create();
        if (dbgchip == NULL) return;
    }

    if (reg&0x100)
    {
        chipemu_writeport(dbgchip,2,reg);
        chipemu_writeport(dbgchip,3,data);
    }
    else
    {
        chipemu_writeport(dbgchip,0,reg);
        chipemu_writeport(dbgchip,1,data);
    }

}

void debug_newbit(void)
{
    printf("Debugging NEW bit\r\n");

    debug_writereg(0x004,0x01);
    debug_writereg(0x104,0x02);

    debug_writereg(0x105,0x00);

    debug_writereg(0x004,0x03);
    debug_writereg(0x104,0x04);

    debug_writereg(0x105,0x01);

    debug_writereg(0x004,0x05);
    debug_writereg(0x104,0x06);

    debug_writereg(0x105,0x00);

    debug_writereg(0x004,0x07);
    debug_writereg(0x104,0x08);

}
