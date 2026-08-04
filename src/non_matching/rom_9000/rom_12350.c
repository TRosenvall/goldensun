/* Func_8012350 @ 0x08012350
 *
 * Source asm: goldensun/asm/rom_9000/rom_1219c_a_c_c_a.s
 *
 * NOT SPLIT. The .s still holds all four of its functions.
 *
 * Spins until two fields of the block at iwram_3001e70 both drop to 0xff or
 * below, giving up after 0x12c frames, then clears a third field.
 *
 * Blocker: THE PRE-HEADER LOAD MERGE. 26 of 27, short by exactly one
 * instruction. See src/non_matching/preheader_load_merge.c for the class, its
 * three members, everything tried across them, and why the compiler-flag
 * attack is closed rather than open.
 *
 * Two things specific to this member:
 *
 *  * the pointer lands in r5 in BOTH the ROM and ours, because it is live
 *    across the WaitFrames call and has to be callee-saved. The
 *    address-register-reuse residue that shows up elsewhere in this family
 *    (src/non_matching/rom_9000/rom_c0cc.c) does not appear here.
 *  * the 0x12c limit is `mov r3, #0x96 / lsl r3, #1` INSIDE the loop, so it is
 *    written as a named local. Left as a literal it hoists into the pre-header.
 *    That is the same direction as Func_80064b8 and the OPPOSITE of
 *    src/rom_b5000/rom_bffb8_a_a_a_b.c -- the loop-invariant lever runs both
 *    ways and has to be read off the ROM each time.
 */
#include "gba/types.h"

extern u32 iwram_3001e70;
extern void WaitFrames(s32 n);

struct T { s32 pad_00; s32 f4; s32 f8; s32 fc; };

void Func_8012350(void)
{
    struct T *p;
    s32 i;
    s32 v;
    s32 lim;

    p = (struct T *)iwram_3001e70;
    v = p->f4;
    i = 0;
    goto check;
loop:
    WaitFrames(1);
    lim = 0x96 << 1;
    i++;
    if (i >= lim)
        goto out;
    v = p->f4;
check:
    if (v > 0xff)
        goto loop;
    if (p->f8 > 0xff)
        goto loop;
out:
    p->fc = 0;
}
