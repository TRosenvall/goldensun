// fakematch
/* OvlFunc_939_2008c74  --  0x02008c74
 *
 * From goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_c_a_c_c.s, which held this
 * function alone, so no split was needed.
 *
 * Sets a flag, walks actor 0 off, plays two effect bursts and leaves the map.
 *
 * PARKED AT 2 OF 53 UNDER "THE `neg` INTERLEAVE", and the park called it a
 * family of at least two: same residue, same `f(0, 0, -8)` shape, different
 * callee in OvlFunc_936_2008504. That was correct -- 2008504 was elevated
 * earlier with the same fix, and this is the second of the pair.
 *
 *     rom   mov r2, #8 / mov r1, #0 / neg r2, r2 / mov r0, #0
 *     ours  mov r2, #8 / neg r2, r2 / mov r1, #0 / mov r0, #0
 *
 * Pinning r1 and r2 and assigning them in the ROM's order matches:
 *
 *     q2 = 8;  q1 = 0;  q2 = -q2;   __Func_80922c4(0, q1, q2);
 *
 * The interleaved argument is a ZERO and it still yields, because it sits
 * against a mov/neg pair on a distinct value -- the sub-case the discriminator
 * in src/non_matching/ovl_7ebdfc/2008120.c predicts will fall. r0 is left a
 * literal; pinning it as well is byte-identical and is not kept.
 *
 * THE PARK KEPT ITS MEASUREMENTS AND NOT ITS CODE, so the body below was
 * written fresh from the disassembly. That reconstruction screens at EXACTLY
 * the 2 of 53 the park recorded, which is the only evidence available that it
 * is the same candidate the park was describing -- and the reason to check
 * that number before trusting a rebuilt body. This is the second park in two
 * batches to lose its source; the batch-193 audit counted 123 of them.
 */

extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_80922c4(int a, int b, int c);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8091e9c(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);

void OvlFunc_939_2008c74(void)
{
    unsigned char *p;
    int e;
    int g;

    __SetFlag(0x242);
    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x3333, 0x1999);
    p = __MapActor_GetActor(0) + 0x55;
    *p = 0;
    __MapActor_SetAnim(0, 2);
    {
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 8;
        q1 = 0;
        q2 = -q2;
        __Func_80922c4(0, q1, q2);
    }
    __PlaySound(0x9e);
    e = 0x29;
    g = 4;
    __Func_80105d4(0x35, 4, 2, 2, e, g);
    __CutsceneWait(0xa);
    __Func_80105d4(0x35, 6, 2, 2, e, g);
    __CutsceneWait(0xa);
    __Func_8091e9c(1);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
}
