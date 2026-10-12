/* OvlFunc_881_20097a4 -- NOT MATCHING, a LENGTH gap: ref 32 instructions against ours 30 (-2).
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/L97a4.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * THE LENGTH-GAP CRITERION APPLIES, and it is this tree's most productive
 * targeting instrument -- four batches run on it closed 8 of 15, 7 of 25,
 * 8 of 25 and 10 of 32, and LOCATED the missing-or-extra instruction on
 * essentially every target.  Our side is 2 instruction(s) SHORTER, so the
 * search is which statement is missing -- see docs/elevation.md, 'OUR STREAM BEING
 * SHORTER IS A SIGNATURE'.
 * No encoding figure exists until the lengths agree.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/20097a4.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c_a.s --func OvlFunc_881_20097a4
 *
 * The original prose park is preserved below.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __Actor_SetAnim(unsigned char *a, int n);

void OvlFunc_881_20097a4(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *p;
    int lim;
    int v;

    a = __MapActor_GetActor(0xf);
    b = __MapActor_GetActor(0xe);
    *(int *)(a + 8) = *(int *)(b + 8);
    lim = 0xa0 << 12;
    *(int *)(a + 0x10) = *(int *)(b + 0x10);
    if (*(int *)(a + 0xc) < lim) {
        v = 0xa0 << 12;
        *(int *)(a + 0xc) = v;
        if (__GetFlag(0x80 << 2) == 0) {
            __PlaySound(0x91);
            __Actor_SetAnim(a, 3);
            __SetFlag(0x80 << 2);
            p = a + 0x64;
            *(short *)p = 1;
        }
    }
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_881_20097a4 -- 0x020097a4,
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a.s
 *
 * 37 vs 39 lines, 24 differing (with --no-rerun-cse; 25 without).
 * Candidate at scratch/L97a4.c.
 *
 * BLOCKER: ours pushes r6 the ROM does not.  The value 0xa0 << 12 is built
 * TWICE in the ROM -- once for the `cmp` before the branch and once for the
 * store inside it -- and gcc commons the two into a callee-saved register held
 * across the intervening __GetFlag call.
 *
 * This is the commoned-constant tell, but NEITHER remedy reaches it:
 *   - `-fno-rerun-cse-after-loop` takes 25 to 24 and leaves the push.
 *   - Two separately named locals (which is already how it is written) do not
 *     stop the commoning.
 *   - Assigning BOTH in the dominating block above the branch is worse (28).
 *
 * That is a third case for the tell and the first where both known remedies
 * fail.  What distinguishes it from the five that CSE_CFLAGS fixes: there the
 * repeated value is a flag ID consumed by two CALLS, here it is a comparison
 * operand and a stored value, so the commoning survives the pass that
 * -fno-rerun-cse-after-loop disables.
 *)

*/
#endif
