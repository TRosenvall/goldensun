/* Cluster OvlFunc_948_2009070..OvlFunc_948_2009070 extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_a_a.s.
 *
 * Slotted between ovl_30_c_a_c_c_a_a_c_a_a_a.o and the rest of the overlay.
 *
 * BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile and the
 * standing item in HANDOFF.md. The flag id is read in a guard and written in
 * the body on the SAME path, so gcc`s second CSE pass hoists it into a
 * callee-saved register across the call; the ROM loads it twice.
 *
 * Three guards, each an early return -- same shape as OvlFunc_948_20090b8 in
 * batch 44, which is the near-twin one overlay slot away. 30 instructions
 * against 28 without the flag.
 */
extern void *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern int __CheckPartyItem(int item);
extern void OvlFunc_948_2008fdc(int a);
extern void __Func_80789dc(int a);

void OvlFunc_948_2009070(void)
{
    void *a;

    a = __MapActor_GetActor(0);
    if (*(unsigned short *)((unsigned char *)a + 6) != (0xc0 << 8))
        return;
    if (__GetFlag(0x9c4))
        return;
    if (__CheckPartyItem(0xf3) == -1)
        return;
    __SetFlag(0x9c4);
    OvlFunc_948_2008fdc(0x80 << 1);
    __Func_80789dc(0xf3);
}
