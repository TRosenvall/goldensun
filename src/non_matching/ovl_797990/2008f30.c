/* OvlFunc_901_2008f30  [overlays/rom_797990]
 *
 * Source asm: goldensun/asm/overlays/rom_797990/ovl_314_c_c_c.s
 *
 * BLOCKER: four independent residues, 19 of 123 (from 60 -> 25 -> 21 -> 19).
 *
 *   1. THE SAME ldrh/ldrsh CSE CLASS as OvlFunc_912_20081c4 (~5 lines). Writing
 *      the value as an `unsigned short` local assigned twice gets the HImode
 *      compare and the double read, but gcc emits both reads as
 *      `mov r2,#0 / ldrsh` where the ROM has a plain `ldrh`.
 *   2. `push {r5, r6}` versus ours `push {r5}` (2). The ROM keeps a stored zero
 *      in callee-saved r6 for a single `strh`. `int z = 0;` at the top, at the
 *      store, threaded through a callee's second argument, and as a `z=1; z=0;`
 *      pair were all screened -- gcc picks r3 every time.
 *   3. An offset-register rename (~6, two sites): the ROM builds `0xe1 << 1` in
 *      r1, gcc in r2. Pure transposition. NOTE the offset must STAY a named
 *      local -- inlining it folds the address into one pool word and costs
 *      three instructions (68 differing).
 *   4. One `lsl r0, #0x10` three positions early. Here the literal beats the
 *      dominating-block local, 19 versus 21.
 *
 * FIXED ON THE WAY, worth reusing: naming the two shifted __MapActor_SetPos
 * arguments in the dominating block above each guard killed BOTH `mov r0`
 * misplacements at once; `int one = 1;` next to the store turns gcc's
 * `ldr r3, =1` HImode pool load back into the ROM's `mov r3, #1`; and
 * `p += 0x64; *(short *)p = one;` gives the ROM's destructive `add r0, #0x64`.
 */
extern unsigned char gState[];

extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_common0_70(int a, int b, int c, int d);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_901_2008400(void);

int OvlFunc_901_2008f30(void)
{
    unsigned char *g;
    unsigned char *p;
    void *f;
    int a1, a2, b1, b2;
    int o1, o2;
    int s1, s2;
    int one;
    int x;
    int z;

    z = 0;
    if (__GetFlag(0x80 << 2)) {
        s1 = 0x17;
        s2 = 0x1a;
        __Func_8010704(0x37, 0x1a, 4, 2, s1, s2);
    }
    OvlFunc_common0_70(0x80 << 16, 0, 0xd2 << 17, 0xdf);
    __CopyMapTiles(0x2d, 0x29, 8, 0x2d, 3, 3);
    __WaitFrames(1);
    p = __MapActor_GetActor(0xe);
    f = OvlFunc_901_2008400;
    *(void **)(p + 0x6c) = f;
    p = __MapActor_GetActor(0xe);
    one = 1;
    p += 0x64;
    *(short *)p = one;
    p = __MapActor_GetActor(0xf);
    *(void **)(p + 0x6c) = f;
    p = __MapActor_GetActor(0xf);
    p += 0x64;
    *(short *)p = z;
    o1 = 0xe1 << 1;
    a1 = 0xd8 << 16;
    a2 = 0xc4 << 17;
    if (__GetFlag(0x858))
        __MapActor_SetPos(0x12, a1, a2);
    if (*(short *)(gState + o1) <= 2 && __GetFlag(0x34) == 0 && __GetFlag(0x109) == 0)
        __ClearFlag(0x867);
    b1 = 0xcc << 17;
    b2 = 0xf0 << 15;
    if (__GetFlag(0x867) && __GetFlag(0x34) == 0)
        __MapActor_SetPos(0x15, b1, b2);
    {
        unsigned short e;
        o2 = 0xe1 << 1;
        g = gState + o2;
        e = *(unsigned short *)g;
        if (*(short *)g == 0xb) {
            __ClearFlag(0x12f);
            e = *(unsigned short *)g;
        }
        if (e == 0xd)
            __ClearFlag(0x90 << 1);
    }
    return 0;
}
