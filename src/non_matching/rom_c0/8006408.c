/* Func_8006408 -- 0x08006408 -- asm/rom_c0/rom_5cf8_a_a_c_a.s
 *
 * BLOCKER: a saved parameter plus register rotation. 29 of 29, two lines long.
 *
 * Refuses if a state word is non-zero, otherwise sets two control bytes, clears
 * a halfword, stores its argument into the state word and clears a flag byte --
 * all under IME-off -- and returns 0.
 *
 * Every element reproduces: the early refusal returning -1, the
 * `savedIme = REG_IME; SET_IO(REG_IME, REG_ADDR_IME)` pair, the restore, and
 * the store of the LOADED value (`strh r4`) rather than a literal zero into
 * ewram_2002238, which is the ROM reusing the word it already tested.
 *
 * THE TWO EXTRA LINES ARE A PARAMETER SAVE. The ROM keeps its argument in r0
 * for the whole body and stores it straight out (`str r0, [r5]`); gcc copies it
 * to r5 at entry (`mov r5, r0`) because the argument has to survive two
 * volatile REG_IME accesses, and the base registers rotate around that.
 *
 * MEASURED: naming a pointer to the state word -- `q = &ewram_20023ac`, used
 * for BOTH the initial load and the final store, which is the twice-used case
 * that IS reachable -- is BYTE-IDENTICAL at 29.
 *
 * The reason is worth recording, because the twice-used rule normally works:
 * `&ewram_20023ac` is a COMPILE-TIME CONSTANT address. There is no value to
 * keep alive, so gcc folds the local away and rematerialises the address at
 * each use. The reachable naming cases -- Func_801a778's `+0x39e` pointer,
 * Func_80a195c's counter -- all name something COMPUTED. Naming a fixed
 * address names nothing.
 *
 * That is the same boundary as the unreachable copies (Func_80a8b10,
 * Func_80e38b8, HeightTile_B), reached from a different direction: a local
 * only earns a register when it holds something gcc cannot recompute for free.
 */
#include "gba/io.h"

extern int ewram_20023ac;
extern unsigned char ewram_2002220[];
extern unsigned short ewram_2002238;
extern unsigned char ewram_20023a4;

int Func_8006408(int v)
{
    unsigned char *p;
    int cur;
    u16 savedIme;

    cur = ewram_20023ac;
    p = ewram_2002220;
    if (cur != 0)
        return -1;
    savedIme = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    p[1] = 0x81;
    ewram_2002238 = cur;
    p[0] = 1;
    ewram_20023ac = v;
    ewram_20023a4 = 0;
    SET_IO(REG_IME, savedIme);
    return 0;
}
