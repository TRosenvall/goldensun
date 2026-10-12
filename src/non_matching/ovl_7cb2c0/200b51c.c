/* OvlFunc_945_200b51c -- NOT MATCHING, a LENGTH gap: ref 103 instructions against ours 104 (+1).
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/Gb51c.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * THE LENGTH-GAP CRITERION APPLIES, and it is this tree's most productive
 * targeting instrument -- four batches run on it closed 8 of 15, 7 of 25,
 * 8 of 25 and 10 of 32, and LOCATED the missing-or-extra instruction on
 * essentially every target.  Our side is 1 instruction(s) LONGER, so the
 * search is what lets gcc REUSE a value -- see docs/elevation.md, 'OUR STREAM BEING
 * SHORTER IS A SIGNATURE'.
 * No encoding figure exists until the lengths agree.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7cb2c0/200b51c.c \
 *     asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a_c_c_c.s --func OvlFunc_945_200b51c
 *
 * The original prose park is preserved below.
 */
extern unsigned char gScript_945__0200e958[];
extern unsigned char gScript_945__0200e840[];

extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void OvlFunc_945_200d004(void);

void OvlFunc_945_200b51c(void)
{
    int p, q, r, s;

    if (__GetFlag(0x93e)) {
        __MapActor_SetPos(8, 0, 0);
        __MapActor_SetPos(9, 0, 0);
        __MapActor_SetPos(0xa, 0, 0);
        __MapActor_SetPos(0xb, 0, 0);
        __MapActor_SetPos(0xc, 0, 0);
        OvlFunc_945_200c8e8(0xe, 0, 0);
    } else if (__GetFlag(0x8a << 4)) {
        OvlFunc_945_200c890(8, 0x98, 0xde << 1, 0xc0 << 6);
        __MapActor_SetBehavior(8, gScript_945__0200e958);
        p = 0xf0 << 1;
        r = 0xb0 << 8;
        q = 0xf4 << 1;
        s = 0xd0 << 8;
        OvlFunc_945_200c890(0xa, 0xb8, p, r);
        OvlFunc_945_200c890(0xc, 0xaa, q, r);
        OvlFunc_945_200c890(0xd, 0x88, q, s);
        OvlFunc_945_200c890(0xf, 0x78, p, s);
        OvlFunc_945_200c890(0xe, 0xb8, 0x20e, r);
        OvlFunc_945_200c890(0xb, 0x88, 0x92 << 2, 0x80 << 8);
        __MapActor_SetBehavior(0xb, gScript_945__0200e840);
    } else if (__GetFlag(0x928)) {
        OvlFunc_945_200d004();
    } else if (__GetFlag(0x925)) {
        OvlFunc_945_200c8e8(0x12, 0, 0);
    } else if (__GetFlag(0x911)) {
        if (__GetFlag(0x922)) {
            OvlFunc_945_200c8e8(0xe, 0, 0);
            __MapActor_SetPos(0xc, 0, 0);
        }
    }
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_945_200b51c -- 0x0200b51c, asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a.s
 *
 * 133 ROM lines against 134 of ours, 85 differing.  Candidate: scratch/Gb51c.c.
 * The five-arm flag chain, both SetBehavior calls and every argument value are
 * right; the whole diff is in one block.
 *
 * BLOCKER: FOUR values held across calls, allocated differently.  The 0x8a0 arm
 * calls OvlFunc_945_200c890 six times with four repeated constants -- 0x1e0,
 * 0x1e8, 0xb000, 0xd000 -- and the ROM keeps them in r10, r5, r8 and r6:
 *
 *      rom   mov r3,#0xb0 / lsl r3,#8 / mov r5,#0xf4 / mov r8,r3 / ... / lsl r5,#1
 *      ours  mov r5,#0xb0 / mov r3,#0xd0 / lsl r5,#8 / lsl r3,#8 / mov r6,#0xf4
 *
 * The ROM also interleaves the partial builds into the argument setup of the
 * call between them, and passes the first call's fourth argument as the LIVE r3
 * rather than copying from r8.
 *
 * ALL SIX ASSIGNMENT ORDERS SCREENED.  The source-order lever picks registers
 * for two independent values; with FOUR it has no purchase.  Every permutation
 * of the three non-first assignments gives 85 or 86 -- the two that give 86 move
 * the first difference one line earlier, so the ordering is doing something, but
 * nothing that converges.
 *
 * That is worth recording as a boundary on the lever rather than another
 * failure: it reaches two values, it does not scale to four.
 *
 * ALSO NOTE the one-line length difference.  The ROM CROSS-JUMPS: the 0x93e arm
 * ends `mov r0, #0xe / b .L360c` and the 0x925 arm falls into the same
 * `.L360c: mov r1,#0 / mov r2,#0 / bl OvlFunc_945_200c8e8`, sharing three
 * instructions between two paths.  Writing the two calls out separately lets gcc
 * share some but not all of it.  Not attacked, because the register allocation
 * above dominates and would have to move first.
 *)

*/
#endif
