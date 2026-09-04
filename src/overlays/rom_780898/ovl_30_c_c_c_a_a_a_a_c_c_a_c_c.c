// fakematch
/* OvlFunc_883_2008dc0  --  0x02008dc0
 *
 * From goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_a_c_c_a_c_c.s, which held this function alone.
 *
 * A MEMBER OF THE "arg_interleave_flat" CLASS, which
 * src/non_matching/overlays/arg_interleave_flat.c described as fourteen
 * functions sharing one blocker and concluded "these need a compiler-level
 * answer, not a source-level one". They do not. The shape is
 *
 *     mov r2, #0xd2 / mov r0, #0 / ldr r1, =0x101 / lsl r2, #1
 *
 * with `mov r0` and the pooled `ldr r1` written INSIDE another register's two-instruction build. Pinning
 * the argument registers and assigning them in the ROM's order matches on the
 * first screen.
 *
 * The class note's reasoning was the batch-43 basic-block lever: moving an
 * argument constant needs a local in a block that DOMINATES the call, every
 * member is straight-line, so `REG_BASIC_BLOCK (regno) < 0` can never hold.
 * That is correct about that lever and is why the group accumulated -- the
 * candidate ranker was refusing these for many rounds, for a reason that was
 * true of the tools available at the time.
 *
 * A pin does not go through basic blocks. It names the hard register, so the
 * placement is decided at the assignment.
 *
 * The `.L` data label is reached with the tree's asm-renamed extern idiom,
 * since C cannot spell a name beginning with a dot.
 */

extern unsigned char L7544[] __asm__(".L7544");
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_883_2008dc0(void)
{
    __PlaySound(0xbc);
    __Func_8010560(L7544, 0x2d, 0xb);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0xd2;
        q0 = 0;
        q1 = 0x101;
        q2 <<= 1;
        __Func_809218c(q0, q1, q2);
    }
    __Func_8091e9c(0xb);
}
