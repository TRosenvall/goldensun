// fakematch
/* OvlFunc_905_20089dc  --  0x020089dc
 *
 * From goldensun/asm/overlays/rom_799abc/ovl_30_a_a_a_c_c_c_c_a.s.
 *
 * FIRST MEMBERS OF SHAPE GROUP 0, the new largest group in the unelevated
 * corpus once arg_interleave_flat was closed in batch 200. tools/shape_groups.py
 * ranks remaining functions by their SET of opcodes; this group is
 *
 *     flat (no branches), opcodes = {bl, bx, lsl, mov, pop, push}
 *
 * -- the closed class's signature minus `ldr`, so the same interleave with no
 * pool loads anywhere. Six members, and the first three screened all matched
 * on the first try.
 *
 * The blocker is the one the closed class taught: the ROM writes an argument
 * INSIDE another register's two-instruction build,
 *
 *     mov r1, #0x80 / mov r0, #0xd / lsl r1, #1 / mov r2, #0
 *
 * and gcc emits it before or after the whole block. Pinning the argument
 * registers and assigning them in the ROM's order places it.
 *
 * Only the FIRST of its three calls needs the pin; the other two take their
 * arguments in gcc's natural order and are written plainly. Pinning a site
 * that does not need it has cost matches before -- see
 * src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_a_c_b.c -- so each call is
 * read off the listing separately.
 */

extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __Func_8091f14(int a, int b);

void OvlFunc_905_20089dc(void)
{
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q0 = 0xd;
        q1 <<= 1;
        q2 = 0;
        __MapActor_Emote(q0, q1, q2);
    }
    __MapActor_Jump(0xd, 2, 0);
    __Func_8091f14(0xc, 0x28);
}
