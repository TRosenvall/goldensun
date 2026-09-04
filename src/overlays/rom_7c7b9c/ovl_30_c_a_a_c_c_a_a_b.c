/* OvlFunc_943_200b558  --  0x0200b558
 *
 * Cut out of goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_c_a_a.s.
 *
 * Classifies a map actor's stored heading, taken from the halfword table
 * .L5b40, into one of four bands; each band advances the actor's angle in
 * .L5b30 by a different step and selects a walk animation. The two bands that
 * pick animation 3 straddle the 0x7000 wrap, which is why one of them is
 * expressed as an addition rather than a subtraction.
 *
 * FOUR RANGE TESTS, WRITTEN AS ORDINARY COMPARISONS. The ROM's shape --
 *
 *     add r3, r2, r4     @ r4 = 0xffff97ff, i.e. v - 0x6801
 *     lsl r3, #16
 *     cmp r3, r5         @ r5 = 0x7fe0000
 *     bhi ...
 *
 * -- is gcc's own lowering of `v >= lo && v <= hi` on an unsigned short, the
 * standard `(unsigned)(v - lo) <= (hi - lo)` transform. It does NOT need to be
 * hand-written that way; the plain two-sided comparison produces it. The `lsl
 * #16` against a pre-shifted bound and the `lsl #16 / lsr #16` against an
 * unshifted one are the same test compiled two ways, and which one appears is
 * gcc's choice, not a source difference.
 *
 * Reading the bands back out of the constants: 0xffff97ff is -0x6801 and the
 * bound is 0x7fe, giving [0x6801, 0x6fff]; +0x17ff with bound 0x7fe wraps,
 * giving [0xe801, 0xefff]; -0x7001 with bound 0x7ffe gives [0x7001, 0xefff];
 * and everything else falls through. The third band overlaps the first two,
 * so the order of the tests is load-bearing and an `else if` chain is required
 * rather than four independent `if`s.
 *
 * THE SHARED TAIL IS CROSS-JUMPED, NOT SHARED IN THE SOURCE. Bands one and two
 * both store and then call with animation 3, and the ROM reaches the store
 * through `.L3588` from both. That is cross-jumping merging two identical
 * suffixes, exactly as in BuildDraw2DFuncs -- the arms are written out in full
 * with their own store and call, and gcc takes the common part back. Writing
 * the two bands as one arm with a conditional step does not reproduce it.
 *
 * The two tables needed no export: .L5b30 and .L5b40 are already `.global` in
 * ovl_30_c_c.s.
 *
 * Matched on the first candidate, no probing.
 */

extern unsigned short L5b40[] __asm__(".L5b40");
extern unsigned short L5b30[] __asm__(".L5b30");
extern void __MapActor_SetAnim(int slot, int anim);

void OvlFunc_943_200b558(int slot, int i)
{
    unsigned short v;

    v = L5b40[i];
    if (v >= 0x6801 && v <= 0x6fff) {
        L5b30[i] += 0x70;
        __MapActor_SetAnim(slot, 3);
    } else if (v >= 0xe801 && v <= 0xefff) {
        L5b30[i] += 0xe0;
        __MapActor_SetAnim(slot, 3);
    } else if (v >= 0x7001 && v <= 0xefff) {
        L5b30[i] += 0x1c0;
        __MapActor_SetAnim(slot, 2);
    } else {
        L5b30[i] += 0x300;
        __MapActor_SetAnim(slot, 1);
    }
}
