#include <stdio.h>
#include "envelope/key_scale.h"

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
