// fakematch
/* OvlFunc_963_2008288  --  0x02008288
 *
 * Cut out of goldensun/asm/overlays/rom_7ec968/ovl_30_c_c_a_a_c.s.
 *
 * PARKED AT 2 OF 46 as an "argument precompute" bind, with the note that
 * "nothing in the source separates them":
 *
 *     rom   mov r2, #0x10 / mov r1, #0x3 / neg r2, r2 / mov r0, #0x0
 *     ours  mov r2, #0x10 / neg r2, r2   / mov r1, #0x3 / mov r0, #0x0
 *
 * Pinning the three argument registers and assigning them in the ROM's order
 * matches on the first screen -- the same form that closed
 * src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c_b.c earlier this batch.
 *
 * THIS ONE WAS PREDICTED, which is the reason to record it. Between those two
 * elevations sat src/non_matching/ovl_7ebdfc/2008120.c, the same mov/neg split
 * around another argument, where FOUR pin arrangements all failed. Comparing
 * the three sites gives the discriminator:
 *
 *     20086a4   mov r2, #0x10 / mov r1, #0x2 / neg r2   -- interleaved arg is 2   MATCHED
 *     2008288   mov r2, #0x10 / mov r1, #0x3 / neg r2   -- interleaved arg is 3   MATCHED
 *     2008120   mov r2, #0x10 / mov r0, #0 / mov r1, #0 -- BOTH interleaved args are 0, PARKED
 *
 * Where the interleaved argument is a distinct value with the mov/neg pair to
 * order against, the pin places it. Where the interleaved arguments are zeros
 * with NO consuming operation of any kind, nothing in the source can place
 * them -- the batch-195 rule from its other side: mov order follows the order
 * of consuming operations, and a mov with no consumer has nothing to follow.
 *
 * The prediction was made in the 2008120 park before this function was
 * attempted, and this is its confirmation. The cheap check before spending
 * screens on one of these: does each interleaved argument have an operation
 * consuming it?
 *
 * KEPT FROM THE PARK, and still load-bearing: the two 2s passed to both
 * __CopyMapTiles calls are a NAMED LOCAL, because the ROM keeps the value in a
 * register across them rather than rebuilding it.
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
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0x10;
        q1 = 3;
        q2 = -q2;
        q0 = 0;
        __Func_8092208(q0, q1, q2);
    }
    __Func_8091e9c(v);
}
