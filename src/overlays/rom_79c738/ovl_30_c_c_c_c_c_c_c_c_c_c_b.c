// fakematch
/* OvlFunc_909_2009984  --  0x02009984
 *
 * From goldensun/asm/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_c.s.
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
 *     mov r1, #0xc0 / mov r0, #1 / lsl r1, #8 / mov r2, #0
 *
 * and gcc emits it before or after the whole block. Pinning the argument
 * registers and assigning them in the ROM's order places it.
 *
 * Three calls to one helper, identical but for the slot and one constant, and
 * all three want the same interleave. The whole function is that pattern
 * repeated.
 */

extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_909_2009984(void)
{
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xc0;
        q0 = 1;
        q1 <<= 8;
        q2 = 0;
        __Func_8092adc(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xc0;
        q0 = 2;
        q1 <<= 8;
        q2 = 0;
        __Func_8092adc(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xa0;
        q0 = 3;
        q1 <<= 8;
        q2 = 0;
        __Func_8092adc(q0, q1, q2);
    }
}
