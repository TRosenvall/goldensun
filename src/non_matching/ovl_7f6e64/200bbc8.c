/* OvlFunc_969_200bbc8 (0x0200bbc8) -- NON-MATCHING, 218 encodings of 295,
 * size 720 against the ROM's 724 (-4).
 *
 * Blocker class: POOL PLACEMENT -- and this park exists mainly to record the
 * measurement discrepancy that produced it, because that is the reusable part.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7f6e64/200bbc8.c \
 *     asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_a_a_c.s \
 *     --func OvlFunc_969_200bbc8
 *
 * A NORMALISED SCREEN SAID 3.  objcmp SAYS 218.  BOTH ARE CORRECT AND THE GAP
 * IS ENTIRELY THE POOL.  The agent that wrote this candidate reported "3
 * differing"; that was a pool-offset-normalised figure.  objcmp, which does not
 * normalise, reports 218 -- and the arithmetic confirms the reading rather than
 * contradicting it: the instruction COUNTS are 295 against 293, a delta of only
 * two, so roughly 216 of the 218 are the same instructions with shifted
 * `ldr [pc, #N]` offsets, and the -4 bytes is one pool word.
 *
 * THIS IS tryc's DOCUMENTED BLIND SPOT #1 SEEN FROM THE OTHER SIDE, and it is
 * worth stating as a rule for large functions: A NORMALISED DIFFERENCE COUNT IS
 * NOT A DISTANCE TO EXACT WHEN THE POOL HAS MOVED.  Three normalised
 * differences with a displaced pool is a much worse position than three real
 * ones, because the pool is a single structural fact that has to be fixed
 * before any of the three can be trusted.  Quote objcmp, not the screen, in any
 * report -- that is what docs/elevation.md means by calling objcmp the
 * authority.
 *
 * WHAT IS ACTUALLY WRONG, per the agent's reading: the pool sits roughly 16
 * bytes below gcc's break threshold, so gcc keeps one pool where the ROM splits
 * or places differently.  The lever family for this is known and was productive
 * elsewhere in the same batch -- see the HImode-vs-SImode pool-range pair in
 * src/non_matching/rom_8a000/808e680.c (int local pushes the pool to the end)
 * and the landed src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_c_a_c.c (a
 * bare-literal HImode zero pulls it early and splits it three ways).
 *
 * NEXT: re-measure from scratch with objcmp ONLY, then work the pool with the
 * HImode/SImode pair above -- a 16-byte shortfall is squarely inside what one
 * MINIPOOL_FIX_SIZE change moves.  This is a promising park, not a stuck one;
 * it was simply never measured with the right tool.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char *__Func_8093554(void);
extern void __PlaySound(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __Func_8092950(int a, int b);
extern void __Func_8092b08(int a, int b);
extern void __StartTask(void *fn, int arg);
extern void OvlFunc_969_200b6d0(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __WaitFrames(int n);
extern void __Func_800fe9c(void);
extern void __Func_8012330(int a, int b, int c);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void __Func_80925cc(int a, int b);
extern void __MessageID(int id);
extern void OvlFunc_969_2008894(int a);
extern void __Func_8093040(int a, int b, int c);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __DeleteFieldActor(int slot);
__asm__(".equ Lzero, 0");
extern int Lzero;

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_969_200bbc8(void)
{
    unsigned char *b;
    unsigned char *p;
    unsigned char *p2;
    int n, nn;
    int sh, z, neg, t3, big, hi, k, m;

    __PlaySound(0x8d);
    n = 0x11;
    nn = 8;
    __Func_8010704(0x11, 0xa, 4, 2, n, nn);
    b = __MapActor_GetActor(0);
    sh = 0xa0 << 8;
    z = (int)&Lzero;
    *(unsigned short *)(b + 6) = sh;
    b = __MapActor_GetActor(1);
    *(unsigned short *)(b + 6) = sh;
    __MapActor_SetPos(1, 0xa4 << 17, 0xa8 << 16);
    b = __MapActor_GetActor(2);
    *(unsigned short *)(b + 6) = sh;
    __MapActor_SetPos(2, 0xaa << 17, 0xc4 << 16);
    b = __MapActor_GetActor(3);
    *(unsigned short *)(b + 6) = sh;
    __MapActor_SetPos(3, 0xa3 << 17, 0xcc << 16);
    b = __MapActor_GetActor(0x15);
    sh = 0xd0 << 8;
    *(unsigned short *)(b + 6) = sh;
    { PIN3; q1 = 0xc8; q2 = 0xd8; q1 <<= 16; q2 <<= 16; q0 = 0x15; __MapActor_SetPos(q0, q1, q2); }
    b = __MapActor_GetActor(6);
    *(unsigned short *)(b + 6) = sh;
    { PIN3; q1 = 0xc8; q2 = 0xd8; q1 <<= 16; q2 <<= 16; q0 = 6; __MapActor_SetPos(q0, q1, q2); }
    b = __MapActor_GetActor(0x14);
    sh = 0xc0 << 6;
    *(unsigned short *)(b + 6) = sh;
    { PIN3; q1 = 0x9b; q2 = 0x9e; q1 <<= 17; q2 <<= 16; q0 = 0x14; __MapActor_SetPos(q0, q1, q2); }
    b = __MapActor_GetActor(0x13);
    *(unsigned short *)(b + 6) = sh;
    { PIN3; q1 = 0x92; q1 <<= 17; q2 = 0x9e; q2 <<= 16; q0 = 0x13; __MapActor_SetPos(q0, q1, q2); }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
    __Func_8092950(0x18, 7);
    __Func_8092b08(0x18, 1);
    b = __MapActor_GetActor(0x18);
    t3 = 0x3333;
    neg = 0xffff0000;
    *(int *)(b + 0x18) = t3;
    *(int *)(b + 0x1c) = neg;
    *(b + 0x55) = z;
    big = 0x98 << 17;
    *(int *)(b + 8) = big;
    hi = 0xc0 << 15;
    *(int *)(b + 0xc) = 0x80 << 10;
    *(int *)(b + 0x10) = hi;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x19), 0);
    __Func_8092950(0x19, 7);
    __Func_8092b08(0x19, 1);
    b = __MapActor_GetActor(0x19);
    *(int *)(b + 0x18) = t3;
    *(int *)(b + 0x1c) = neg;
    *(b + 0x55) = z;
    *(int *)(b + 8) = big;
    *(int *)(b + 0xc) = 0x88 << 14;
    *(int *)(b + 0x10) = hi;
    __StartTask((void *)OvlFunc_969_200b6d0, 0xc80);
    *(__Func_8093554() + 0x55) = z;
    __Func_80933f8(big, 0x80 << 14, 0xb4 << 16, 0);
    __WaitFrames(1);
    __Func_800fe9c();
    __WaitFrames(1);
    { PIN3; q0 = 0x80; q0 <<= 9; q1 = q0; q2 = q1; __Func_8012330(q0, q1, q2); }
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0xa);
    __MessageID(0x2809);
    OvlFunc_969_2008894(0x14);
    __Func_80925cc(0x13, 3);
    __CutsceneWait(0x14);
    __Func_8093040(0x13, 0, 0x28);
    __PlaySound(0x11);
    p = __MapActor_GetActor(0x14);
    *(int *)(p + 8) = 0x9a << 17;
    m = 0x98 << 16;
    k = 0xe0 << 13;
    *(int *)(p + 0xc) = k;
    *(int *)(p + 0x10) = m;
    __MapActor_SetAnim(0x14, 0xa);
    __CutsceneWait(0x14);
    *(int *)(p + 0xc) = k;
    *(int *)(p + 8) = 0x99 << 17;
    *(int *)(p + 0x10) = m;
    __MapActor_SetAnim(0x14, 0xb);
    __CutsceneWait(0xc);
    *(int *)(p + 8) = big;
    *(int *)(p + 0xc) = 0xa8 << 13;
    *(int *)(p + 0x10) = m;
    __MapActor_SetAnim(0x14, 0xc);
    __CutsceneWait(8);
    __DeleteFieldActor(0x14);
    p2 = __MapActor_GetActor(0x13);
    *(int *)(p2 + 0xc) = k;
    *(int *)(p2 + 8) = 0x93 << 17;
    *(int *)(p2 + 0x10) = m;
    __MapActor_SetAnim(0x13, 8);
    __CutsceneWait(0x14);
    *(int *)(p2 + 8) = 0x96 << 17;
    *(int *)(p2 + 0xc) = 0xd8 << 13;
    *(int *)(p2 + 0x10) = m;
    __MapActor_SetAnim(0x13, 9);
    __CutsceneWait(0xc);
    *(int *)(p2 + 8) = big;
    *(int *)(p2 + 0xc) = 0x88 << 13;
    *(int *)(p2 + 0x10) = m;
    __MapActor_SetAnim(0x13, 0xa);
    __CutsceneWait(8);
    __DeleteFieldActor(0x13);
    __CutsceneWait(0xa0);
}
