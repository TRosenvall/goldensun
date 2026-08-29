/* Cluster OvlFunc_948_20090b8..OvlFunc_948_20090b8 extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_a.s.
 *
 * Slotted between ..._c_a_a.o and the rest of the overlay.
 *
 * Three guards, each an early return to the shared epilogue. The flag id is
 * `n + (0x9c << 4)` and is computed once into a local, because the ROM keeps it
 * in r5 across two calls.
 */
extern void *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern int __CheckPartyItem(int item);
extern void OvlFunc_948_2008f40(int a);
extern void __Func_80789dc(int a);

void OvlFunc_948_20090b8(int n)
{
    void *a;
    int flag;

    a = __MapActor_GetActor(0);
    if (*(unsigned short *)((unsigned char *)a + 6) != (0xc0 << 8))
        return;
    flag = n + (0x9c << 4);
    if (__GetFlag(flag))
        return;
    if (__CheckPartyItem(0xf4) == -1)
        return;
    __SetFlag(flag);
    OvlFunc_948_2008f40((0x80 << 1) | n);
    __Func_80789dc(0xf4);
}
