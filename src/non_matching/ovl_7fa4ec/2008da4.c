/* OvlFunc_970_2008da4 (0x02008da4) -- NON-MATCHING.
 * Blocker class: a narrowed mask, an allocation role, and one scheduled load.
 *
 * 167 lines against 167, SEVEN differing -- down from 158 through four levers,
 * three of which are worth more than the park.
 *
 * LEVER 1: A WRITE-ONLY LOCAL THE ROM STORES TO THE STACK IS `volatile`.
 * The ROM computes each of three BG control words, stores it to a stack
 * halfword AND to the register:
 *
 *     mov r5, sp / add r5, #2 / ... / strh r3, [r5] / strh r3, [r0]
 *
 * Nothing ever reads that stack slot. Declared plainly gcc keeps the value in a
 * register and drops the frame entirely -- 165 lines against 167 with 158
 * differing. `volatile unsigned short t;` restores it: 166 lines, 35 differing.
 * (The `mov r5, sp / add r5, #2` is not a lever, it is Thumb: `strh` has no
 * sp-relative form, so a halfword stack store must compute its address.)
 *
 * LEVER 2: NAMING A STORED VALUE STOPS THE POOLING -- twice here, and it is
 * worth 17 lines. Batch 176 recorded this for a halfword store of a small
 * literal; both of this function's hardware-register writes are the same shape:
 *
 *     `REG_BLDALPHA = 0x81 << 4;`            gcc pools 0x810 -- and the pool it
 *                                            creates needs a skip jump
 *     `n = 0x81 << 4; REG_BLDALPHA = n;`     `mov r2, #0x81 / lsl r2, #4`, and
 *                                            the skip jump disappears with the
 *                                            pool entry.  35 -> 26
 *     `n = 0x2648; REG_BLDCNT = n;`          15 -> 7, by the same mechanism
 *
 * LEVER 3: THE OFFSET BELONGS IN THE LOAD. `*(int *)(b + (0x9a << 1) + 0xc)`
 * folds to a single 0x140 offset and gcc addresses it as `[r2, #0]`; the ROM
 * has `[r1, #0xc]` off a base built from 0x134. Naming the base
 * (`r = b + (0x9a << 1); *(int *)(r + 0xc) -= ...`) keeps the +0xc in the load.
 * 26 -> 15.
 *
 * WHAT IS LEFT, and none of it is source-reachable:
 *
 *   (a) ONE LINE, the mask width. The ROM builds `~0xc` as -13 by DERIVING it
 *       from the 2 already live (`mov r5, #2` for the earlier stores, then
 *       `sub r5, #0xf`). gcc emits `mov r5, #0xf3` -- the same mask narrowed to
 *       a byte, which is legal because the result is stored with `strb` and is
 *       one instruction either way. gcc is simply doing a narrowing the
 *       original build did not. `& -13` instead of `& ~0xc` is byte-identical.
 *
 *   (b) THREE LINES, the last of ten `->f50` reloads: the ROM puts the pointer
 *       in r2 and we put it in r1. Same "last use kills the register"
 *       asymmetry as ovl_7b8cb0/2008904.c records for its `orr` masks.
 *
 *   (c) THREE LINES, `ldr r3, =REG_BLDCNT` hoisted one store early. Splitting
 *       the two BLD values into separate locals is inert; a named
 *       `vu16 *bld = &REG_BLDCNT;` with `bld[0]` / `bld[1]` is WORSE (166
 *       lines, 35 differing) because it makes gcc derive BLDALPHA where the
 *       ROM loads BLDCNT fresh.
 *
 * ALSO RIGHT: `iwram_3001ebc` reached as `iwram_3001e70[0x13]` -- the two are
 * 0x4c apart and the ROM reads both off one pool entry, the adjacent-globals
 * rule again; the `(x & ~0xc) | 4` field edit repeated ten times with gcc
 * sharing both constants by itself; and the actor pointers advanced in place.
 *
 * NEXT: nothing in eight probes.
 */
#include "gba/types.h"
#include "gba/io.h"

extern unsigned char gState[];
extern unsigned char *iwram_3001e70[];
extern void __GiveItemTo(int slot, int item);
extern char *__MapActor_GetActor(int slot);
extern void __Func_800fe9c(void);
extern void OvlFunc_970_200807c(void);

int OvlFunc_970_2008da4(void)
{
    unsigned char *g;
    unsigned char *b;
    unsigned char *q;
    char *p;
    char *s;
    volatile unsigned short t;
    int n;
    unsigned char *r;

    g = gState;
    b = iwram_3001e70[0];
    if (*(short *)(g + (0xe1 << 1)) == 0x63)
        __GiveItemTo(0, 0xf2);
    q = iwram_3001e70[0x13];
    *(int *)(q + (0xe0 << 1)) = 0x100;
    p = __MapActor_GetActor(8) + 0x59;
    *p = 0;
    p = __MapActor_GetActor(8) + 0x23;
    *p = 2;
    p = __MapActor_GetActor(9) + 0x59;
    *p = 0;
    p = __MapActor_GetActor(9) + 0x23;
    *p = 2;
    s = *(char **)(__MapActor_GetActor(8) + 0x50);
    s[9] = (s[9] & ~0xc) | 4;
    s = *(char **)(__MapActor_GetActor(9) + 0x50);
    s[9] = (s[9] & ~0xc) | 4;
    p = __MapActor_GetActor(0);
    s = *(char **)(p + 0x50);
    s[9] = (s[9] & ~0xc) | 4;
    s = *(char **)(p + 0x50);
    s[0x15] = (s[0x15] & ~0xc) | 4;
    p = __MapActor_GetActor(1);
    s = *(char **)(p + 0x50);
    s[9] = (s[9] & ~0xc) | 4;
    s = *(char **)(p + 0x50);
    s[0x15] = (s[0x15] & ~0xc) | 4;
    p = __MapActor_GetActor(2);
    s = *(char **)(p + 0x50);
    s[9] = (s[9] & ~0xc) | 4;
    s = *(char **)(p + 0x50);
    s[0x15] = (s[0x15] & ~0xc) | 4;
    p = __MapActor_GetActor(3);
    s = *(char **)(p + 0x50);
    s[9] = (s[9] & ~0xc) | 4;
    s = *(char **)(p + 0x50);
    s[0x15] = (s[0x15] & ~0xc) | 4;
    t = (REG_BG3CNT & 0xfffc) | 2;
    REG_BG3CNT = t;
    t = (REG_BG2CNT & 0xfffc) | 3;
    REG_BG2CNT = t;
    t = (REG_BG1CNT & 0xfffc) | 3;
    REG_BG1CNT = t;
    n = 0x2648;
    REG_BLDCNT = n;
    n = 0x81 << 4;
    REG_BLDALPHA = n;
    r = b + (0x9a << 1);
    *(int *)(r + 0xc) -= 0x5a0000;
    r = b + (0xb2 << 1);
    *(int *)(r + 0xc) -= 0x5a0000;
    __Func_800fe9c();
    OvlFunc_970_200807c();
    return 0;
}
