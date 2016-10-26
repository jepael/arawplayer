#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
/* for memset() */
#include <string.h>

#include "wave_out.h"
#include "rdosraw.h"
#include "chipemu/chipemu.h"

void print_usage(void)
{
    printf( "\n"
            "Usage : arawplayer <rawfile> [wavfile]\n"
          );
}

/* 16k samples for 2 channels and 16-bit samples is 64k */
#define WAVEBUFFER_SAMPLES 16384
#define WAVEBUFFER_INDEXES (2*WAVEBUFFER_SAMPLES)

/*
    14318180/288 = 49715,902777778, rounding up.
*/
#define CHIP_SAMPLING_RATE 49716

int main(int argc, char *argv[])
{
    struct CHIPEMU *chip = NULL;
    struct RDOSRAW_CTX *raw = NULL;

    uint16_t *wavebuffer[WAVEBUFFER_INDEXES];

    struct WAVEOUT_CTX wavfile;
    char *wavname = "out.wav";

    printf("RAW player\n");

    if (argc < 2)
    {
        print_usage();
        return 0;
    }

    if (argc == 3)
    {
        wavname = argv[2];
    }

    chip = chipemu_create();
    if (chip == NULL)
    {
        printf("Could not allocate chip instance\n");
        return 0;
    }

    waveout_configure_format(&wavfile, WAVE_FMT_STEREO_16BIT_PCM, CHIP_SAMPLING_RATE);

    if (waveout_open(&wavfile, wavname))
    {
        printf("Could not open output file\n");
        return 0;
    }

    printf("Input file name %s\n",argv[1]);

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

        // clear this if nothing generates samples
        memset(wavebuffer, 0, sizeof(wavebuffer));

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

                    switch (raw->chip_base)
                    {
                        case 0:
                            chipemu_writeport(chip, 0, raw->chip_regindex);
                            chipemu_writeport(chip, 1, raw->chip_regdata);
                            break;

                        case 1:
                            chipemu_writeport(chip, 2, raw->chip_regindex);
                            chipemu_writeport(chip, 3, raw->chip_regdata);
                            break;

                        default:
                            break;
                    }

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

                    while (samples>0)
                    {
                        uint32_t x = samples;
                        if (x>WAVEBUFFER_SAMPLES) x=WAVEBUFFER_SAMPLES;
                        // printf("write %d samples\n",x);

                        if (waveout_write(&wavfile, wavebuffer, x))
                        {printf("write failed\n");}
                        samples -= x;
                    }

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

    waveout_close(&wavfile);

    RDOSRAW_destroy_ctx(&raw);

    chipemu_dumpregs(chip);
    chipemu_destroy(chip);

    return 0;
}
