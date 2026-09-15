/* Func_942e0 -- 0x080942e0, asm/rom_8a000/rom_93304_c_a.s
 *
 * BLOCKER CLASS: sched2 PLACEMENT OF A BARE CONSTANT. Two encodings of 52,
 * size exact (116 bytes, 52 instructions), and everything else aligned.
 *
 * THE READING IS SETTLED. Two separate defects were found and both are closed;
 * only record the residue below as open.
 *
 * 1. THE gState OFFSET MUST BE TWO STATEMENTS. The ROM computes the address at
 *    runtime -- `ldr r3,=gState / mov r0,#0xfa / lsl r0,#1 / add r3,r0` -- where
 *    a single `*(int *)(gState + (0xfa << 1))` folds the whole offset into the
 *    pool word and emits ONE instruction. Splitting the base from the offset is
 *    what stops the fold:
 *
 *        gs = gState;
 *        gs += 0xfa << 1;
 *
 *    That is not invented for this function: it is the idiom 198 landed sites
 *    already use, e.g. src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_b.c, and
 *    `mov r2,#250 / lsl r2,r2,#1` there is byte-for-byte this function's
 *    `mov r0,#0xfa / lsl r0,#1`. Worth 4 bytes and the whole reloc layout.
 *
 * 2. THE ZERO MUST BE A NAMED LOCAL, ASSIGNED AFTER THE CALL. The ROM holds 0
 *    in r1 and reuses it for `strb r1,[r6]`, `str r1,[r5,#0x24]` and
 *    `str r1,[r5,#0x2c]`; three bare literals get rematerialised as two `mov`s
 *    because r3 is clobbered by the mask arithmetic in between. `int z = 0;`
 *    written AFTER the _Sprite_AddLayer call is exact for that. Assigning it
 *    BEFORE the call is much worse -- 124 bytes against 116 -- because z then
 *    crosses the call and takes a callee-saved register of its own.
 *
 * WHAT IS LEFT, AND WHY IT NEEDS A PIN TO GET THERE. The actor pointer and the
 * part pointer hold each other's registers: the ROM has actor in r5 and part in
 * r6, we get part in r5. `.18.greg` says `;; 0 regs to allocate`, so BOTH are
 * local allocnos and this is local-alloc's priority formula
 * (floor_log2(n_refs) * n_refs * size / (death - birth)) deciding it, not a
 * global auction. The actor has many more refs; the part has a far shorter live
 * range, and the range wins.
 *
 * Declaration order is INERT here -- p before a, layer before p, gs first, all
 * measured 15 differing, unchanged. `z` declared and assigned first is 124
 * bytes. Reordering the two stores is 13 at best. Dropping the named part
 * pointer and writing `a->part` twice is WORSE (108 bytes, 50 instructions):
 * gcc collapses it and never keeps the value across the call, where the ROM's
 * `add r6,#0x26` after the `bl` proves it does.
 *
 * PINNING EITHER MEMBER TAKES IT 15 -> 2, and it really is either:
 * `register struct Actor *a __asm__("r5")` and
 * `register u8 *p __asm__("r6")` measure identically. That matches the recorded
 * rule for a contest that is decided by REMOVING a variable from it, and is the
 * evidence this is not a priority ordering that some spelling could shift.
 *
 * THE LAST TWO ARE A BARE CONSTANT sched2 PLACES EARLY:
 *
 *     rom    strb r1,[r6] / mov r3,#0xf / strb r3,[r0,#5]
 *     ours   mov r3,#15   / strb r1,[r6] / strb r3,[r0,#5]
 *
 * The two STORES stay in ROM order -- both are u8 lvalues, so they share an
 * alias set and sched2 may not swap them. It is the constant's `mov`, which
 * depends on nothing, that floats up into the gap.
 *
 * -fno-schedule-insns2 IS RULED OUT AND THE MEASUREMENT IS THE POINT. The tree
 * already has SCHED2_CFLAGS with two rules, and Makefile:340 describes this very
 * symptom ("hoists `mov r0,#0x8f / lsl r0,#4`"), so it was the obvious next
 * move. It makes this function WORSE, 2 differing -> 15, on every pinned
 * candidate, and 15 -> 21 unpinned. THE ROM WAS BUILT WITH sched2 ON and its
 * order IS the scheduled order; ours differs by a priority tie inside the pass,
 * not by the pass running. Do not add a SCHED2 rule for this file.
 *
 * ALSO TRIED, all 2: the store spelled `p += 0x26; *p = z;`, the constant taken
 * into its own local `f = 0xf` assigned between the two stores, and the
 * -fno-schedule-insns2 sweep above at both pin sites.
 *
 * NEXT: the open question is what gives a bare `mov #K` a LOWER sched2 priority
 * than an adjacent independent store. Nothing in the notebook covers that
 * direction -- the recorded sched2 lever (batch 265, Func_80270d8) moves an
 * ARITHMETIC instruction by moving the arithmetic in the source, and there is
 * no arithmetic here to move.
 */
#include "gba/types.h"

struct Actor {
    u8 pad00[8];
    int x;              /* 0x08 */
    u8 pad0c[4];
    int z;              /* 0x10 */
    u8 pad14[0x10];
    int f24;            /* 0x24 */
    u8 pad28[4];
    int f2c;            /* 0x2c */
    u8 pad30[8];
    int f38;            /* 0x38 */
    u8 pad3c[4];
    int f40;            /* 0x40 */
    u8 pad44[0xc];
    u8 *part;           /* 0x50 */
};

extern unsigned char gState[];
extern struct Actor *GetFieldActor(int id);
extern u8 *_Sprite_AddLayer(u8 *part, int n);
extern void _Actor_SetAnim(struct Actor *a, int anim);
extern void WaitFrames(int n);

void Func_942e0(int anim)
{
    register struct Actor *a __asm__("r5");      /* 15 -> 2; either pin works */
    u8 *gs;
    u8 *p;
    u8 *layer;
    int z;

    gs = gState;
    gs += 0xfa << 1;
    a = GetFieldActor(*(int *)gs);
    p = a->part;
    layer = _Sprite_AddLayer(p, 0x1b);
    z = 0;
    p[0x26] = z;
    layer[5] = 0xf;
    a->x = (a->x & 0xfff00000) + (0x80 << 12);
    a->z = (a->z & 0xfff00000) + (0x80 << 13);
    a->f24 = z;
    a->f2c = z;
    a->f38 = 0x80 << 24;
    a->f40 = 0x80 << 24;
    _Actor_SetAnim(a, anim);
    WaitFrames(0x12);
}
