// fakematch
/* OvlFunc_883_200af14  --  0x0200af14
 *   [asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a.s, 4th of 4]
 *
 * 254 instructions of straight-line cutscene with two guarded actor fetches and
 * one if/else near the end.  Built at the tree default -O2: no Makefile pattern
 * rule matches rom_780898/ovl_30_c_c_c_a_a_a%, so `asm/%.o: src/%.c` applies and
 * a scratch-path screen sees the same flags as the real object.
 *
 * THE WHOLE PROBLEM IS COMMONED CONSTANTS, AND THE PROLOGUE NAMES THE CURE.
 * The ROM's prologue is `push {lr}` ALONE -- it spends no callee-saved register
 * at all, so every repeated constant is rebuilt at every use.  Plain C is
 * 265 lines against 258, 259 of 258 differing, with `push {r5, r6, lr}` plus
 * `mov r6, r8 / push {r6}`: cse_main commons 0x2e9, 0xd0<<8, 0xc0<<6 and
 * 0xa0<<7 into pseudos whose ranges straddle bl, the allocator gives them
 * r5/r6 and then reaches into r8.  It reads as a LENGTH difference first.
 *
 * NO FLAG REACHES IT, as recorded in docs/elevation.md.  -fno-gcse,
 * -fno-rerun-cse-after-loop and -fno-expensive-optimizations each produce
 * output byte-identical to the default (265 lines, 259 differing).
 *
 * NAMED LOCALS ARE THE WRONG CURE HERE.  One named local per duplicated value
 * (0x2e9, 0xd000, 0x3000, 0x5000, 0x1000, 0x102) assigned in the dominating
 * block is 260 lines and 251 differing -- it gives the values somewhere to live
 * instead of taking it away.  That lever belongs to a ROM that DOES keep a
 * callee-saved register; this one keeps none, so only pins work.
 *
 * EIGHTEEN PINS, MINIMAL BY MEASUREMENT.  Twenty pinned sites were stripped one
 * at a time; two were inert (__Func_8092adc(1, 0xd0<<8, 0x14) and the last
 * __Func_8092adc(1, 0xd0<<8, 0xa)), and removing both together still screens
 * exact.  A second round over the surviving 18 found every one load-bearing.
 * The dearest is the FIRST __Func_80921c4(0, 0x16f, 0x2e9): unpinning it alone
 * is 260 lines and 258 differing, because it is the earlier definition CSE
 * rewrites the second 0x2e9 into.  Unpinning the middle 0xd0<<8 site -- once
 * its two inert siblings are gone -- is 104 differing, which is that same
 * mechanism migrating to whichever site is left holding the value.
 *
 * WITHIN A PINNED SITE, WRITE THE STATEMENTS IN THE ROM'S EMITTED ORDER,
 * INCLUDING THE SHIFT.  The template ovl_30_c_c_a_c_a_c_b.c says to write the
 * movs in order and let sched2 re-land the shifts; that is true where the shift
 * is last, but where the ROM emits `lsl` BETWEEN two movs the shift has to sit
 * there in the source too.  Fourteen of the residual differences in the
 * first pinned draft were exactly this, each an adjacent transposition:
 * 28 differing -> 0.
 *
 * THE MESSAGE IDS ARE NOT BOTH LITERALS.  0x1c60 is `_MSG_1c60` in message.sym
 * and the reference object carries R_ARM_ABS32 _MSG_1c60 at +0x29c, so it must
 * be `(int)&_MSG_1c60`; spelled as the literal 0x1c60 the function is 259 lines
 * and 55 differing and the relocation is simply absent.  0x1c53 has no entry in
 * message.sym and no relocation in the reference, so it stays a bare literal.
 * The asymmetry is real and it is checkable only with objcmp -- tryc normalises
 * pool loads and cannot tell a symbol from a literal.
 *
 * The `iwram_3001ebc` offset build is the NON-interleaved variant of the shape
 * parked in src/non_matching/ovl_7e636c/2008df0.c site 3: this ROM loads the
 * pointer first and then builds 0xec<<1, which is what plain C emits.
 *
 * LANDING NEEDS A SPLIT.  The .s holds four functions and this is the last;
 * overlays/rom_780898/overlay.ld:63 names the single .o.
 */
extern unsigned char *iwram_3001ebc;
extern int _MSG_1c60;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);
extern int __Func_8091c7c(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_883_200af14(void)
{
    unsigned char *p;

    __CutsceneStart();
    __Func_80933f8(0x165 << 16, -1, 0x2e2 << 16, 1);
    { PIN3; q0 = 0; q1 = 0x16f; q2 = 0x2e9;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0, 0xa0 << 8, 0);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(1, *(int *)(p + 8), *(int *)(p + 0x10));
    { PIN3; q1 = 0xad; q0 = 1; q1 <<= 1; q2 = 0x2e9;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(1, 0xd0 << 8, 0x14);
    __MessageID(0x1c53);
    __ActorMessage(1, 0);
    __Func_80925cc(9, 2);
    { PIN3; q1 = 0x80; q0 = 9; q1 <<= 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 9; q1 <<= 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 9; q1 <<= 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 9; q1 <<= 6; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(9, 0, 0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(1, 3);
    __Func_80925cc(9, 1);
    { PIN3; q1 = 0xa0; q0 = 9; q1 <<= 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(9, 0, 0x14);
    __Func_809259c(1, 1);
    { PIN3; q0 = 1; q1 = 0x103; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(1, 0, 0xa);
    __MapActor_DoAnim(9, 3);
    __Func_8093040(9, 0, 0x14);
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x28; q0 = 1; q1 <<= 5;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(9, 4);
    __ActorMessage(9, 0);
    { PIN3; q1 = 0xb0; q0 = 0; q1 <<= 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q2 = 0xa; q0 = 1; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __Func_8093040(1, 0, 0xa);
    __MapActor_DoAnim(9, 3);
    __Func_8093040(9, 0, 0x14);
    { PIN3; q1 = 0x81; q0 = 0; q1 <<= 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x81; q0 = 1; q1 <<= 1; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(1, 0, 0x14);
    __Func_80925cc(9, 2);
    __Func_8093040(9, 0, 0x14);
    { PIN3; q1 = 0x80; q0 = 1; q1 <<= 5; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 1;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        PIN3; q0 = 1; q1 = 0x105; q2 = 0x3c;
        __MapActor_Emote(q0, q1, q2);
    } else {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    }
    __Func_8093040(1, 0, 0x14);
    __Func_8092adc(1, 0xd0 << 8, 0xa);
    __MessageID((int)&_MSG_1c60);
    __ActorMessage(1, 0);
    __MapActor_DoAnim(9, 3);
    __Func_8093040(9, 0, 0x14);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x14);
    { PIN3; q1 = 0x80; q2 = 0x14; q0 = 1; q1 <<= 5;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(1, 3);
    __MapActor_DoAnim(0, 3);
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __SetFlag(0xc1 << 2);
    __CutsceneEnd();
}
