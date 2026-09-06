/* OvlFunc_924_2009db4  --  0x02009db4
 * [asm/overlays/rom_7ac2d8/ovl_1db4_a.s, FIRST of two functions]
 *
 * PARK.  301 lines against 301, 291 encodings against 291, 21 differing --
 * and the two multisets are IDENTICAL.  The whole residue is one reload
 * register and the sched2 placement it drags with it, repeated once in the
 * mode 0 arm and once in the mode 1 arm.  Candidate: this file.
 *
 * The sibling in the same .s, OvlFunc_924_200a030, is EXACT
 * (src/overlays/rom_7ac2d8/ovl_1db4_a_b.c) and uses the same `struct P`, the
 * same five prototypes and the same argument shape, so a later merge into one
 * TU costs nothing structurally -- but it must be RE-SCREENED whole-object,
 * because one TU forces one struct and one set of declarations.
 *
 * WHAT IS SETTLED, and worth reusing:
 *
 * (1) THE TWO POSITIONS ARE EXPLICIT ACCUMULATORS, NOT AN EXPRESSION IN `i`.
 * The ROM opens each outer pass with `lsl r2, r3, #0x14` (j << 20) used TWICE
 * -- once subtracted from 0x32c0000 into r8, once added to 0x2c00000 into r7 --
 * and then walks r7 up and r8 down by 0x10000 per inner tick.  Writing the
 * positions as the arithmetic they are (`0x2c00000 + (j << 20) + (i << 16)`
 * and `0x32c0000 - (j << 20) - (i << 16)`) is 313 lines against 301 with 241
 * differing: gcc manufactures ONE giv for the ascending pair, strength-reduces
 * `-(j << 20)` into a stack-resident OUTER-loop giv (frame `sub sp, #0x44`
 * against the ROM's `#0x40`) and computes the descending position by hand
 * inside the mode >= 2 arm.  Naming `j << 20` alone still leaves 234.  Two
 * named accumulators updated in the loop tail is 301 lines and 56.
 *
 * This is docs/elevation.md's §"An add/sub chain on a constant may be gcc's OWN
 * arithmetic" test -- "write the literals first and see whether gcc produces
 * the chain" -- applied to an INDUCTION VARIABLE rather than a constant, and it
 * answers the other way: gcc does NOT produce the ROM's pair, so the pair was
 * in the source.  NEW, as an extension of that section: when a ROM carries TWO
 * counters over one loop and gcc will only manufacture one of them, the literal
 * form is the wrong one and both belong in the source.
 *
 * THE ORDER OF THE THREE TAIL STATEMENTS IS OBSERVABLE, and it is not the
 * ROM's emission order.  All six permutations were measured:
 *
 *     down -= 0x10000; up += 0x10000; i++;   21   <- exact tail
 *     down -= 0x10000; i++; up += 0x10000;   29
 *     up += 0x10000; down -= 0x10000; i++;   27
 *     up += 0x10000; i++; down -= 0x10000;   30
 *     i++; down -= 0x10000; up += 0x10000;  113   (302 lines)
 *     i++; up += 0x10000; down -= 0x10000;  112   (302 lines)
 *
 * The ROM EMITS `add r8, <-0x10000>` then `add r3, #0x1` then `add r7, r2`,
 * i.e. down, i, up -- which is 29, not 21.  Read the emission order as a hint
 * and then sweep; do not copy it.
 *
 * (2) THE `* 0x3333` SHIFT-ADD CHAIN MUST NOT HAVE A NAMED TARGET.  NEW.
 * gcc expands the multiply as `t*3`, `*17`, `*257` -- `lsl/add/lsl/add/lsl/add`
 * -- and WHERE THE RESULT GOES decides whether the last `add` is destructive:
 *
 *   `rx = (...) * 0x3333;`             last add is `add r5, r3, r2`, three
 *                                      operands, the chain having run in r3
 *   `rx = (...) * 0x3333 - 0xcccc;`    chain destructive in r3, but the
 *                                      constant add becomes `mov r1, r11/
 *                                      add r5, r3, r1` -- two instructions
 *   inline in the argument list        every add destructive, and the bias is
 *                                      `add r5, r11` -- the ROM exactly
 *
 * The mechanism is that a variable assignment gives `expand_mult` a target
 * pseudo, and the final add writes THAT, so the accumulator and the result are
 * two pseudos and the allocator separates them.  As a sub-expression of an
 * argument there is no target and the accumulator IS the result, which is what
 * lets a high register (`r11`, holding 0xffff3334) be the second operand of a
 * two-operand `add`.  Naming just the call-crossing product is 118 differing;
 * naming both products with the bias inline is 109.  The ROM runs the first
 * product in call-saved r5 across the second __Random call and applies its bias
 * AFTER the second product has been stored -- all of that falls out of the
 * inline form; none of it is reachable with a named local.
 *
 * (3) The prologue, the three-way tile paint, the outer-loop head, the whole
 * mode >= 2 arm, the entire trailing paint and both loop tails reproduce
 * unaided, including the ROM's spilled `i` at sp+0x14, the spilled `&p` at
 * sp+0x10 and the `mov r2, sp / add r2, #0x18` two-instruction address build.
 * sp = 0x40 is 0x10 outgoing arguments + 2 spill words + a 0x28 `struct P`.
 *
 * BLOCKER: A RELOAD REGISTER, r2 WHERE THE ROM HAS r3, IN TWO ARMS.
 *
 *     rom  ... str r3, [sp, #0x8] / ldr r3, [sp, #0x10] / add r5, r11 /
 *          mov r0, #0xc6 / str r3, [sp, #0xc]
 *     ours ... ldr r2, [sp, #0x10] / str r3, [sp, #0x4] / mov r3, #0x90 /
 *          lsl r3, #0xc / add r5, r11 / mov r0, #0xc6 / str r3, [sp, #0x8] /
 *          str r2, [sp, #0xc]
 *
 * First differing encoding is index 113, ref 9301 (`str r3, [sp, #4]`) against
 * ours 9a04 (`ldr r2, [sp, #0x10]`).  The mode >= 2 arm, which has a third
 * reload (its `mov r2, #0` for the fourth stack argument), matches us exactly.
 *
 * The mechanism, read from `.18.greg`: the store of the seventh argument needs
 * a scratch for the constant 0x90000 and takes r3; `allocate_reload_reg` then
 * resumes its round robin at `last_spill_reg + 1`, so the eighth argument's
 * reload of the spilled `&p` takes r2.  For the ROM to reuse r3 the 0x90000
 * store cannot have been a reload -- it must have been a pseudo with a hard
 * register -- and no source spelling reaches that: a named `int d = 0x90 << 12`
 * is const-propagated straight back to a literal.  With r5..r11 all committed
 * (rx, mode, up, down, 1, j, 0xffff3334) there is no register to give it.
 *
 * MEASURED INERT (all 21, i.e. byte-for-byte the same object):
 *   struct P *pp = &p and passing pp;  the same with the field stores written
 *   through pp;  pp declared before p;  a named `int d = 0x90 << 12` at
 *   function scope, per arm, or with pp;  `0x90000` for `0x90 << 12`;
 *   `up + 0x600000` for `up + 0xc0 * 0x8000`;  0x8000 for `0x80 << 8`;
 *   `p` declared after up/down;  i and j declaration order (three orders);
 *   `int p[10]` with `p[1]/p[2]/p[3]` instead of the struct;  passing `mode`
 *   instead of a literal 0 for the fourth argument in the mode 0 arm;  a named
 *   `zero` for the fourth argument in the mode 1 and mode >= 2 arms;  a named
 *   `zero` for the second argument everywhere;  no prototype on
 *   OvlFunc_common0_10c;  an `int` return type on it;  `void *` for its last
 *   parameter;  no prototype on __Random;  an `int` return on __CutsceneWait.
 *
 * MEASURED WORSE:
 *   named locals for both products with the bias inline            109
 *   naming only the call-crossing product                          118
 *   a named local for the second product only                      184
 *   a named local for the first product only                       200 (304 ln)
 *   `switch (mode)` instead of the if / else-if chain              201 (303 ln)
 *   naming the position argument per site (zz = up, zz = up + K)   239 (307 ln)
 *   `do { } while (0)` before each call                             76
 *   __CutsceneWait moved inside the three arms                     176 (302 ln)
 *   `struct P *volatile pp`                                        237 (305 ln)
 *   d and q named together per site, either order                  212 (305 ln)
 *   `j = 0` hoisted above the field stores                         236 (302 ln)
 *   `j = 0` hoisted above __PlaySound                              122
 *   a named `zero` for the fourth argument in ALL THREE arms        64
 *
 * NO FLAG GROUP HELPS, and that is worth recording because the shape (a
 * scheduler-visible hoist) invites one.  Against this candidate:
 *   -fno-strength-reduce 21, -fno-rerun-cse-after-loop 21,
 *   -fno-strict-aliasing 21, -fno-gcse 230 (299 lines),
 *   -fno-schedule-insns2 221 (300 lines), -ffixed-r7 239 (307 lines).
 * The two inert ones are inert because the accumulators and the goto-free inner
 * loop have already removed what they would have disabled.
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x28 - 0x10];
};

extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);

void OvlFunc_924_2009db4(int mode)
{
    struct P p;
    unsigned int j, i;
    int up, down;

    __PlaySound(0xd3);
    if (mode == 0) {
        __CopyMapTiles(0x6f, 0x39, 0x71, 0x2a, 1, 1);
        __CopyMapTiles(0x6f, 0x3b, 0x71, 0x2b, 1, 1);
    } else if (mode == 1) {
        __CopyMapTiles(0x71, 0x3a, 0x70, 0x2e, 1, 1);
        __CopyMapTiles(0x73, 0x3a, 0x71, 0x2e, 1, 1);
    } else {
        __CopyMapTiles(0x73, 0x39, 0x74, 0x2c, 1, 1);
        __CopyMapTiles(0x71, 0x39, 0x73, 0x2c, 1, 1);
    }
    p.f4 = 7;
    p.f8 = 0x80 << 8;
    p.fc = 0x80 << 8;
    j = 0;
    do {
        i = 0;
        down = 0xcb * 0x40000 - (j << 20);
        up = (j << 20) + 0xb0 * 0x40000;
        do {
            if (i & 1) {
                if (mode == 0) {
                    OvlFunc_common0_10c(0xc6 * 0x40000, 0, up,
                                        ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0,
                                        ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0x90 << 12, &p);
                } else if (mode == 1) {
                    OvlFunc_common0_10c(up + 0xc0 * 0x8000, 0, 0x2ea0000,
                                        ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0,
                                        ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0x90 << 12, &p);
                } else {
                    OvlFunc_common0_10c(down, 0, 0x2ca0000,
                                        ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0,
                                        ((__Random() << 3) >> 16) * 0x3333 - 0xcccc, 0x90 << 12, &p);
                }
                __CutsceneWait(1);
            }
            down -= 0x10000;
            up += 0x10000;
            i++;
        } while (i <= 7);
        if (mode == 0) {
            __CopyMapTiles(0x6f, 0x3a, 0x71, j + 0x2b, 1, 1);
            __CopyMapTiles(0x6f, 0x3b, 0x71, j + 0x2c, 1, 1);
        } else if (mode == 1) {
            __CopyMapTiles(0x72, 0x3a, j + 0x71, 0x2e, 1, 1);
            __CopyMapTiles(0x73, 0x3a, j + 0x72, 0x2e, 1, 1);
        } else {
            __CopyMapTiles(0x72, 0x39, 0x73 - j, 0x2c, 1, 1);
            __CopyMapTiles(0x71, 0x39, 0x72 - j, 0x2c, 1, 1);
        }
        j++;
    } while (j <= 1);
}
