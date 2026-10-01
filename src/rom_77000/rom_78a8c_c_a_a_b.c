/* Func_8078ad0 (NotifyItemUsed) @ 0x08078ad0  --  LANDS, BYTE-IDENTICAL
 *
 * FIGURE: 0.  Measured, not inherited:
 *     OK Func_8078ad0 -- 40 bytes, 17 encodings and 2 relocations identical
 *     OK whole file   -- 40 bytes, 17 encodings and 2 relocations identical
 * Baseline for the park body in src/non_matching/rom_77000/rom_78ad0.c was
 * 3 of 17 (instruction counts equal, relocations ok).
 *
 * Verify with:
 *     docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/rom_77000/rom_78a8c_c_a_a_b.c \
 *       asm/rom_77000/rom_78a8c_c_a_a_b.s --whole
 *
 * SPLIT.  asm/rom_77000/rom_78a8c_c_a_a.s holds TWO functions.
 *   python3 tools/datacheck.py asm/rom_77000/rom_78a8c_c_a_a.s   -> CLEAN (no data)
 *   python3 tools/split_s.py asm/rom_77000/rom_78a8c_c_a_a.s Func_8078ad0 --dry-run
 *     would write asm/rom_77000/rom_78a8c_c_a_a_a.s  (1 function, 31 lines)  [Func_8078aa0]
 *     would write asm/rom_77000/rom_78a8c_c_a_a_b.s  (1 function, 20 lines)  [Func_8078ad0]
 *     would REMOVE asm/rom_77000/rom_78a8c_c_a_a.s ; would rewrite stage1.ld
 * INSTALL AT: src/rom_77000/rom_78a8c_c_a_a_b.c, deleting
 *             asm/rom_77000/rom_78a8c_c_a_a_b.s after the split.
 * `bl Func_8078aa0` survives the split: .thumb_func_start makes the symbol
 * .global, and objcmp --whole confirms both relocations (the .L7b490 pool word
 * and the bl) are byte-identical across the cut.
 *
 * PIN COUNT: 0.  No fakematch.txt row needed.
 *
 * THE MECHANISM -- A CROSSED PAIR, EACH HALF EXACTLY INERT ALONE.
 *
 *   A. Func_8078aa0 REALLY TAKES TWO PARAMETERS, and Func_8078ad0 forwards the
 *      second one.  The ROM never writes r1 before `bl Func_8078aa0`, so the
 *      caller's r1 passes straight through; src/non_matching/rom_77000/8078aa0.c
 *      -- in the same directory -- already had the callee at (int idx, int d).
 *      The park declared it with ONE parameter, which is also a semantic bug:
 *      the delta was being dropped.  ALONE: 3 (exactly inert).
 *
 *   B. `s32 r = 0;` INITIALISED AT ITS DECLARATION and declared FIRST, before
 *      `v`.  ALONE: 3 (exactly inert).
 *
 *   A + B: 0.
 *
 * WHY, and it REFUTES the park's diagnosis.  Both park files
 * (rom_78ad0.c, 8078ad0.c) say: "Both are scratch registers here ... this is a
 * pure allocation preference.  After the ldrb both r3 and r4 are free and gcc
 * reaches for r3 first.  For the ROM to pick r4 something else must have held
 * r3, and nothing in this function does."
 *
 * Something does.  With the delta forwarded, `d` is live from function entry to
 * the call; and with `r` born at entry rather than after the table lookup, r's
 * range covers the whole index computation, so it conflicts with the mask
 * pseudo (r3) and the table pointer (r2) as well as with the loaded value (r0).
 * r3 is NOT free when r is allocated.  Only r4 is left.  Neither half does it:
 * the forwarded delta does not touch r3, and an early-born r with a one-argument
 * callee still finds r3 dead by the time it needs it.
 *
 * The park's list of six byte-identical variants is ACCURATE -- I re-measured
 * seven of them and every one reads 3.  It is a clean instance of the
 * one-at-a-time failure: the missing half was never in the list because the
 * callee's arity was taken from this file rather than from its neighbour.
 *
 * Everything that park got right, stands: the `.L7b490` asm-label extern
 * reproduces exactly, and the plain literal `id & 0x1ff` already gives
 * `ldr r3, =0x1ff` with no narrow_constant lever needed.
 */
#include "gba/types.h"

extern u8 L7b490[] __asm__(".L7b490");
extern s32 Func_8078aa0(s32 n, s32 d);

/* Looks an id up in a 0x200-entry byte table and, when the entry is non-zero,
 * reports the consumed item through Func_8078aa0, passing the delta through.
 */
s32 Func_8078ad0(u32 id, s32 d)
{
    s32 r = 0;
    u32 v = L7b490[id & 0x1ff];

    if (v != 0)
        r = Func_8078aa0(v - 1, d);
    return r;
}
