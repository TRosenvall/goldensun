/* Cluster OvlFunc_920_20084b4..OvlFunc_920_20084b4 extracted from goldensun/asm/overlays/rom_7a6ae4/ovl_30_c_c_a_a_a.s.
 *
 * Slotted between ovl_30_c_c_a_a_a_a.o and the rest of the overlay.
 *
 * Stack-arg-pair lever, standard form: both values named, in the order the ROM
 * stores them, immediately before the call.
 */
extern int __GetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetAnim(int slot, int anim);

void OvlFunc_920_20084b4(void)
{
    int m;
    int n;

    if (__GetFlag(0x305)) {
        m = 8;
        n = 0xd;
        __Func_8010704(0x1f, 0, 1, 1, m, n);
        __MapActor_SetAnim(8, 0);
    }
}
