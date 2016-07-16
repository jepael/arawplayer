#ifndef WAVE_OUT_H_INCLUDED
#define WAVE_OUT_H_INCLUDED

/* for FILE */
#include <stdio.h>
#include <stdint.h>

struct WAVEOUT_CTX
{
    FILE *fp;

    unsigned char header[44];

    int audio_format;
    int num_channels;
    int sampling_rate;
    int byte_rate;
    int block_align;
    int bits_per_sample;

    int bytes_written;
};

#define WAVE_FMT_STEREO_16BIT_PCM 0

#define WAVE_DATA_OFFSET 0x2C

int waveout_configure_format(struct WAVEOUT_CTX *ctx, int format, int rate);
int waveout_open(struct WAVEOUT_CTX *ctx, char *wavname);
int waveout_write(struct WAVEOUT_CTX *ctx, void *buffer, size_t samplecount);
int waveout_close(struct WAVEOUT_CTX *ctx);

#endif // WAVE_OUT_H_INCLUDED
