#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "rdosraw.h"

void print_usage(void)
{
    printf( "\n"
            "Usage : arawplayer <file>\n"
          );
}

int main(int argc, char *argv[])
{
    struct RDOSRAW_CTX *raw = NULL;

    printf("RAW player\n");

    if (argc != 2)
    {
        print_usage();
        return 0;
    }

    printf("File name %s\n",argv[1]);

    if (RDOSRAW_create_ctx(&raw))
    {
        printf("Unable to allocate context\n");
        return 0;
    }

    raw->file_name=argv[1];

    if (RDOSRAW_load_file(&raw)==0)
    {
        int playing=1;

        uint32_t tick_modulus=0;
        uint32_t total_samples=0;
        uint32_t total_ticks=0;

        while(playing==1)
        {

            int event = RDOSRAW_get_event(&raw);

            if (event)
            {
                if (event&EVENT_CHIP_WRITE)
                {
                    event &=~ EVENT_CHIP_WRITE;

                    /*
                    uint32_t chipbase=raw->chip_base;
                    uint32_t chipreg=raw->chip_regindex;
                    uint32_t chipdat=raw->chip_regdata;
                    */

                    // printf("Write chip %1X reg %2X dat %2X\n",chipbase, chipreg, chipdat);
                }

                if (event&EVENT_TIMER_TICKS)
                {
                    event &=~ EVENT_TIMER_TICKS;

                    uint32_t timerdelay=raw->delay_ticks;
                    // printf("Timer delay %d ticks\n",timerdelay);

                    total_ticks += timerdelay;

                    tick_modulus += timerdelay;
                    uint32_t samples = tick_modulus/24;
                    tick_modulus %= 24;

                    total_samples += samples;

                    // printf("Generate %d samples, tick modulus %d\n",samples, tick_modulus);

                }

                if (event&EVENT_TIMER_SET)
                {
                    event &=~ EVENT_TIMER_SET;

                    double timerhz=(14.318180e6/12.0)/(raw->timer_period);
                    printf("Timer set to %f Hz\n",timerhz);
                }

                if (event&EVENT_CHIP_BASECHANGE)
                {
                    event &=~ EVENT_CHIP_BASECHANGE;

                    printf("Chip baseport change\n");
                }

                if (event&EVENT_CHIP_BASE0)
                {
                    event &=~ EVENT_CHIP_BASE0;

                    printf("Chip baseport base 0\n");
                }

                if (event&EVENT_CHIP_BASE1)
                {
                    event &=~ EVENT_CHIP_BASE1;

                    printf("Chip baseport base 1\n");
                }

                if (event & EVENT_END_MARKER)
                {
                    event &=~ EVENT_END_MARKER;
                    printf("End of data marker found\n");
                    // NOTE: if looping, must continue playing
                    // playing = 0;
                }

                if (event & EVENT_PLAY_STOP)
                {
                    event &=~ EVENT_PLAY_STOP;
                    printf("Finished playback!\n");
                    playing=0;
                }

                if (event) printf("ERROR: Unhandled event(s) %4X\n", event);
            }
            else
            {
                printf("ERROR: No event!\n");
                playing=0;
            }

        }
        printf("Generated %d samples in %d timer ticks\n",total_samples,total_ticks);
    }

    RDOSRAW_destroy_ctx(&raw);

    return 0;
}
