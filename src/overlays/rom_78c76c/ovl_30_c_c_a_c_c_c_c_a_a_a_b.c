// fakematch
/* OvlFunc_891_20095d4  --  0x020095d4
 *
 * From goldensun/asm/overlays/rom_78c76c/ovl_30_c_c_a_c_c_c_c_a_a_a.s.
 *
 * Member of shape group 0 (branch, {beq, bl, bx, cmp, lsl, mov, pop, push}),
 * from tools/shape_groups.py after group 0 of batch 201 was cleared.
 *
 * A four-argument call whose arguments interleave, then a test-and-call:
 *
 *     mov r1, #0xd0 / mov r2, #0xe0 / mov r0, #2 / lsl r1, #16 / lsl r2, #15 / mov r3, #0
 *
 * `mov r0` is written inside the two shift builds and `mov r3` after both.
 * Pinning all four argument registers and assigning them in the ROM's order
 * places them. This is the first function in these batches to need r3 pinned as
 * well -- the shape is the familiar one, just one argument wider.
 *
 * The guard is written as a plain `if (call(...) != 0) tail();`. The ROM's
 * `cmp r0, #0 / beq` over a single tail call falls out of that with no lever,
 * which is the branch-polarity rule behaving as documented.
 *
 * TWIN: OvlFunc_891_20095fc in the same .s is the same function with different constants
 * and a different tail callee. Both were written from one template and both
 * matched on the first screen.
 */

extern void __Func_8012078(int a, int b, int c, int d);
extern int OvlFunc_891_2009be8(int a, int b, int c);
extern void OvlFunc_891_200a244(void);

void OvlFunc_891_20095d4(void)
{
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        register int q3 __asm__("r3");
        q1 = 0xd0;
        q2 = 0xe0;
        q0 = 2;
        q1 <<= 16;
        q2 <<= 15;
        q3 = 0;
        __Func_8012078(q0, q1, q2, q3);
    }
    if (OvlFunc_891_2009be8(0xa, 0xe, 7) != 0)
        OvlFunc_891_200a244();
}
