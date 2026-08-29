/* Cluster OvlFunc_922_20085b8..OvlFunc_922_20085b8 extracted from goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_a_c_c_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_7a8c8c/overlay.ld is
 * unchanged.
 *
 * A lever being thrown: play a sound, move something to one of two positions
 * depending on two save flags, set the flags to match, and hand off.
 *
 * The control flow is a plain `||` diamond and comes out right written as one:
 *
 *     bne .L0    -- first flag set, take the if body
 *     beq .L1    -- second flag clear, take the else
 *     (fall through into .L0)
 *
 * which is exactly what gcc emits for `if (A || B) { X } else { Y }`.
 *
 * ONE TRANSPOSITION had to be fixed, on OvlFunc_922_2008180:
 *
 *     rom    mov r1, #0x30 / neg r1, r1 / mov r0, #0x8 / mov r2, #0x0
 *     ours   mov r1, #0x30 / neg r1, r1 / mov r2, #0x0 / mov r0, #0x8
 *
 * The ROM fills r0 BEFORE r2; gcc filled it after. Declaring the callee --
 * `extern void OvlFunc_922_2008180(int, int, int);` -- reverses the order gcc
 * fills that call's own argument registers and matches. Everything else in the
 * file is left implicit, which is the house style and is load-bearing here:
 * adding a prototype for __GetFlag as well still matches, but adding ONLY the
 * __GetFlag prototype does not, so it is the callee's own declaration doing the
 * work and not the preceding call's return type.
 *
 * See docs/elevation.md -- this is the second of the two declaration levers,
 * the one about the mismatching call itself rather than the call before it.
 *
 * Both 0x310 and 0x308 are built at runtime (`mov #0xc4 / lsl #2`), so they are
 * written as `0xc4 << 2` and `0xc2 << 2` rather than as the folded constants;
 * either spelling folds to the same value, but writing the shift keeps the
 * relationship to the flag block visible.
 *
 * 0x308 and 0x309 each appear twice, once per arm. They are on mutually
 * exclusive paths, so gcc has no opportunity to cache either and the
 * constant-CSE blocker does not arise -- unlike OvlFunc_899_200852c in
 * rom_794ac0, which reads and sets the same flag id on ONE path and is parked
 * for exactly that reason.
 */
extern void OvlFunc_922_2008180(int a, int b, int c);

void OvlFunc_922_20085b8(void)
{
    __PlaySound(0xf1);
    if (__GetFlag(0xc4 << 2) != 0 || __GetFlag(0x30d) != 0) {
        OvlFunc_922_2008180(8, -0x30, 0);
        __ClearFlag(0xc2 << 2);
        __SetFlag(0x309);
    } else {
        OvlFunc_922_2008180(8, -0x60, 0);
        __SetFlag(0xc2 << 2);
        __ClearFlag(0x309);
    }
    __PlaySound(0x121);
    __WaitFrames(2);
    OvlFunc_922_20092cc();
}
