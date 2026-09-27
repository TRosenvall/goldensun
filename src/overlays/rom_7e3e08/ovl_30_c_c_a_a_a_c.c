/* OvlFunc_957_2008a54  --  0x02008a54, was asm/overlays/rom_7e3e08/ovl_30_c_c_a_a_a_c.s
 * (this function alone), so it converts whole.
 *
 * FAKEMATCH -- two `__asm__ volatile("")` barriers, booked in fakematch.txt.
 * Parked at 3 of 48 as "provably unreachable": a two-sided priority wall in one
 * sched2 region. A volatile asm is a total scheduling barrier that emits no
 * bytes, so each piece schedules alone and the contradiction goes away. Every
 * part is load-bearing: first barrier alone 5, second alone 2, `cnt` initialised
 * at its declaration 2 (the constant hoists above the barrier); the byte must be
 * read as unsigned char and converted to signed char after the second barrier.
 */
#include "gba/io.h"

extern unsigned char ewram_2001004[];

void OvlFunc_957_2008a54(void) {
    unsigned char m = ewram_2001004[0];
    signed char mode;
    int cnt;
    int v;

    __asm__ volatile("");
    cnt = 0x3f42;
    REG_BLDCNT = cnt;
    __asm__ volatile("");
    mode = m;
    if (mode == 0) {
        v = 0x1000;
        REG_BLDALPHA = v;
    } else if (mode == 1) {
        v = 0x0e00;
        REG_BLDALPHA = v;
    } else if (mode == 2) {
        v = 0x0c00;
        REG_BLDALPHA = v;
    } else if (mode == 3) {
        v = 0x0a00;
        REG_BLDALPHA = v;
    } else if (mode == 4) {
        v = 0x0800;
        REG_BLDALPHA = v;
    } else {
        v = 0x0600;
        REG_BLDALPHA = v;
    }
}
