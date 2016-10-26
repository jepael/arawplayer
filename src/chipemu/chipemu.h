#ifndef CHIPEMU_H_INCLUDED
#define CHIPEMU_H_INCLUDED

#include <stdint.h>

#define CHIPEMU_DEBUG 1

struct CHIPEMU * chipemu_create(void);
void chipemu_destroy(struct CHIPEMU * instance);

void chipemu_reset(struct CHIPEMU *instance);
void chipemu_writeport(struct CHIPEMU *instance, int ioport, int iodata);
void chipemu_render(struct CHIPEMU *instance, uint16_t *stereobuf, int samples);

#if CHIPEMU_DEBUG == 1
void chipemu_dumpregs(struct CHIPEMU *instance);
#endif

#endif // CHIPEMU_H_INCLUDED
