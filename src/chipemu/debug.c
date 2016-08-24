#include <stdio.h>
#include "envelope/key_scale.h"
#include "operator/operator.h"
#include "operator/logsin_rom.h"

extern int logsin_calc(int i);

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
