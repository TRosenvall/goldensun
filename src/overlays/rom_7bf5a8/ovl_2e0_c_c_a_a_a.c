/* Cluster OvlFunc_935_20084d0..OvlFunc_935_20084d0 extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * Near-twin of src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c in the same overlay,
 * differing only in that this one does not call OvlFunc_935_2008170 first.
 * Read that header for the `&&` chain and for why this TU is built with
 * CSE_CFLAGS.
 */
void OvlFunc_935_20084d0(void)
{
    if (__GetFlag(0x9aa) == 0 && OvlFunc_935_2008458() != 0 && __GetFlag(0x207) == 0) {
        __PlaySound(0x50);
        OvlFunc_935_2008410();
        __SetFlag(0x9aa);
    }
}
