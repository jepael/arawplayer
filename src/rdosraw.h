#ifndef RDOSRAW_H_INCLUDED
#define RDOSRAW_H_INCLUDED

/* Even without any register data, file is at least 12 bytes */
#define RDOSRAW_MIN_FILESIZE        (12L)
/* Arbitrary maximum supported file size */
#define RDOSRAW_MAX_FILESIZE        (1L*1024L*1024L)
/* Header signature length */
#define RDOSRAW_SIGNATURE_LENGTH    8
/* Header signature string */
#define RDOSRAW_SIGNATURE_STRING    "RAWADATA"

#define RDOSRAW_OFFSET_SIGNATURE    0
#define RDOSRAW_OFFSET_TIMERPERIOD  8
#define RDOSRAW_OFFSET_DATA         10

#define RDOSRAW_EVENTDATA_END       0xFFFF
#define RDOSRAW_EVENTREG_DELAY      0x00
#define RDOSRAW_EVENTREG_CONTROL    0x02

#define RDOSRAW_CONTROL_SETTIMER    0x00
#define RDOSRAW_CONTROL_CHIPBASE0   0x01
#define RDOSRAW_CONTROL_CHIPBASE1   0x02

#define EVENT_PLAY_RESTART          0x0001
#define EVENT_PLAY_STOP             0x0002
#define EVENT_TIMER_TICKS           0x0004
#define EVENT_END_MARKER            0x0008
#define EVENT_TIMER_SET             0x0010
#define EVENT_CHIP_WRITE            0x0020
#define EVENT_CHIP_BASE0            0x0040
#define EVENT_CHIP_BASE1            0x0080
#define EVENT_CHIP_BASECHANGE       0x0100
#define EVENT_UNKNOWN_CONTROL       0x0200
#define EVENT_UNKNOWN               0x0400

struct RDOSRAW_CTX {
    char    *file_name;
    int     file_size_limit;
    int     file_size_loaded;
    char    *file_buffer;
    int     timer_period;
    int     play_offset;
    int     loop_times;
    int     delay_ticks;
    int     chip_base;
    int     chip_regindex;
    int     chip_regdata;
};

int RDOSRAW_create_ctx(struct RDOSRAW_CTX **ctx);
int RDOSRAW_destroy_ctx(struct RDOSRAW_CTX **ctx);
int RDOSRAW_load_file(struct RDOSRAW_CTX **ctx);
//int RDOSRAW_play_tick(struct RDOSRAW_CTX **ctx);
unsigned int RDOSRAW_get_event(struct RDOSRAW_CTX **ctx);


#endif // RDOSRAW_H_INCLUDED
