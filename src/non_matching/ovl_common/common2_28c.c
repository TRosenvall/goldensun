/* OvlFunc_common2_28c  --  asm/overlays/common/common2_a.s
 *
 * BLOCKER CLASS: frame-relative addressing choice.
 * Status: 27 lines against the ROM's 27, FOUR differing, and TWO FLAG FACTS
 * about this translation unit established on the way, which are the reason this
 * park is worth reading.
 *
 * WHAT IT DOES
 * Packs its four arguments into two 8-byte stack records, runs each through
 * OvlFunc_common2_618 into a 0x14-byte result, flips bit 0 of the second
 * result's word at +4, and combines the two into a third 0x14-byte record.
 *
 * TWO THINGS ABOUT THIS TU'S FLAGS, BOTH READ OFF THE ROM RATHER THAN GUESSED
 *
 *   1. IT IS BUILT WITHOUT -mthumb-interwork. The ROM's epilogue is
 *      `pop {r4, r5, r6, pc}`; with interwork gcc emits
 *      `pop {r4, r5, r6} / pop {r0} / bx r0`. The Makefile already has a
 *      COMMON2_CFLAGS rule dropping interwork -- but it is spelled
 *      `common2_c%`, and this is `common2_a`. The rule needs widening.
 *
 *   2. IT IS BUILT WITHOUT -fcall-used-r4. The ROM's prologue pushes r4 and
 *      keeps a frame address in it ACROSS TWO CALLS. With -fcall-used-r4 -- in
 *      GCC296_CFLAGS -- gcc cannot do that, so it reaches past r4 to r8 and
 *      spends four instructions saving and restoring it. Adding
 *      -fcall-saved-r4 takes this function from 33 differing lines to 4.
 *
 * SWEPT, AND THE RESULT BOUNDS THE CLAIM. All 164 parks were re-screened with
 * -fcall-saved-r4. It improves EIGHT and matches NONE:
 *
 *      ovl_77dd1c/200c5b8.c   36 -> 21     rom_a1000/80a9d84.c    33 -> 15
 *      rom_9000/HeightTile_4  22 -> 16     rom_a1000/rom_ad608.c  38 -> 28
 *      ovl_common/common2_254 22 -> 21     rom_b5000/rom_c00d8.c  33 -> 32
 *      ovl_7c6bac/200851c.c  150 -> 143    rom_f9000/rom_f92fc.c 143 -> 141
 *
 * So the r4 question is real and wider than this file, but flipping the flag is
 * not by itself an answer anywhere else. Do not turn it into a global change on
 * the strength of this function.
 *
 * WHAT IS ACTUALLY LEFT, with both flags applied, is four instructions:
 *
 *      rom   str r0, [r4, #0]     ours  str r0, [sp, #0x8]
 *      rom   str r2, [r5, #0]     ours  str r2, [sp, #0x0]
 *      rom   mov r1, r4 / mov r0, r6      ours  mov r0, r6 / mov r1, r4
 *
 * The frame addresses ARE materialised into r4/r5/r6 exactly as the ROM does,
 * and the SECOND store of each pair goes through them (`str r1, [r4, #4]`).
 * Only the offset-zero store of each pair is rewritten sp-relative. gcc folds a
 * frame reference to `[sp, #imm]` when it can and uses the register when the
 * member offset makes it convenient, and nothing in the source chooses between
 * them.
 *
 * WHAT WAS TRIED
 *   - the two stores in the other order (`->y` before `->x`): 11 of 29, worse
 *   - the records as `int[2]` and indexed rather than struct members: identical
 *   - explicit pointer locals for all three frame addresses: this is what
 *     produced the ROM's three `add rN, sp, #imm` up front, and is kept below
 *   - declaration order permuted until the stack layout matched the ROM's
 *     (q at 0x00, p at 0x08, out at 0x10, r at 0x24, s at 0x38) -- that part is
 *     settled and should be kept in any future attempt
 *
 * The last two lines are argument-load order into r0 and r1, which is the
 * argument-precompute class already documented; it may well fall out with the
 * store question rather than needing its own fix.
 *
 * NOTE ON SCREENING THIS FILE: tools/tryc.py takes per-file flags from the
 * Makefile, and there is no rule matching `common2_a`, so a bare screen builds
 * it with interwork and -fcall-used-r4 and reports 33 differing lines. Screen it
 * with  --cflags "-fcall-saved-r4 -mno-thumb-interwork".
 */

struct Vec2 { int x, y; };
struct Rec { int a, b, c, d, e; };

extern void OvlFunc_common2_618(struct Vec2 *src, struct Rec *dst);
extern void OvlFunc_common2_0(struct Rec *a, struct Rec *b, void *out);
extern void OvlFunc_common2_44c(void);

void OvlFunc_common2_28c(int x0, int y0, int x1, int y1)
{
    struct Vec2 q;
    struct Vec2 p;
    struct Rec s;
    struct Rec r;
    struct Rec out;
    struct Vec2 *pp;
    struct Rec *ps;
    struct Vec2 *pq;

    pp = &p;
    ps = &s;
    pq = &q;
    pp->x = x0;
    pp->y = y0;
    pq->x = x1;
    pq->y = y1;
    OvlFunc_common2_618(pp, ps);
    OvlFunc_common2_618(pq, &r);
    r.b ^= 1;
    OvlFunc_common2_0(ps, &r, &out);
    OvlFunc_common2_44c();
}
