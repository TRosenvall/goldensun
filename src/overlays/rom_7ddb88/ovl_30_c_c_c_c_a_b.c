// fakematch
/* OvlFunc_955_2008b38  --  0x02008b38
 *
 * Cut out of goldensun/asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_a.s.
 *
 * Sixty-three instructions. THE TWO CROSSED CURES ARE COMPLEMENTARY, and this
 * function is the clean demonstration -- the first case where both were needed
 * to be tried before one worked.
 *
 * The closing __Func_80933f8 fill is
 *
 *     mov r0, #0xd0 / mov r2, #0xc0 / mov r1, #0 / lsl r2, #16 /
 *     mov r3, #0 / lsl r0, #15
 *
 * -- movs r0, r2, r1 against shifts r2, r0. Batch 212's cure, writing the
 * shifts in the movs' order, gets the MOVS right and leaves the shifts
 * transposed: 2 of 63. Writing the fill in the ROM's literal order instead gets
 * the SHIFTS right and leaves the movs transposed: also 2 of 63, at a different
 * instruction. That is the two-state trap exactly as batch 195 described it,
 * and neither state is the ROM.
 *
 * A volatile asm after `q0 = 0xd0`, with the ROM's literal order for the rest,
 * is exact. So the barrier remains the general cure and the reordering is the
 * cheap one to try first; where the reordering reaches only one of the two
 * orders, the barrier resolves the pair. This is the first function in the tree
 * where the reordering was tried and a barrier was still required, which bounds
 * the claim batch 213 made that no such case had yet appeared.
 *
 * The rest is ordinary: four pinned argument fills, a two-armed animation
 * choice on the sign of the parameter, and a tail call passing it through.
 */
extern void OvlFunc_common1_fac(int a);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __DeleteFieldActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_807808c(int a);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_955_2008b38(int a)
{
    __DeleteFieldActor(0x28);
    __DeleteFieldActor(0x29);
    __Func_807808c(1);
    __CutsceneStart();
    { PIN3; q1 = 0xb0; q2 = 0x80; q0 = 8; q1 <<= 15; q2 <<= 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xf0; q2 = 0x80; q0 = 0; q1 <<= 15; q2 <<= 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 8; q1 <<= 7; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0; __Func_809280c(q0, q1, q2); }
    if (a < 0) {
        __MapActor_SetAnim(8, 0xa);
        __MapActor_SetAnim(0, 0x23);
    } else {
        __MapActor_SetAnim(8, 8);
        __MapActor_SetAnim(0, 0x1c);
    }
    __WaitFrames(1);
    {
        PIN4;
        q0 = 0xd0; __asm__ volatile ("" : : "r" (q0));
        q2 = 0xc0; q1 = 0; q2 <<= 16; q3 = 0; q0 <<= 15;
        __Func_80933f8(q0, q1, q2, q3);
    }
    OvlFunc_common1_fac(a);
    __CutsceneEnd();
}
