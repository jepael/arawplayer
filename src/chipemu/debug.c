#include <stdio.h>
#include "envelope/key_scale.h"
#include "operator/operator.h"

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
