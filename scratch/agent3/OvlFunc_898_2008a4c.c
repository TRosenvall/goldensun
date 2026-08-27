/* OvlFunc_898_2008a4c -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_793768/ovl_314_c_c_a_c_c_c_a_a.s
 * Best screen: 51 instructions against the ROM's 50.
 *
 * BLOCKER CLASS: literal-pool placement -- a FOURTH member of the cutscene
 * bookend family, alongside src/overlays/rom_793768/ovl_314_c_c_c_a_a_c_a_b.c
 * (matched), src/non_matching/overlays/2008acc.c and
 * src/non_matching/overlays/2008640.c.
 *
 * This one has an `if` whose exit and the pool-skip branch COINCIDE in the ROM:
 * one `b .La98` serves both, with `.pool_aligned` between it and the label. We
 * emit two labels and two branches, which is the extra instruction.
 *
 * So the family now has three distinct outcomes on the same shape -- one
 * matches, one is off by one instruction of pool placement (2008acc), one is
 * off by scheduling (2008640), and this one merges a branch the ROM shares.
 * The pooled 2 is `_CONST_2` in all four and behaves identically.
 *
 * The body screens clean: the walked +0x64 pointer, the signed facing saved and
 * restored, the flag test on save bit 2, and the counter bump at
 * [iwram_3001ebc]+0x1d8.
 */
struct A {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x64 - 8];
    unsigned short f64;
};

extern int _CONST_2;
extern char *iwram_3001ebc;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __MapActor_SetAnim(int slot, int n);
extern void __WaitFrames(int n);
extern void OvlFunc_898_200973c(int a, int b, int c);
extern void OvlFunc_898_2009724(int a, int b);

void OvlFunc_898_2008a4c(void)
{
    struct A *a;
    unsigned short *p;
    unsigned short *q;
    unsigned short two;
    short saved;

    a = __MapActor_GetActor(0xe);
    p = &a->f64;
    saved = a->f6;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    __MessageID(0x1339);
    if (__GetFlag(2)) {
        q = (unsigned short *)(iwram_3001ebc + (0xec << 1));
        *q = *q + 1;
    }
    __MapActor_SetAnim(0xe, 0);
    OvlFunc_898_200973c(0xe, 0, 2);
    OvlFunc_898_2009724(0xe, 0xa);
    a->f6 = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    *p = *p & 1;
}
