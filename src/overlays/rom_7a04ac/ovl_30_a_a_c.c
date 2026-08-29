/* Cluster OvlFunc_913_20089fc..OvlFunc_913_20089fc extracted from goldensun/asm/overlays/rom_7a04ac/ovl_30_a_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * An actor update. Advances the gradual-turn target at +0x64 by a random
 * amount, picks one of two configuration values from it, and wraps it to zero
 * past a second threshold. Always returns 1.
 *
 * THE FIELD IS SIGNED HERE. actor.h calls +0x64 `goalFacing` and types it u16,
 * but both comparisons in the ROM are signed 16-bit: the first shifts the live
 * value and the threshold left by 16 and compares those (`lsl r3,#16 /
 * mov r2,#0xfa / lsl r2,#18 / cmp r3,r2` -- 0xfa<<18 is 1000<<16), and the
 * second reloads with `ldrsh`. An unsigned field produces neither. So this
 * accesses it through a `short *`, and the header's type is left alone: one
 * overlay reading it signed is not enough to retype a field the whole tree
 * shares.
 *
 * THE INVERTED narrow_constant CASE, second sighting. `*p = 0;` written
 * directly compiles to `ldrh r3, .L3` -- gcc pools a zero rather than moving
 * it, because the destination is narrow. Assigning through a named `int` first
 * restores `mov r3, #0`. Same lever, same direction, as
 * src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_b.c, which needed it for a 1.
 * Worth trying on any diff that is a pooled small constant where the ROM has a
 * mov.
 */

extern unsigned int __Random(void);
extern void __Func_80929d8(void *actor, int n);

int OvlFunc_913_20089fc(void *actor)
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
