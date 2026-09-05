// fakematch
/* OvlFunc_910_20085dc  --  0x020085dc
 *   [asm/overlays/rom_79dd90/ovl_30_c_c_c_c_a_c_a_b_c.s, the whole file]
 *
 * 293 instructions of cutscene: a long straight-line opening, a dialogue loop
 * with the test at the bottom, one flag-guarded block holding the two bitfield
 * writes, and a straight-line close.  Built at the tree default -O2 -- nothing
 * in the Makefile has a prefix that captures rom_79dd90, and objcmp against the
 * asm/ path and against a scratch copy of the reference give the same answer,
 * so no pattern rule is in play.
 *
 * THE PROLOGUE IS `push {lr}` ALONE, so the ROM spends no callee-saved register
 * and every repeated constant is rebuilt at every use.  ONLY PINS WORK: plain C
 * is 297 lines against 296 with `push {r5, r6, lr}` plus an r8 spill, and 237
 * of 296 encodings differ, because gcse commons -1, 0xa0<<17, 0x3c, 0x80<<1,
 * 0xa0<<7, 0xc0<<6, 0xa4<<1, 0xcccc/0x6666 and 0x80<<9/0x80<<8 into pseudos
 * whose ranges straddle a `bl`.  Same diagnosis as the sibling
 * OvlFunc_910_20081e4 in ovl_30_c_c_c_c_a_a.c, whose notes this file follows.
 *
 * TWENTY-SEVEN PINNED SITES, MINIMAL BY MEASUREMENT.  Forty-nine sites were
 * pinned first; twenty-two are inert one at a time, all twenty-two came out
 * together in a greedy pass with a re-test after every drop, and a second
 * one-at-a-time sweep over the twenty-seven survivors finds NONE still inert --
 * that is the fixpoint.  The survivors are the FIRST use of each commoned
 * value: dropping the first `__Func_80933f8` costs 291 of 294, the first
 * `__MapActor_SetSpeed(0xb, 0xcccc, 0x6666)` costs 64 of 299, the first
 * `__Func_8092adc(0xb, 0xc0<<6, 0xa)` costs 59; the later sites of the same
 * values are the ones that dropped out, exactly as "one pin at the first use
 * covers the later ones" predicts.  An r1/r2 pin around the `&= 0xfe` site --
 * the shape the sibling needed -- is byte-identical here and is not carried.
 *
 * EVERY FILL IS THE UNIFORM ONE-STATEMENT FORM EXCEPT ONE.  Writing every
 * pinned fill as one statement per argument in ascending q0..q3 reproduced all
 * of the ROM's emitted orders -- `mov r1 / mov r2 / mov r0 / lsl r1`,
 * `mov r1 / lsl r1 / mov r2 / mov r0`, seeds-then-shifts -- with a residue of
 * exactly two differing lines.  The residue is `__Func_8092c40(0xc, 0)` before
 * the loop, which genuinely wants the DESCENDING fill `q1 = 0; q0 = 0xc;`; the
 * second `__Func_8092c40(0xc, 0)` inside the loop is ascending and needs no pin
 * at all.  Same shape as OvlFunc_966_2008218's single descending survivor.
 *
 * THE LOOP IS A PLAIN `while`, AND THAT IS NOT THE USUAL ANSWER.  The recorded
 * rule is that a ROM loop with the test at the top is not a `while`; this one
 * has the test at the BOTTOM (`b .L73a / .L70e: body / .L73a: test / bne`),
 * which is exactly what `expand_end_loop` rotates a `while` into.  The
 * do/while(1)-with-break spelling that cures the other shape is WORSE here, 22
 * differing.  `for (;;)` with a break and a goto-back form both tie.
 *
 * 0x1720 IS A message.sym SYMBOL AND 0x1724 / 0x1726 / 0x84a ARE NOT, settled
 * by relocation and not by line count.  The reference object carries exactly
 * two R_ARM_ABS32 records, `_MSG_1720` at +0x2ec and `iwram_3001ebc` at +0x308;
 * spelling the first message id as the literal 0x1720 drops that relocation and
 * costs 4 bytes and 237 encodings.  The disassembly's own `ldr r0, =_MSG_1720`
 * against `ldr r0, =0x1724` agrees, and objcmp is what proves it.
 *
 * `iwram_3001ebc` IS A SCALAR POINTER, NOT AN ARRAY.  `extern unsigned char
 * iwram_3001ebc[]` folds the store's base into `=sym+0x1c0` and comes out four
 * instructions short, 14 differing.  The word stored is 0x201 and the ROM
 * builds it as `add r2, #0x41` off the index 0xe0<<1 that is already in r2 --
 * that falls out of the plain store and needs no help.
 *
 * THE TWO BITFIELD WRITES WANT DIFFERENT SPELLINGS, as they do in the sibling.
 * `&= 0xfe` already puts the CONSTANT in the `and` destination with no local
 * and no named pointer (naming the pointer ties; flipping the operands to
 * `0xfe & *p` is 301 lines and 81 differing).  `|= 1` does not: bare
 * `__MapActor_GetActor(0xc)[0x5a] |= 1` is 2 differing, and so is an `int`
 * local -- what the ROM wants is the QImode `unsigned char one = 1`, which
 * survives as a distinct pseudo so gcc ties `orr` to the constant.  With the
 * narrow local in hand, `one | p[0x5a]` and `p[0x5a] |= one` are identical.
 *
 * LANDING NEEDS NO SPLIT: the .s holds this one function and no data, and
 * overlays/rom_79dd90/overlay.ld:32 already names the single .o.
 */
extern unsigned char *iwram_3001ebc;
extern int _MSG_1720;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_WaitMovement(int slot);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MessageID(int id);
extern void __Func_800fe9c(void);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int n);
extern void OvlFunc_910_20088e8(void);
extern void OvlFunc_910_20088e8(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_910_20085dc(void)
{
    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN4; q0 = 0xa0 << 17; q1 = -1; q2 = 0xa0 << 17; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    { PIN3; q0 = 0; q1 = 0xa0 << 17; q2 = 0xba << 17;
      __MapActor_SetPos(q0, q1, q2); }
    __MapTransitionIn();
    __Func_80933d4(0x3333, 0x666);
    __Func_80933f8(0xa0 << 17, -1, 0x91 << 17, 1);
    { PIN3; q0 = 0; q1 = 0x9999; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xa0 << 1; q2 = 0x9b << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0xb, 2);
    { PIN3; q0 = 0xb; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __MessageID((int)&_MSG_1720);
    __Func_8093040(0xb, 0, 0xa);
    { PIN3; q0 = 0xc; q1 = 0xa0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0xc, 2);
    { PIN3; q0 = 0xc; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0xc, 0, 0x14);
    { PIN3; q0 = 0xb; q1 = 0x80 << 5; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xe0 << 7; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xa0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xb, 1);
    __Func_8093040(0xb, 0, 0xa);
    __Func_80925cc(0xc, 1);
    { PIN2; q1 = 0; q0 = 0xc;
      __Func_8092c40(q0, q1); }
    { PIN3; q0 = 0; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    while (__Func_8091c7c(0, 0) != 0) {
        __MapActor_Emote(0xc, 0x80 << 1, 0x3c);
        __MessageID(0x1724);
        __Func_8093040(0xc, 0, 0xa);
        __Func_809259c(0xc, 2);
        __Func_8092c40(0xc, 0);
    }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xb; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xb, 3);
    __MapActor_DoAnim(0xc, 3);
    __CutsceneWait(0x14);
    __Func_80925cc(0xb, 1);
    __MessageID(0x1726);
    __Func_8093040(0xb, 0, 0xa);
    { PIN3; q0 = 0xb; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x9d << 1; q2 = 0x8c << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0xb, 0, 0x28);
    { PIN2; q0 = 0; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    if (__GetFlag(0x84a) == 0) {
        __MapActor_SetSpeed(0xc, 0x80 << 9, 0x80 << 8);
        __MapActor_GetActor(0xc)[0x5a] &= 0xfe;
        __Func_80921c4(0xc, 0xad << 1, 0x107);
        __CutsceneWait(1);
        { unsigned char *p = __MapActor_GetActor(0xc);
          unsigned char one = 1;
          p[0x5a] = one | p[0x5a]; }
    }
    { PIN3; q0 = 0xb; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xa4 << 1; q2 = 0x83 << 1;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xa4 << 1; q2 = 0x8b << 1;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(0xb, 1);
    OvlFunc_910_20088e8();
    __CutsceneWait(0x28);
    { PIN3; q0 = 0; q1 = 0xa4 << 1; q2 = 0xf2;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xa4 << 1; q2 = 0xf2;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(0xb, 0, 0);
    __MapActor_WaitMovement(0);
    __MapActor_SetPos(0, 0, 0);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x201;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0xa);
    __CutsceneEnd();
}
