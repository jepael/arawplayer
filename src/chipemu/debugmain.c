
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
    compare_exp_rom();

	print_phaseinc(0);
    print_phaseinc(1);
    print_phaseinc(2);
    print_phaseinc(3);
    print_phaseinc(4);
    print_phaseinc(580); // 0x244 BL=4 MULTI=1 : 440 Hz A-4
    print_phaseinc(975); // 0x3CF BL=4 MULTI=1 : 740 Hz F#4

    debug_newbit();

    return 0;
}
