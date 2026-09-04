// fakematch
/* OvlFunc_959_2008c78  --  0x02008c78
 *
 * From goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_a_a_b.s, which held this function alone.
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
 *     mov r1, #0xf8 / mov r2, #0xbc / mov r0, #0xc / lsl r1, #16 / lsl r2, #17
 *
 * and gcc emits it before or after the whole block. Pinning the argument
 * registers and assigning them in the ROM's order places it.
 *
 * Ten instructions: one placed call and a tail call. The smallest function
 * elevated in these batches.
 */

extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_959_2008b4c(void);

void OvlFunc_959_2008c78(void)
{
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xf8;
        q2 = 0xbc;
        q0 = 0xc;
        q1 <<= 16;
        q2 <<= 17;
        __MapActor_SetPos(q0, q1, q2);
    }
    OvlFunc_959_2008b4c();
}
