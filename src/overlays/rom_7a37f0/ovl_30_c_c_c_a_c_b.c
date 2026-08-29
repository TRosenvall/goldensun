/* OvlFunc_916_2008ecc  --  0x02008ecc, cut from goldensun/asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted after
 * asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c.o in goldensun/overlays/rom_7a37f0/overlay.ld.
 *
 * A BGR555 warming fade: pull `rate`-th parts out of green and blue, push a
 * quarter of one back into red, clamp red at 0x1f and repack. Every component
 * is a `short`, which is where all six `lsl #16 / asr #16` pairs come from.
 *
 * One of FOUR byte-identical copies, one per overlay -- OvlFunc_914_2008b24,
 * OvlFunc_915_2008cf4, OvlFunc_916_2008ecc, OvlFunc_917_20097d0. Found with
 * tools/find_twins.py, which ranked it second by payoff behind the
 * thirteen-member group.
 *
 * TWO THINGS MADE THIS LOOK HARDER THAN IT IS, and both were tooling.
 *
 * 1. THE SCREEN REPORTED A MISMATCH THAT DOES NOT EXIST IN THE BYTES.
 *    gcc spells the pooled mask
 *
 *        ldrh r2, .L0
 *
 *    because the AND is HImode, while the ROM disassembly writes
 *
 *        ldr r2, =0x1f
 *
 *    Thumb has no pc-relative `ldrh`, so gas assembles gcc's line to the very
 *    same halfword, 0x4a0d = `LDR r2, [pc, #52]`. tryc.py compares assembly
 *    TEXT and normalises only the `=` spelling, so it reported this as a
 *    difference for as long as the candidate existed. Confirmed by objdump of
 *    both objects: identical from the first byte to the last.
 *
 * 2. `pick_candidates.py` REJECTED THIS FUNCTION OUTRIGHT because the ROM keeps
 *    its literal pool inside the body behind a `.pool_aligned` -- the batch-30
 *    rule. gcc reproduces that placement here exactly, `b` around the pool and
 *    all, and the reason is the same HImode load: a HImode pool reference has a
 *    pool_range of 32-60 bytes in arm.md against SImode's 1020, so the pool
 *    cannot wait for the barrier at the end of the function and gets dumped
 *    mid-body with a manufactured jump over it.
 *
 * THE FALSE LEAD, RECORDED BECAUSE IT WAS CONVINCING. Reading `ldr r2, =0x1f`
 * as the pool tell -- gcc never pools what `mov r2, #0x1f` can build, therefore
 * the operand was a symbol -- produced `(int)&_K_1f`, which reproduces the
 * ROM's assembly TEXT exactly. It is wrong. An SImode symbol load has the full
 * 1020-byte range, so the pool then moves to the end of the function and the
 * `b` disappears: right text, wrong bytes. The narrow mask was never a symbol.
 * Check bytes, not text, before believing a pool-tell diagnosis.
 *
 * Three ordinary levers finished the body, all already on the books:
 *   - `colour` is `unsigned short`, so its zero-extension `lsl r0, #16` is
 *     shared by all three field extractions;
 *   - the locals are declared `b, g, r` -- declaration order picks which of
 *     r5/r7 holds green;
 *   - the repack is `r | ((b << 10) | (g << 5))`, not left-associated, which is
 *     what puts blue's shift first and combines the two into r3 before r6.
 */

unsigned short OvlFunc_916_2008ecc(unsigned short colour, int rate)
{
    short b, g, r;

    r = colour & 0x1f;
    g = (colour >> 5) & 0x1f;
    b = (colour >> 10) & 0x1f;
    r = r + r / (rate * 4);
    g = g - g / rate;
    b = b - b / rate;
    if (r > 0x1f)
        r = 0x1f;
    return r | ((b << 10) | (g << 5));
}
