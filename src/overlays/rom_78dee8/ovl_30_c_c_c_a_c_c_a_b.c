// fakematch
/* OvlFunc_895_2008d1c  --  0x02008d1c
 * [asm/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_a.s, first of three functions]
 *
 * 241 instructions of straight-line cutscene: three actors are placed on the
 * party leader, walked out, made to jump and talk, then walked back and reset.
 * Byte-exact: 624 bytes, 244 encodings and 61 relocations identical.
 *
 * THE PROLOGUE PICKS THE CURE. The ROM's is `push {r5, lr}` -- exactly ONE
 * callee-saved register, and it holds &iwram_3001ebc for the two stores that
 * bracket the function. Nothing else may be kept, so every repeated constant
 * has to be rebuilt at its use, and that makes this a PIN function rather than
 * a named-local one. Plain C is 249 lines against 241, which is the
 * constant-CSE class reading as a length difference first.
 *
 * THE FIRST USE IS THE ONE THAT MATTERS. 0x9999 appears at FOUR sites: the
 * __Func_80933d4 call and three __MapActor_SetSpeed calls. Pinning only the
 * three SetSpeed sites still left 242 lines and 229 differing, with
 * `ldr r5, =0x9999` in the prologue and `push {r5, r6, lr}` -- the value was
 * being commoned out of the ONE site left unpinned, which displaced
 * iwram_3001ebc from r5 into r6 and shifted the whole function. Pinning that
 * first site took it to 241 lines and 3 differing in one step.
 *
 * This is the "an earlier USE serves as the definition" entry seen from the
 * other side: the earliest use is the definition, so it is the one that must
 * be broken. Pinning later sites while leaving the first one open buys nothing.
 *
 * THE NEGATED ARGUMENTS ARE PLAIN `-1` LITERALS. The ROM's
 * `mov r0,#1 / mov r1,#1 / mov r2,#1 / neg r2,r2 / mov r3,#0 / neg r0,r0 /
 * neg r1,r1` looks like it has to be transcribed as a mov-then-negate dance,
 * and written that way with the negs in the ROM's own order (q2, q0, q1) the
 * MOVS come out in that order instead -- 3 differing, since the movs are
 * slaved to the source's negate order and sched2 re-lands the negs itself.
 * Writing the negs ascending fixes it, but so does simply writing
 * `q0 = -1; q1 = -1; q2 = -1; q3 = 0;` -- both are byte-exact, and the literal
 * form is kept here because it says what the call means.
 *
 * TWO CONSTANTS ARE DERIVED FROM THE OFFSET REGISTER, and writing the stores
 * plainly gets both for free: 0x100 as `sub r2, #0xc0` and 0x204 as
 * `add r2, #0x44`, both off the 0x1c0 already in r2.
 *
 * No -O1 wildcard captures rom_78dee8; the one Makefile rule naming this
 * directory is an explicit rule for a different object.
 */
extern char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093054(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_895_2008d1c(void)
{
    unsigned char *a;

    __CutsceneStart();
    *(int *)(iwram_3001ebc + 0x1c0) = 0x100;
    __MapTransitionIn();
    { PIN2; q1 = 0; q0 = 0; __MapActor_SetAnim(q0, q1); }
    __CutsceneWait(4);
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN2; q0 = 0x9999; q1 = 0x1333; __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0x99; q1 = 1; q2 = 0x88; q0 <<= 19; q1 = -q1; q2 <<= 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(8, *(int *)(a + 8), *(int *)(a + 0x10));
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(5, *(int *)(a + 8), *(int *)(a + 0x10));
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(1, *(int *)(a + 8), *(int *)(a + 0x10));
    { PIN3; q0 = 8; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x4ccc; q0 = 1; q1 = 0x9999; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    __MapActor_SetAnim(5, 2);
    __MapActor_SetAnim(8, 2);
    { PIN3; q1 = 0x10; q0 = 1; q1 = -q1; q2 = 0; __Func_809228c(q0, q1, q2); }
    __Func_809228c(5, 0x10, 0);
    { PIN3; q2 = 0x20; q2 = -q2; q1 = 0; q0 = 8; __Func_809228c(q0, q1, q2); }
    __MapActor_WaitMovement(1);
    __MapActor_SetAnim(1, 0);
    __MapActor_SetAnim(5, 0);
    { PIN3; q1 = 0xc0; q0 = 1; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0; q1 <<= 8; q0 = 5; __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitMovement(8);
    { PIN2; q1 = 1; q0 = 8; __MapActor_SetAnim(q0, q1); }
    __CutsceneWait(0x28);
    { PIN2; q1 = 2; q0 = 8; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q1 = 0xc0; q0 = 8; q1 <<= 6; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 8; q1 <<= 7; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 8; q1 <<= 6; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0x14; q1 = 4; q0 = 8; __MapActor_Jump(q0, q1, q2); }
    __MessageID(0xfd3);
    { PIN2; q1 = 0; q0 = 0x4008; __Func_8093054(q0, q1); }
    __CutsceneWait(0x14);
    { PIN4; q0 = 0x99; q1 = 1; q2 = 0x94; q0 <<= 19; q1 = -q1; q2 <<= 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __MapActor_SetAnim(1, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(1, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_SetAnim(5, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(5, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_SetAnim(8, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(8, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    { PIN3; q1 = 0; q2 = 0; q0 = 5; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_WaitMovement(8);
    { PIN3; q2 = 0; q0 = 8; q1 = 0; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetAnim(1, 1);
    __MapActor_SetAnim(5, 1);
    { PIN2; q1 = 1; q0 = 8; __MapActor_SetAnim(q0, q1); }
    __SetFlag(0x802);
    *(int *)(iwram_3001ebc + 0x1c0) = 0x204;
    __ClearFlag(0x12f);
    __CutsceneEnd();
}
