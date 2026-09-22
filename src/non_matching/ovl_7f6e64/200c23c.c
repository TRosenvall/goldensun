/* OvlFunc_969_200c23c -- NON-MATCHING, 616 encodings of 677, size 1664 against the
 * ROM's 1692 (-28), 665 instructions against 677.  648 instructions in the
 * reference.  EARLY -- two defect classes are named but neither is closed.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7f6e64/200c23c.c \
 *     asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_a_c_a.s \
 *     --func OvlFunc_969_200c23c
 * NOTE THE _a SUFFIX: its file-mate OvlFunc_969_200c8d8 LANDED in batch 281
 * (src/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_a_c_b.c) and the file was
 * split, so this function now lives alone in the _a half and needs NO FURTHER SPLIT
 * to land.
 *
 * DEFECT 1 -- `mov` ORDER INSIDE A PINNED ARGUMENT BLOCK FOLLOWS THE SHIFT ORDER,
 * AND THE ROM'S IS ALWAYS ASCENDING.  All eight __Func_8012330 sites plus the
 * __Func_80933f8 site emit `mov r0/r1/r2` ascending and THEN the shifts in a
 * per-site rotation.  The grouped form emits the movs IN THE SHIFT ORDER; a
 * `do { } while (0);` barrier between movs and shifts emits them FULLY REVERSED.
 * Neither reaches ascending-then-rotated.  Worth about 30 encodings across 9 sites.
 *
 * THIS IS THE OPEN HALF OF BATCH 280'S "put each mov adjacent to its own lsl" LEVER,
 * and batch 281's closed half is in
 * src/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_a_a_c_c_a.c -- SPLIT THE PINNED mov/lsl
 * PAIR IN THE SOURCE, which puts INSN_LUID(mov r0) between the mov and the lsl.
 * THAT LEVER WAS FOUND AFTER THIS CANDIDATE WAS WRITTEN AND HAS NOT BEEN TRIED HERE.
 * IT IS THE FIRST THING TO TRY.  `-fno-schedule-insns2` (an existing Makefile row
 * elsewhere) is the other obvious probe and was not run.
 *
 * DEFECT 2 -- ONE EXTRA CALLEE-SAVED REGISTER in the prologue: ours pushes
 * r5,r6,r7,r8,r9,r10 against the ROM's r5,r6,r8,r9,r10.  Pinning the two repeated
 * `0x4063ff` sites removed the CSE that caused it in the __Func_8091200 window and
 * that fix IS in this file; a second instance remains.  The per-block pointer split
 * that carried the file-mate (four separate pointers rather than one reused, because
 * local-alloc orders quantities SHORTEST-LIVED FIRST) was applied to the
 * 0x1a/0x1b/0x1c blocks and SHOULD BE EXTENDED TO THE SIX TAIL BLOCKS.
 *
 * THE END-POOL `int`-LOCAL LEVER IS THE WRONG DIRECTION HERE.  The ROM keeps TWO
 * mid-body `.pool_aligned` dumps with `b`-overs, so this function wants the early
 * dumps, not an end pool.  Read where the ROM's pools are before reaching for it.
 *
 * Two dot-prefixed globals it stores to, `.L6760` and `.L6764`, are handled with the
 * standard `extern int L6760 __asm__(".L6760");` idiom -- no .sym entry, no linker
 * change.
 *
 * No symbol tells found.  No per-file Makefile flag override applies to this stem.
 *
 * NEXT, in order: the split-mov/lsl lever from the landed file-mate's sibling; then
 * extend the per-block pointer split to the six tail blocks; then re-measure.  This
 * is an EARLY park with two named, untried handles -- not a plateau.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __Func_8012330(int a, int b, int c);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_8092adc(int slot, int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __WaitFrames(int n);
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void __Func_8092950(int a, int b);
extern void __Func_8092b08(int a, int b);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_969_200d688(unsigned char *a);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void OvlFunc_969_2008894(int a);
extern void __MapActor_Surprise(int slot, int a);
extern void __Func_809259c(int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_80921c4(int slot, int a, int b);
extern void __StopTask(void *fn);
extern void __StartTask(void *fn, int arg);
extern void OvlFunc_969_200b6d0(void);
extern void OvlFunc_969_200d6a0(void);
extern void __Func_808ba38(void);
extern void __SetDestMap(int a, int b);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern unsigned char gScript_969__0200e088[];
extern unsigned char gScript_969__0200e0d0[];
extern unsigned char gScript_969__0200e0f4[];
extern unsigned char gScript_969__0200e130[];
extern int L6760 __asm__(".L6760");
extern int L6764 __asm__(".L6764");
extern unsigned char *iwram_3001ebc;

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_969_200c23c(void)
{
    unsigned char *a;
    unsigned char *b0;
    unsigned char *b1;
    unsigned char *b2;
    unsigned char *c0;
    unsigned char *c1;
    unsigned char *c2;
    unsigned char *c3;
    unsigned char *c4;
    unsigned char *c5;
    unsigned char *s;
    int e, f, g;
    int m, hi, ww, d, hv;

    __PlaySound(0x13);
    __PlaySound(0x90 << 1);
    { PIN3; q0 = 0xc0; q1 = 0xc0; q2 = 0x80; q0 <<= 10; q1 <<= 10; q2 <<= 9;
      __Func_8012330(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 1; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 2; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 3; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x15; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 6; q1 <<= 1; q2 = 0xa; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 1; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 2; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 3; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q0 = 0x15; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q0 = 6; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q1 <<= 10; q2 <<= 9; q0 <<= 10;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q0 <<= 9; q1 <<= 9; q2 <<= 9;
      __Func_8012330(q0, q1, q2); }
    { PIN4; q0 = 0x98; q1 = 0x80; q2 = 0xb4; q3 = 1; q2 <<= 16; q1 <<= 14; q0 <<= 17;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    m = 0x1999;
    *(int *)(__MapActor_GetActor(0x18) + 0x18) = m;
    *(int *)(__MapActor_GetActor(0x19) + 0x18) = m;
    s = gScript_969__0200e088;
    __MapActor_SetBehavior(0x18, s);
    __MapActor_SetBehavior(0x19, s);
    __PlaySound(0x91);
    { PIN3; q0 = 0xc0; q1 = 0xc0; q2 = 0x80; q2 <<= 9; q0 <<= 11; q1 <<= 11;
      __Func_8012330(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0x4063ff; __Func_8091200(q0, q1); }
    __Func_8091254(0x10);
    __WaitFrames(0x14);
    { PIN2; q1 = 0; q0 = 0x7fff; __Func_8091200(q0, q1); }
    __Func_8091254(0x18);
    __WaitFrames(0x3c);
    __PlaySound(0x8d);
    __SetFlag(0x236);
    *(int *)(__MapActor_GetActor(0x18) + 0xc) = 0xffa00000;
    *(int *)(__MapActor_GetActor(0x19) + 0xc) = 0xffc00000;
    __Func_8092950(0x1a, 7);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1a), 0);
    b0 = __MapActor_GetActor(0x1a);
    d = 0xffff0000;
    *(int *)(b0 + 0x1c) = d;
    *(int *)(b0 + 0x18) = *(int *)(__MapActor_GetActor(0x18) + 0x18);
    b0[0x55] = 0;
    hi = 0x98 << 17;
    *(int *)(b0 + 8) = hi;
    *(int *)(b0 + 0xc) = 0xffe00000;
    ww = 0xc0 << 15;
    *(int *)(b0 + 0x10) = ww;
    __Func_8092950(0x1b, 7);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1b), 0);
    b1 = __MapActor_GetActor(0x1b);
    *(int *)(b1 + 0x1c) = d;
    *(int *)(b1 + 0x18) = *(int *)(__MapActor_GetActor(0x18) + 0x18);
    b1[0x55] = 0;
    *(int *)(b1 + 8) = hi;
    *(int *)(b1 + 0xc) = 0;
    *(int *)(b1 + 0x10) = ww;
    __Func_8092950(0x1c, 7);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1c), 0);
    b2 = __MapActor_GetActor(0x1c);
    *(int *)(b2 + 0x1c) = d;
    *(int *)(b2 + 0x18) = *(int *)(__MapActor_GetActor(0x18) + 0x18);
    b2[0x55] = 0;
    *(int *)(b2 + 8) = hi;
    *(int *)(b2 + 0xc) = 0x80 << 14;
    *(int *)(b2 + 0x10) = ww;
    e = 0x12;
    f = 0x17;
    __CopyMapTiles(0x66, 4, 0x4a, 4, e, f);
    e = 0x10;
    f = 0x15;
    __CopyMapTiles(0x27, 0x48, 0xb, 0x48, e, f);
    e = 0x16;
    f = 6;
    __Func_8010704(0x13, 6, 3, 7, e, f);
    g = 0xd;
    __Func_8010704(0x13, 6, 3, 7, g, f);
    __Func_8010704(0x13, 6, 3, 7, e, g);
    __Func_8010704(0x13, 6, 3, 7, g, g);
    __WaitFrames(1);
    d = 0xfff00000;
    a = __MapActor_GetActor(8);
    *(int *)(a + 8) = *(int *)(a + 8) + d;
    OvlFunc_969_200d688(a);
    a = __MapActor_GetActor(9);
    *(int *)(a + 8) = *(int *)(a + 8) + d;
    OvlFunc_969_200d688(a);
    a = __MapActor_GetActor(0xa);
    d = 0x80 << 13;
    *(int *)(a + 8) = *(int *)(a + 8) + d;
    OvlFunc_969_200d688(a);
    a = __MapActor_GetActor(0xb);
    *(int *)(a + 8) = *(int *)(a + 8) + d;
    OvlFunc_969_200d688(a);
    a = __MapActor_GetActor(0);
    *(int *)(a + 8) = *(int *)(a + 8) + d;
    *(int *)(a + 0x10) = *(int *)(a + 0x10) + d;
    OvlFunc_969_200d688(a);
    a = __MapActor_GetActor(1);
    *(int *)(a + 8) = *(int *)(a + 8) + d;
    *(int *)(a + 0x10) = *(int *)(a + 0x10) + d;
    OvlFunc_969_200d688(a);
    a = __MapActor_GetActor(2);
    *(int *)(a + 8) = *(int *)(a + 8) + d;
    *(int *)(a + 0x10) = *(int *)(a + 0x10) + d;
    OvlFunc_969_200d688(a);
    a = __MapActor_GetActor(3);
    *(int *)(a + 8) = *(int *)(a + 8) + d;
    *(int *)(a + 0x10) = *(int *)(a + 0x10) + d;
    OvlFunc_969_200d688(a);
    { PIN3; q1 = 0xc4; q2 = 0xdc; q2 <<= 16; q0 = 0x15; q1 <<= 16;
      __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetAnim(0x15, 5);
    { PIN3; q1 = 0xbc; q2 = 0x9e; q2 <<= 17; q0 = 6; q1 <<= 16;
      __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetAnim(6, 5);
    __Actor_SetSpriteFlags(__MapActor_GetActor(6), 0);
    { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q2 <<= 9; q0 <<= 11; q1 <<= 11;
      __Func_8012330(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0x4063ff; __Func_8091200(q0, q1); }
    __Func_8091254(0x78);
    s = gScript_969__0200e0d0;
    __MapActor_SetBehavior(0x18, s);
    __MapActor_SetBehavior(0x19, s);
    __MapActor_SetBehavior(0x1a, s);
    __MapActor_SetBehavior(0x1b, s);
    __MapActor_SetBehavior(0x1c, s);
    __WaitFrames(0x78);
    { PIN3; q0 = 0xc0; q1 = 0xc0; q2 = 0x80; q2 <<= 9; q0 <<= 10; q1 <<= 10;
      __Func_8012330(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0x203210; __Func_8091200(q0, q1); }
    __Func_8091254(0x78);
    __WaitFrames(0x78);
    { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q2 <<= 9; q0 <<= 10; q1 <<= 10;
      __Func_8012330(q0, q1, q2); }
    __Func_8091200(0x80 << 9, 0);
    __Func_8091254(0x78);
    __WaitFrames(0x78);
    { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q2 <<= 9; q1 <<= 9; q0 <<= 9;
      __Func_8012330(q0, q1, q2); }
    *(int *)(__MapActor_GetActor(0x18) + 0x1c) = 0x51e;
    s = gScript_969__0200e0f4;
    __MapActor_SetBehavior(0x19, s);
    __MapActor_SetBehavior(0x1a, s);
    __MapActor_SetBehavior(0x1b, s);
    __MapActor_RunScript(0x1c, s);
    __PlaySound(0x121);
    __Func_8092950(0x18, 0xf);
    __CutsceneWait(0x14);
    __MapActor_RunScript(0x18, gScript_969__0200e130);
    __StopTask((void *)OvlFunc_969_200b6d0);
    __MapActor_Jump(2, 2, 0x14);
    OvlFunc_969_2008894(2);
    { PIN3; q1 = 0xc0; q2 = 0x14; q0 = 1; q1 <<= 7; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0x81; q1 <<= 1; q0 = 1; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x14);
    __Func_809259c(1, 2);
    OvlFunc_969_2008894(1);
    __MapActor_SetSpeed(3, 0xcccc, 0x6666);
    { PIN3; q1 = 0xa3; q2 = 0xdc; q1 <<= 1; q0 = 3; __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN2; q1 = 0x81; q1 <<= 1; q0 = 3; __MapActor_Surprise(q0, q1); }
    OvlFunc_969_2008894(3);
    hv = 0xa0 << 8;
    c0 = __MapActor_GetActor(0);
    c0[0x62] = 0;
    c0[0x63] = 1;
    *(int *)(c0 + 0x4c) = *(int *)(c0 + 0xc);
    *(unsigned short *)(c0 + 6) = hv;
    c1 = __MapActor_GetActor(1);
    c1[0x62] = 0;
    c1[0x63] = 1;
    *(int *)(c1 + 0x4c) = *(int *)(c1 + 0xc);
    *(unsigned short *)(c1 + 6) = hv;
    c2 = __MapActor_GetActor(2);
    c2[0x62] = 0;
    c2[0x63] = 1;
    *(int *)(c2 + 0x4c) = *(int *)(c2 + 0xc);
    *(unsigned short *)(c2 + 6) = hv;
    c3 = __MapActor_GetActor(3);
    c3[0x62] = 0;
    c3[0x63] = 1;
    *(int *)(c3 + 0x4c) = *(int *)(c3 + 0xc);
    *(unsigned short *)(c3 + 6) = hv;
    c4 = __MapActor_GetActor(0x15);
    c4[0x62] = 0;
    c4[0x63] = 1;
    *(int *)(c4 + 0x4c) = *(int *)(c4 + 0xc);
    c5 = __MapActor_GetActor(6);
    c5[0x62] = 0;
    c5[0x63] = 1;
    *(int *)(c5 + 0x4c) = *(int *)(c5 + 0xc);
    *(__MapActor_GetActor(0x17) + 0x55) = 0;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x17), 0);
    __Func_8092950(0x17, 7);
    __Func_8092b08(0x17, 2);
    L6764 = 0;
    L6760 = 0xf0;
    { int tk; tk = 0xc80; __StartTask((void *)OvlFunc_969_200d6a0, tk); }
    do {
        __WaitFrames(1);
    } while (__GetFlag(0x237) == 0);
    __SetFlag(0x101);
    __CutsceneWait(0x1e);
    __SetFlag(0x8d << 1);
    __Func_808ba38();
    __SetDestMap(2, 0x5b);
    *(unsigned short *)(0xa0 << 19) = 0x7fff;
    {
        unsigned char *q;
        q = iwram_3001ebc;
        *(int *)(q + (0xe4 << 1)) = 1;
    }
    __MapTransitionOut();
    __WaitMapTransition();
}
