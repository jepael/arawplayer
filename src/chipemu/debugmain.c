
#include "debug.h"

int main(int argc, char *argv[])
{
    argc=argc;
    argv=argv;

    print_ksl_db(0);
    print_ksl_db(1);
    print_ksl_db(2);
    print_ksl_db(3);

    print_pos_neg(0,0xC01);

    compare_logsin_rom();

    return 0;
}
