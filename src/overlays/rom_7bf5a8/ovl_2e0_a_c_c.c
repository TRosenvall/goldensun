/* OvlFunc_935_20083e0 extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_a_c_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile. The
 * flag id is READ IN THE GUARD AND WRITTEN IN THE BODY, which batch 50 named as
 * the recognition rule for this class: gcc`s second CSE pass hoists it into a
 * callee-saved register across the intervening calls, spending a push and a pop
 * to save one pool load, while the ROM simply loads it twice.
 *
 * 18 instructions against 16 without the flag. Near-twin of ovl_2e0_a_c_a.c,
 * which has one extra call before the second guard.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __PlaySound(int id);
extern int OvlFunc_935_2008334(void);
extern void OvlFunc_935_2008398(void);

void OvlFunc_935_20083e0(void)
{
    if (__GetFlag(0x9a9))
        return;
    if (!OvlFunc_935_2008334())
        return;
    __SetFlag(0x9a9);
    __PlaySound(0x50);
    OvlFunc_935_2008398();
}
