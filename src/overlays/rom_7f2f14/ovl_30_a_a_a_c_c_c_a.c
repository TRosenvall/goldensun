/* Cluster OvlFunc_968_20085ac..OvlFunc_968_20085ac extracted from goldensun/asm/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_c.s.
 *
 * Total .text for this TU = 52 bytes (= 0x34).
 * Preserves the original ROM layout when slotted immediately before
 * asm/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_c_b.o in
 * goldensun/overlays/rom_7f2f14/overlay.ld.
 *
 * Two independent reads of the same global with different masks. Both are
 * spelled out separately rather than cached in a local: gcc reloads, which is
 * what the ROM does, so no lever is needed here.
 */
extern unsigned int iwram_3001e40;
extern void __Func_80929d8(void *a, int n);
extern void __PlaySound(int id);

int OvlFunc_968_20085ac(void *a)
{
    if ((iwram_3001e40 & 3) == 0)
        __Func_80929d8(a, 7);
    else
        __Func_80929d8(a, 0);
    if ((iwram_3001e40 & 7) == 0)
        __PlaySound(0x8a);
    return 0;
}
