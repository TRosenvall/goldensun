/* OvlFunc_963_2008288  --  0x02008288, asm/overlays/rom_7ec968/ovl_30_c_c_a_a_c.s
 *
 * Source asm: goldensun/asm/overlays/rom_7ec968/ovl_30_c_c_a_a_c.s
 *
 * BLOCKER CLASS: argument precompute. 46 lines against 46, FORTY-FOUR
 * identical, and the two that differ are a transposition in one argument block:
 *
 *     rom    mov r2, #0x10 / mov r1, #0x3 / neg r2, r2 / mov r0, #0x0
 *     ours   mov r2, #0x10 / neg r2, r2 / mov r1, #0x3 / mov r0, #0x0
 *
 * `-0x10` costs more than 2 in arm_rtx_costs, so it is precomputed into a
 * pseudo before the hard registers are filled; the ROM interleaves the cheap
 * `mov r1, #3` between the `mov` and the `neg`. Nothing in the source separates
 * them.
 *
 * MEASURED, all 2 of 46: `-0x10` through a named `int`, `3` through a named
 * `int`, `0 - 0x10` written out, __Func_8092208 left implicit, __WaitFrames
 * given an `int` return type, __WaitFrames left implicit, __CopyMapTiles left
 * implicit.
 *
 * SOLVED ON THE WAY: the two `2`s that go to both __CopyMapTiles calls as
 * arguments five and six are a NAMED LOCAL, because the ROM keeps the value in
 * r5 across both calls -- batch 83's stack-argument lever. With literals gcc
 * rebuilds it per store.
 */
extern unsigned char iwram_3001ebc[];
extern void *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_963_2008288(void)
{
    char *base;
    short v;
    unsigned char *p;
    int two;

    base = *(char **)iwram_3001ebc;
    v = *(short *)(base + (0xb6 << 1));
    p = (unsigned char *)__MapActor_GetActor(0) + 0x55;
    *p = 0;
    __PlaySound(0x9e);
    two = 2;
    __CopyMapTiles(0x42, 0x24, 0x47, 8, two, two);
    __WaitFrames(4);
    __CopyMapTiles(0x44, 0x24, 0x47, 8, two, two);
    __WaitFrames(4);
    __Func_8092208(0, 3, -0x10);
    __Func_8091e9c(v);
}
