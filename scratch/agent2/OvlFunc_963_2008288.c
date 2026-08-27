/* OvlFunc_963_2008288 (CrossThreshold)  --  NOT MATCHING, 2 differing of 46
 * ref: asm/overlays/rom_7ec968/ovl_30_c_c_a_a_c.s
 *
 * Everything matches except the last call's argument block:
 *     rom   mov r2, #0x10 / mov r1, #0x3 / neg r2, r2 / mov r0, #0x0
 *     ours  mov r2, #0x10 / neg r2, r2 / mov r1, #0x3 / mov r0, #0x0
 * i.e. the straight-line arg-interleave class -- the ROM wedges `mov r1, #3`
 * into the two-instruction build of -0x10, and there is no branch in the
 * function for the basic-block lever to use.
 *
 * Measured, all 2 of 46: `n = 0x10; n = -n;` as its own statements, `n = -0x10;`
 * as a local, both operands named (in either assignment order), an int return
 * type on __Func_8092208, no declaration for __Func_8092208, an int return on
 * the preceding __WaitFrames, and -fno-rerun-cse-after-loop /
 * -fno-schedule-insns / -fno-peephole / -fno-caller-saves / -fno-force-mem.
 * -fno-schedule-insns2 and -O1 are 25.
 *
 * Three details that ARE settled and worth keeping:
 *  - the zero index of the `ldrsh` is the inline `(unsigned int)0` form, not a
 *    named zero: the function stores a zero later and a named one gets merged;
 *  - the __MapActor_GetActor result must NOT go through a local -- inlined into
 *    the store expression it gives the ROM's `add r0, #0x55 / strb`;
 *  - the stack pair (2, 2) is the SAME value twice, so plain literals are
 *    right and gcc parks it in r5 by itself, exactly as the ROM does.
 */
extern char *iwram_3001ebc;
extern void *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern int __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_963_2008288(void)
{
    char *p;
    int t;

    p = iwram_3001ebc;
    p += 0xb6 << 1;
    t = *(short *)(p + (unsigned int)0);
    *((unsigned char *)__MapActor_GetActor(0) + 0x55) = 0;
    __PlaySound(0x9e);
    __CopyMapTiles(0x42, 0x24, 0x47, 8, 2, 2);
    __WaitFrames(4);
    __CopyMapTiles(0x44, 0x24, 0x47, 8, 2, 2);
    __WaitFrames(4);
    __Func_8092208(0, 3, -0x10);
    __Func_8091e9c(t);
}
