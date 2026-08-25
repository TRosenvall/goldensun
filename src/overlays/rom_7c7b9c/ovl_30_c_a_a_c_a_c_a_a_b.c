/* Cluster OvlFunc_943_2008bb8..OvlFunc_943_2008bb8 extracted from goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_a.s.
 *
 * Slotted between ovl_30_c_a_a_c_a_c_a_a_a.o and the rest of the overlay.
 *
 * BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile and the
 * standing item in HANDOFF.md. The flag id is read in the guard and written at
 * the end of the body -- the recognition rule from batch 50 -- and at plain -O2
 * gcc hoists it into a callee-saved register across the two intervening calls.
 * 25 instructions against 23 without the flag.
 *
 * Near-twin of ovl_30_c_a_a_c_a_c_a_a_c.c, differing in the flag id, one tile
 * coordinate and one stack value.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_943_2008bb8(void)
{
    int m;
    int n;

    if (__GetFlag(0x271))
        return;
    __PlaySound(0x9e);
    m = 1;
    n = 3;
    __CopyMapTiles(0x1e, 0x5e, 0xd, 0x5e, m, n);
    __SetFlag(0x271);
}
