/* Cluster OvlFunc_935_200848c..OvlFunc_935_200848c extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_c_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * A one-shot reward: if the flag is clear and a condition holds and a second
 * flag is clear, play a chime, give the thing, and set the flag.
 *
 * Built with CSE_CFLAGS. The flag id 0x9aa is read at the top and written at
 * the bottom with three calls in between, and at -O2 gcc hoists it into a
 * callee-saved register -- 23 instructions against 21, diverging from
 * instruction zero because the ROM needs no push at all.
 *
 * FOUND BY THE SWEEP RATHER THAN BY READING. tools/pick_candidates.py rejects
 * candidates that load the same pooled constant twice, because that is the
 * constant-CSE shape. Once the flag was understood, running the scan WITH
 * `--allow-repeat` turned that rejection list into a worklist: five candidates,
 * three of which matched immediately with the flag. This is one of them.
 *
 * The three guards are written as one `&&` chain because that is what the ROM's
 * three consecutive `bne/beq/bne` to a common exit label is -- each test jumps
 * to the same place, which is short-circuit evaluation and not three separate
 * ifs.
 */
void OvlFunc_935_200848c(void)
{
    OvlFunc_935_2008170();
    if (__GetFlag(0x9aa) == 0 && OvlFunc_935_2008458() != 0 && __GetFlag(0x207) == 0) {
        __PlaySound(0x50);
        OvlFunc_935_2008410();
        __SetFlag(0x9aa);
    }
}
