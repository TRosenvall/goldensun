/* Func_800c5b4 @ 0x0800c5b4
 *
 * Source asm: goldensun/asm/rom_9000/rom_c004_c_a_c_c_a.s
 *
 * NOT SPLIT, and it would not need one -- the .s holds only this function.
 *
 * Registers two callbacks, starts something, waits a frame, then rewrites the
 * background-mode bits of REG_DISPCNT. Twenty-three instructions against
 * twenty-three, TWENTY-ONE IDENTICAL.
 *
 * Blocker: narrow_constant. Both pool loads come out as halfwords:
 *
 *     rom    ldr r3, =0xf1ff / ... / ldr r2, =0x1000
 *     ours   ldrh r3, L0     / ... / ldrh r2, L0+4
 *
 * A REFINEMENT ON THE narrow_constant LEVER, and it is the reason to keep this
 * note. src/non_matching/rom_15000/rom_1c154.c established that a mask written
 * as a named `u32` local comes out as a 32-bit `ldr` where a literal narrows to
 * a halfword, and that ONE variable reassigned also reproduces the ROM's reuse
 * of a single register. Neither works here:
 *
 *   1. two named u32 masks           -- 24 lines, 6 differ (WORSE)
 *   2. one reassigned u32 mask       -- 24 lines, 10 differ (worse still)
 *   3. `~0x0e00` instead of `0xf1ff`, so the constant is 32-bit by
 *      construction                  -- 23 lines, 2 differ, no change at all
 *   4. the plain literal (the form below) -- 23 lines, 2 differ, the best
 *
 * WHY IT DIFFERS FROM rom_1c154.c: there the narrow value was a `u16` STRUCT
 * FIELD read into a local, so giving the mask a wider type gave gcc something
 * to widen to. Here the value comes from a `vu16 *` hardware register and goes
 * straight back to one, so gcc knows the whole expression is sixteen bits wide
 * from the POINTER TYPE, and no amount of retyping the mask changes that. (3)
 * is the clearest evidence: a constant that is 32-bit by construction still
 * gets narrowed, because gcc narrows the operation and not the operand.
 *
 * So narrow_constant has two sub-cases and only one of them has a lever. The
 * one that does is "a narrow value in a wide expression"; the one that does not
 * is "a narrow expression end to end".
 *
 * RE-ATTEMPTED, batch 40, from tools/rank_parks.py. Still 2 of 23 and the two
 * are the whole diff:
 *
 *     rom    ldr r3, =0xf1ff   ...   ldr r2, .Lc5ec   @ 0x1000
 *     ours   ldrh r3, .L0      ...   ldrh r2, .L0+4
 *
 * Both constants fit in sixteen bits and the destination is a `vu16`, so gcc
 * narrows the whole expression to HImode and pools them as HALFWORDS. The ROM
 * pools them as WORDS, which is what an un-narrowed expression gives.
 *
 * This looks like the inverted narrow_constant class -- gcc narrowing what the
 * ROM keeps wide -- and the lever for that class does NOT work here:
 *
 *   naming the RESULT, `u32 t = (v & 0xf1ff) | 0x1000; *p = t;`     2 (no change)
 *   the same with the mask first, or split into two statements      2 (no change)
 *   the same with `int t` rather than `u32 t`                       2 (no change)
 *   naming the OPERANDS, `m = 0xf1ff; o = 0x1000;`                  7 (worse)
 *
 * The lever works when a narrow STORE pulls a `mov` down into a pool load. Here
 * the narrowing is of the whole computation, and an intervening local does not
 * stop it because gcc re-derives the mode from the store either way. What would
 * is something that makes the constants SImode at the tree level -- a symbol
 * would do it, since gcc always word-loads a symbol address, but 0xf1ff and
 * 0x1000 are a mask and a bit and there is no reason to think either was one.
 */
#include "gba/types.h"

extern void Func_80042c8(void *fn);
extern void Func_800c62c(void);
extern void Func_800c880(void);
extern void _Func_8091200(int a, int b);
extern void _Func_8091254(int a);
extern void WaitFrames(int n);

void Func_800c5b4(void)
{
    vu16 *p;
    u32 v;

    Func_80042c8(Func_800c62c);
    Func_80042c8(Func_800c880);
    _Func_8091200(0x80 << 9, 1);
    _Func_8091254(1);
    WaitFrames(1);
    p = (vu16 *)(0x80 << 19);
    v = *p;
    *p = (v & 0xf1ff) | 0x1000;
}
