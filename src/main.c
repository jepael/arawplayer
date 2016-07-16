#include <stdio.h>
#include <stdlib.h>

void print_usage(void)
{
    printf( "\n"
            "Usage : arawplayer <file>\n"
          );
}

int main(int argc, char *argv[])
{
    printf("RAW player\n");

    if (argc != 2)
    {
        print_usage();
        return 0;
    }

    printf("File name %s\n",argv[1]);

    return 0;
}
