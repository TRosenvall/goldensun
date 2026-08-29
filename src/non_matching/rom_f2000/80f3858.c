/* Func_80f3858 (BeginPaletteFade) -- NON-MATCHING.
 * Blocker class: CONSTANT DERIVATION, and it is the MIRROR IMAGE of
 * src/non_matching/ovl_7ac2d8/200adcc.c.
 *
 * 28 lines against the ROM's 28, 7 differing, all in the two byte stores.
 *
 *     rom   ldr r1, =0x3001 / add r3, r4, r1 / add r1, #1  ... add r3, r4, r1
 *     ours  ldr r2, =0x3001 / add r3, r4, r2 ... ldr r3, =0x3002 / add r2, r4, r3
 *
 * The ROM keeps 0x3001 in a register and DERIVES 0x3002 by adding one.  gcc
 * takes a second pool entry instead.
 *
 * Worth pairing with 200adcc, where the same class runs the other way: there
 * gcc derived 0x50000c4 as 0x50000ce - 0xa and the ROM took the second pool
 * entry.  So gcc-2.96 is not uniformly more or less willing to derive one
 * near constant from another than the ROM's compiler was; the two disagree in
 * both directions, on the same kind of address pair, a few hundred bytes
 * apart in the same game.  That rules out a single flag as the explanation
 * for either, and it is why neither park proposes one.
 *
 * Tried:
 *   - `p[0x3001]` and `p[0x3002]` as plain subscripts:  7 differing
 *   - a named `off` variable incremented between the stores, which is the
 *     "name the OFFSET, not the base" lever and is exactly the ROM's shape:
 *     20 differing and a stream two lines SHORT.  gcc constant-folds `off++`
 *     before it ever reaches register allocation, so the lever cannot express
 *     a runtime increment of a compile-time offset.  This is the useful
 *     negative: that lever moves which VALUE is named, never whether the
 *     addition survives to runtime.
 *   - --no-rerun-cse (7, unchanged), --O1 (18), --no-sched2 (18).
 *
 * The rest of the function -- the three scaled base additions feeding
 * Func_80f2ebc -- is byte-exact, including the interleaved lsl/add pairs.
 */
extern char *iwram_3001ed0;
extern void Func_80f2ebc(void *a, void *b, void *c, int n);

void Func_80f3858(int frames)
{
    char *p;

    p = iwram_3001ed0;
    if (p != 0) {
        p[0x3001] = frames;
        p[0x3002] = 0;
        Func_80f2ebc(p + 0x400, p + 0x1000, p + 0x1c00, frames);
    }
}
