/* Cluster OvlFunc_898_2008ef4..OvlFunc_898_2008ef4 extracted from
 * goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_a_c.s.
 *
 * Total .text for this TU = 72 bytes (= 0x48). Parked twice (at 2 of 30 and 4 of 30);
 * elevated in batch 278. TWO ARGUMENT PINS -- one fakematch row.
 *
 * IDENTICAL TWIN: OvlFunc_901_2008a80, at
 * src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_c_c.c -- 32 normalised lines,
 * identical, and verified by objcmp against its OWN reference. If you edit one, edit both.
 *
 * ===== THIS RETIRES THE "ARGUMENT PRECOMPUTE" BLOCKER CLASS. =====
 *
 * Both parks named this residue a COMPILER DIFFERENCE -- something about how the original
 * toolchain precomputed call arguments. IT IS NOT. It is a post-reload scheduling tie, and
 * `.23.sched2` says so exactly: at t = 6 the ready list is `20 67`, where insn 20 is
 * `mov r0, #0` and insn 67 is `lsl r2, #7`, BOTH PRIORITY 72, and the tie falls to
 * INSN_LUID. The chain `expand_call` emits is
 * `[mov r1 / lsl r1][mov r2 / lsl r2][mov r0]` -- both constant arguments precomputed and r0
 * filled last -- so `lsl r2` wins the tie and lands before `mov r0`. The ROM needs
 * `[mov r1 / lsl r1][mov r0][mov r2 / lsl r2]`, i.e. the r0 fill moved EARLIER IN THE CHAIN.
 *
 * HOW IT WAS FOUND, and the method is the transferable part: sweep the SOLVED corpus for the
 * three-line window `lsl / mov r0,#0 / lsl`. That returned 23 hits, every one with a `.c`
 * beside it, and src/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_a_c.c produces the shape from an
 * argument pin pair on r0 and r1 with argument 3 left BARE, the pinned assignments written as
 * separate statements before the call. Corpus scanning for a SHAPE beats sweeping spellings
 * for it -- second time that has paid (see src/non_matching/rom_a1000/80aad10.c).
 *
 * THE PIN SET IS THE MEASURED MINIMUM, and r0 alone was screened first as the recorded rule
 * requires:
 *
 *     no pins (the park's C)                    2 differing
 *     r0 only                                   2
 *     r1 only                                   2
 *     r0 + r2                                   2
 *     r2 + r0                                   2
 *     r0 + r1, r0 assigned first                EXACT
 *     r0 + r1, r1 assigned first                EXACT
 *
 * So two pins, and the order between them is inert. The r0-first form is kept to match the
 * rom_7c6bac precedent. Note argument 3 must stay BARE -- pinning it is not the lever.
 */
extern unsigned char iwram_3001ebc[];
extern void __MapActor_SetSpeed(unsigned int, int, int);
extern void __Func_809218c(int, int, int);
extern void __Func_8091e9c(int);

void OvlFunc_898_2008ef4(int a, int b, int c)
{
    char *base;

    {
        register int p0 __asm__("r0") = 0;
        register int p1 __asm__("r1") = 0x80 << 8;
        __MapActor_SetSpeed(p0, p1, 0x80 << 7);
    }
    __Func_809218c(0, a, b);
    base = *(char **)iwram_3001ebc;
    *(int *)(base + (0xe4 << 1)) = 0x10;
    __Func_8091e9c(c);
}
