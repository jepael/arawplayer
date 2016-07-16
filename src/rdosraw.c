#include "rdosraw.h"

/* for malloc() */
#include <stdlib.h>
/* for memset() */
#include <string.h>
/* for file I/O */
#include <stdio.h>

#ifndef RDOSRAW_DEBUG
#define RDOSRAW_DEBUG 0
#endif

#if RDOSRAW_DEBUG > 0
#define DEBUG_PRINTF printf
#define DEBUG_UNUSED_VAR(x)
#else
#define DEBUG_PRINTF(...)
#define DEBUG_UNUSED_VAR(x) {if(x) x=0;}
#endif

static unsigned int get_uint16le(unsigned char *buffer, int offset);

int RDOSRAW_create_ctx(struct RDOSRAW_CTX **ctx)
{
    if (*ctx) RDOSRAW_destroy_ctx(ctx);

    (*ctx) = malloc(sizeof(struct RDOSRAW_CTX));

    if (*ctx == NULL) return 1;

    memset( (*ctx), 0, sizeof(struct RDOSRAW_CTX));

    (*ctx)->file_size_limit = RDOSRAW_MAX_FILESIZE;

    return 0;
}

int RDOSRAW_destroy_ctx(struct RDOSRAW_CTX **ctx)
{
    if (*ctx)
    {
        if ((*ctx)->file_buffer) free((*ctx)->file_buffer);
    }

    free(*ctx);
    *ctx = NULL;
    return 0;
}

int RDOSRAW_load_file(struct RDOSRAW_CTX **ctx)
{
    char signature[RDOSRAW_SIGNATURE_LENGTH] = RDOSRAW_SIGNATURE_STRING;
    int filesize = 0;
    FILE *fp = NULL;

    if ((*ctx)->file_buffer)
    {
        free((*ctx)->file_buffer);
        (*ctx)->file_buffer = NULL;
    }

    (*ctx)->file_size_loaded = 0;

    DEBUG_PRINTF("Opening %s\n",(*ctx)->file_name);

    fp=fopen((*ctx)->file_name, "rb");

    if (fp==0)
    {
        /* could not open file */
        return 1;
    }


    /* obtain file size */
    fseek (fp , 0 , SEEK_END);
    filesize = ftell (fp);
    rewind (fp);

    /* limit how much to allocate and load */
    if (filesize > (*ctx)->file_size_limit) filesize = (*ctx)->file_size_limit;

    (*ctx)->file_buffer = malloc(filesize);

    if ((*ctx)->file_buffer)
    {
        (*ctx)->file_size_loaded = fread ((*ctx)->file_buffer, 1, filesize, fp);
    }

    if (fclose(fp)) return 1;

    if ((*ctx)->file_buffer == NULL) return 1;

    if ((*ctx)->file_size_loaded & 1)
    {
        DEBUG_PRINTF("Warning: loaded data count was not multiple of two - rounding down\n");
        (*ctx)->file_size_loaded--;
    }

    if ((*ctx)->file_size_loaded < RDOSRAW_MIN_FILESIZE) return 1;

    /* Check signature */
    if (memcmp( (*ctx)->file_buffer+RDOSRAW_OFFSET_SIGNATURE,
                signature,
                RDOSRAW_SIGNATURE_LENGTH)!=0) return 1;

    (*ctx)->timer_period = get_uint16le((void*)(*ctx)->file_buffer, RDOSRAW_OFFSET_TIMERPERIOD);

    if ((*ctx)->timer_period == 0) (*ctx)->timer_period = 0x10000;

    (*ctx)->play_offset = RDOSRAW_OFFSET_DATA;

    (*ctx)->loop_times = 1;

    return 0;
}

unsigned int RDOSRAW_get_event(struct RDOSRAW_CTX **ctx)
{
    unsigned int events = 0;
    unsigned int offset = 0;
    int data;
    int cmd;

    DEBUG_UNUSED_VAR(offset);

    if ((*ctx)->play_offset == (*ctx)->file_size_loaded)
    {
        DEBUG_PRINTF("End of file reached\n");
        (*ctx)->play_offset = RDOSRAW_OFFSET_DATA;

        (*ctx)->timer_period = get_uint16le((void*)(*ctx)->file_buffer, RDOSRAW_OFFSET_TIMERPERIOD);

        if ((*ctx)->timer_period == 0) (*ctx)->timer_period = 0x10000;

        events |= EVENT_TIMER_SET;
        (*ctx)->chip_base=0;
        events |= EVENT_CHIP_BASE0;
        events |= EVENT_CHIP_BASECHANGE;

        if ((*ctx)->loop_times==0)
        {
            events |= EVENT_PLAY_RESTART;
            return events;
        }
        else
        {
            if ((*ctx)->loop_times>0)
            {
                (*ctx)->loop_times--;
                if ((*ctx)->loop_times==0)
                {
                    events |= EVENT_PLAY_STOP;
                    return events;
                }
                else
                {
                    events |= EVENT_PLAY_RESTART;
                    return events;
                }
            }

        }
    }

    offset = (*ctx)->play_offset;

    data=(unsigned char)(*ctx)->file_buffer[(*ctx)->play_offset];
    (*ctx)->play_offset++;
    cmd=(unsigned char)(*ctx)->file_buffer[(*ctx)->play_offset];
    (*ctx)->play_offset++;

    switch (cmd)
    {
        case RDOSRAW_EVENTREG_DELAY:

            events |= EVENT_TIMER_TICKS;
            /*
                - Max timer period is 65536 timer cycles (0x10000)
                - Max periods is 255 (0xFF)
                - Max return value is 16711680 or 0xFF0000 ticks
            */
            (*ctx)->delay_ticks = (*ctx)->timer_period * data;
            DEBUG_PRINTF("%4X: Delay %2X\n",offset, data);
            break;

        case RDOSRAW_EVENTREG_CONTROL:

            switch (data)
            {
                case RDOSRAW_CONTROL_SETTIMER:

                    (*ctx)->timer_period = get_uint16le((void*)(*ctx)->file_buffer, (*ctx)->play_offset );
                    (*ctx)->play_offset += 2;

                    if ((*ctx)->timer_period == 0) (*ctx)->timer_period = 0x10000;

                    events |= EVENT_TIMER_SET;

                    DEBUG_PRINTF("%4X: Control timer data %2X\n",offset, (*ctx)->timer_period);
                    break;

                case RDOSRAW_CONTROL_CHIPBASE0:
                    (*ctx)->chip_base=0;
                    events |= EVENT_CHIP_BASE0;
                    events |= EVENT_CHIP_BASECHANGE;
                    DEBUG_PRINTF("%4X: Control base0 (chip%2X)\n",offset, data);
                    break;

                case RDOSRAW_CONTROL_CHIPBASE1:
                    (*ctx)->chip_base=1;
                    events |= EVENT_CHIP_BASE1;
                    events |= EVENT_CHIP_BASECHANGE;
                    DEBUG_PRINTF("%4X: Control base1 (chip%2X)\n",offset, data);
                    break;

                default:
                    events |= EVENT_UNKNOWN;
                    events |= EVENT_UNKNOWN_CONTROL;
                    DEBUG_PRINTF("%4X: Control unknown data %2X\n",offset, data);
                    break;
            }


            break;

        default:
            if ((cmd==0xFF) && (data==0xFF))
            {
                /* End of data marker */
                (*ctx)->play_offset = (*ctx)->file_size_loaded;
                events |= EVENT_END_MARKER;
                DEBUG_PRINTF("%4X: End data marker\n",offset);
            }
            else
            {
                (*ctx)->chip_regindex=cmd;
                (*ctx)->chip_regdata=data;
                events |= EVENT_CHIP_WRITE;
                DEBUG_PRINTF("%4X: Write cmd %2X data %2X\n",offset,cmd,data);
            }
            break;
    }

    return events;
}

static unsigned int get_uint16le(unsigned char *buffer, int offset)
{
    unsigned int val;
    val=buffer[offset];

    val=val+(buffer[offset+1]<<8);

    return val;
}
