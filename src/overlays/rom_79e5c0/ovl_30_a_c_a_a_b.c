/* Cluster OvlFunc_911_2008050..OvlFunc_911_2008050 extracted from goldensun/asm/overlays/rom_79e5c0/ovl_30_a_c_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_79e5c0/ovl_30_a_c_a_a_a.o and
 * asm/overlays/rom_79e5c0/ovl_30_a_c_a_a_c.o in
 * goldensun/overlays/rom_79e5c0/overlay.ld.
 *
 * A byte-for-byte cross-overlay copy of src/overlays/rom_7a04ac/ovl_30_a_a_c.c.
 * Both levers are that file's and both are needed here:
 *
 *   - actor +0x64 is read through a `short *`. actor.h types `goalFacing` as
 *     u16, but both comparisons in the ROM are signed sixteen-bit -- one by
 *     shifting both operands left 16, one by `ldrsh` -- which an unsigned field
 *     does not produce. The header is still left alone.
 *   - `*p = 0` goes through a named `int`, or gcc pools the zero as a halfword
 *     constant instead of moving it. That is narrow_constant running inverted.
 */
extern unsigned int __Random(void);
extern void __Func_80929d8(void *actor, int n);

int OvlFunc_911_2008050(void *actor)
{
    short *p = (short *)((unsigned char *)actor + 0x64);
    int z;

    *p = *p + ((__Random() * 100) >> 16);
    if (*p > 1000)
        __Func_80929d8(actor, 7);
    else
        __Func_80929d8(actor, 0xa);
    if (*p > 1200) {
        z = 0;
        *p = z;
    }
    return 1;
}
