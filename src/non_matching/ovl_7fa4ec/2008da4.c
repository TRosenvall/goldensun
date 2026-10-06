/* OvlFunc_970_2008da4 (0x02008da4) -- NON-MATCHING.
 *
 * NON-MATCHING, 6 differing encodings of 175  (MEASURED, batch 331 brief E).
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7fa4ec/2008da4.c asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_a_c.s --func OvlFunc_970_2008da4
 *
 * ===== BATCH 331 BRIEF E: CAME DOWN FROM NINE ON THE ONE DIMENSION NOBODY
 * ===== HAD VARIED -- HOW MANY NAMES THE *LAST* ACTOR BLOCK SHARES
 *
 * The body changed, so everything below that was measured against the old
 * body carries its old figure of nine, spelled in words so no tool reads it as
 * a claim.  The 3-insn `->f50` run the header below calls "not closed" IS NOW
 * CLOSED.  All four actor blocks read the same `*(char **)(p + 0x50)` twice,
 * once for `[9]` and once for `[0x15]`, and the whole standing negative list
 * had varied the mask, the OR constant and the bitfield spelling -- never the
 * naming.  Giving the SECOND read of the LAST block its own local closes it:
 *
 *     all four blocks reusing `s` for both reads (the old body)   nine of 175
 *     a second name in the LAST block only                           6 of 175
 *     a second name in the last TWO blocks                          twelve
 *     a second name in ALL FOUR blocks                              eighteen
 *     a second POINTER name in the last block only                  nine
 *     a second pointer AND a second result name, last block          6 of 175
 *     one single `->f50` read reused for both offsets, last block    forty
 *
 * So it is strictly the last block, and extending the split to the others
 * costs three encodings each.  This is batch 330's "do not search the diagonal
 * of a square" rule on a FOUR-fold repeat: the right edit is asymmetric across
 * textually identical regions, and any sweep that varies them together misses
 * it.  The second pointer name is a tie on top of the second result name, so
 * only the result name ships.
 *
 * WHAT THE REMAINING SIX ARE.  One encoding is the mask build, and the
 * reference's spelling is now readable: at index 42 ref `3d0f` is
 * `sub r5, #0xf` against our `25f3`, `mov r5, #0xf3`.  The reference does NOT
 * materialise the mask -- it takes the `mov r5, #2` already standing from the
 * two `*p = 2;` byte stores and subtracts 0xf to reach -13, then feeds that one
 * register to all ten `mov r3, r5` copies.  So the reference's program relates
 * the STORED VALUE and the MASK, and the untried dimension here is a spelling
 * in which 2 and -13 come from one named quantity rather than two constants.
 * The other five are the pool-load swap the header below describes.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7fa4ec/2008da4.c \
 *     asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_a_c.s --func OvlFunc_970_2008da4
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
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
 

 *
 * ===== BATCH 329 BRIEF I: THE FIGURE IS 9, NOT SEVEN =====
 *
 * Re-measured unfiltered (OLD BODY): nine differing of a hundred and seventy-five, ref and ours equal, first
 * at index 42, no SIZE and no POOL WORD line, so the streams are aligned. The
 * header's "SEVEN differing" is tryc's instruction-stream count; the two extra
 * encodings are the pc-relative offsets of the two pool loads that the (c)
 * scheduling swap moves, which tryc normalises away and objcmp does not.
 * objcmp is the authority, so the figure the old body carried forward was nine.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7fa4ec/2008da4.c asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_a_c.s --func OvlFunc_970_2008da4
 *
 * The three runs reproduce as the header describes them: 1 insn for the mask
 * build (`sub r5, #0xf` against `mov r5, #0xf3`), 3 for the last `->f50`
 * reload (rom r2, ours r1), 3 for the pool-load swap.
 *
 * TWO CROSS-PARK PORTS MEASURED HERE AND BOTH REFUTED, which matters because
 * this park and ovl_7d0e88/200a1ac.c carry the SAME `(x & -13) | K` byte-field
 * edit and sit at the same figure:
 *
 *   a named `int mask = -13;` assigned at the top of the function, which is the
 *   lever 200a1ac's header calls "a NEW LEVER worth reusing" (it is what stops
 *   the narrowing there and is worth 33 differing on that function)
 *                       MUCH WORSE HERE: 177 differing of 175, 159 instructions
 *                       against 152, 412 bytes against 396. It does not port.
 *                       That agrees with this park's own note that `& -13`
 *                       instead of `& ~0xc` is byte-identical, and settles the
 *                       obvious "then try naming it" follow-up.
 *
 *   the ten field edits written as 2-bit BITFIELD stores (`s->b2 = 1` and
 *   `s->c2 = 1` on a `struct Sprite` with `b0:2, b2:2, b4:4`), which is the
 *   construct that produces the ROM's hoisted constant build in the landed
 *   `src/overlays/rom_794ac0/ovl_30_a_c_c_c_a_c_c_c_a.c` -- and that file's
 *   edits, like these, are separated by `__MapActor_GetActor` calls, so it is
 *   the closest solved analogue in the tree
 *                       WORSE: 128 differing of 175, and 149 instructions
 *                       against 152, so it emits LESS WORK than the ROM. The
 *                       park's "the field edit repeated ten times with gcc
 *                       sharing both constants by itself" is RIGHT and should
 *                       be kept; the bitfield spelling makes gcc share harder.
 *
 * The 3-insn pointer run is the same class as the other three parks in this
 * batch: `local-alloc.c:combine_regs` (:1593) ties a destination to a
 * block-local input that dies exactly once, so the reloaded `->f50` pointer
 * lands on the register the previous `strb` just freed (r1) where the ROM has
 * r2. Its only source-visible escape is the guard
 * `ureg >= FIRST_PSEUDO_REGISTER && reg_qty[ureg] < 0` -- "not local to this
 * block OR DIES MORE THAN ONCE" -- and this program gives each pointer one
 * death. Not closed; the header's "nothing in eight probes" stands, now with
 * the pass named.
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
    char *u;
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
    u = *(char **)(p + 0x50);
    u[0x15] = (u[0x15] & ~0xc) | 4;
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
