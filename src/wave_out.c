
#include "wave_out.h"

void ser_fourcc(unsigned char *dst, char *src)
{
    *dst++=*src++;
    *dst++=*src++;
    *dst++=*src++;
    *dst++=*src++;
}

void ser_32(unsigned char *dst, uint32_t val)
{
    *dst++=val;
    *dst++=val>>8;
    *dst++=val>>16;
    *dst++=val>>24;
}

void ser_16(unsigned char *dst, uint32_t val)
{
    *dst++=val;
    *dst++=val>>8;
}

void waveout_update_header(struct WAVEOUT_CTX *ctx)
{

    unsigned char *ph=ctx->header;

    ser_fourcc(ph, "RIFF");
    ph=ph+4;
    ser_32(ph, 4+(8+16)+(8+ctx->bytes_written) );
    ph=ph+4;
    ser_fourcc(ph, "WAVE");
    ph=ph+4;

    ser_fourcc(ph, "fmt "); ph=ph+4;
    ser_32(ph, 16); ph=ph+4;
    ser_16(ph, ctx->audio_format); ph=ph+2;
    ser_16(ph, ctx->num_channels); ph=ph+2;
    ser_32(ph, ctx->sampling_rate); ph=ph+4;
    ser_32(ph, ctx->byte_rate); ph=ph+4;
    ser_16(ph, ctx->block_align); ph=ph+2;
    ser_16(ph, ctx->bits_per_sample); ph=ph+2;

    ser_fourcc(ph, "data");
    ph=ph+4;
    ser_32(ph, ctx->bytes_written);

}

int waveout_configure_format(struct WAVEOUT_CTX *ctx, int format, int rate)
{

    format = WAVE_FMT_STEREO_16BIT_PCM;

    if (format==WAVE_FMT_STEREO_16BIT_PCM)
    {
        ctx->audio_format=1;
        ctx->num_channels=2;
        ctx->bits_per_sample=16;
        ctx->block_align=4;
        ctx->sampling_rate=rate;
        ctx->byte_rate=rate*4;
        return 0;
    }

    return 1;
}

int waveout_writeheader(struct WAVEOUT_CTX *ctx)
{
    if (ctx->fp)
    {
        if (fseek (ctx->fp,0,SEEK_SET)) return 1;
        // printf("header size %lu\n",sizeof(ctx->header));
        int written=fwrite(ctx->header, 1, sizeof(ctx->header), ctx->fp);
        if (written != sizeof(ctx->header)) return 1;
    }
    return 0;

}

int waveout_open(struct WAVEOUT_CTX *ctx, char *wavname)
{
    int errorcode = 0;

    ctx->bytes_written = 0;
    /*
    if (wavname==NULL)
    {
        printf("File name is null\n");
    }
    else
    {
        printf("opening %s\n",wavname);
    }
    */

    ctx->fp = fopen( wavname, "wb");

    if (ctx->fp == NULL)
    {
        // printf("open failed\n");
        errorcode=1;
        return errorcode;
    }

    waveout_update_header(ctx);
    waveout_writeheader(ctx);

    return errorcode;
}

int waveout_write(struct WAVEOUT_CTX *ctx, void *buffer, size_t samplecount)
{
    int errorcode = 0;

    if (ctx->fp)
    {
        int writebytecount = ctx->block_align*samplecount;

        int written = fwrite(buffer, 1, writebytecount, ctx->fp);

        ctx->bytes_written += written;

        if (written != writebytecount) errorcode=1;

        // printf("File written %d %d\n",written, ctx->bytes_written);

    }

    return errorcode;
}

int waveout_close(struct WAVEOUT_CTX *ctx)
{
    int errorcode = 0;
    // printf("Closing, write count is %d\n",ctx->bytes_written);
    waveout_update_header(ctx);
    // printf("Closing, write count is %d\n",ctx->bytes_written);
    if (waveout_writeheader(ctx)) errorcode=1;
    if (fclose(ctx->fp)) errorcode=1;
    ctx->fp=0;
    return errorcode;
}
