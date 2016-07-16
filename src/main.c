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

        while(playing==1)
        {

            int event = RDOSRAW_get_event(&raw);

            if (event & EVENT_PLAY_STOP)
            {
                playing=0;
            }

        }
    }

    RDOSRAW_destroy_ctx(&raw);

    return 0;
}
