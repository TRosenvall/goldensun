/* Cluster OvlFunc_941_2008460..OvlFunc_941_2008460 extracted from goldensun/asm/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_c.s.
 *
 * Slotted between ovl_30_c_a_c_c_c_a_c_a.o and the rest of the overlay.
 *
 * BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile. The
 * flag id is READ IN THE GUARD AND WRITTEN IN THE BODY, which batch 50 named as
 * the recognition rule for this class: gcc`s second CSE pass hoists it into a
 * callee-saved register across the intervening calls, spending a push and a pop
 * to save one pool load, while the ROM simply loads it twice.
 *
 * 25 instructions against 23 without the flag. Near-twin of
 * ovl_30_c_a_c_c_c_a_b.c (OvlFunc_941_200833c) from batch 50: same body with
 * the two guards swapped and a different callee.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_801776c(int a, int b);
extern void __PlaySound(int id);
extern void OvlFunc_941_2008384(void);

void OvlFunc_941_2008460(void)
{
    if (__GetFlag(0x80 << 2))
        return;
    if (__GetFlag(0x202))
        return;
    __Func_801776c(0x1528, 1);
    __PlaySound(0x9d);
    OvlFunc_941_2008384();
    __SetFlag(0x202);
    __ClearFlag(0x201);
}
