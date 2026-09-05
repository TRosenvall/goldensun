// fakematch
/* OvlFunc_955_20096d4  --  0x020096d4
 *
 * Cut out of goldensun/asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c.s.
 * 179 instructions.  Same cutscene frame as OvlFunc_955_20092f0
 * (ovl_30_c_c_c_c_b.c) in this overlay: signed-halfword read of gState +
 * 0xe1*2, early return through OvlFunc_common1_2c4 when it is 2, otherwise a
 * three-way dispatch on OvlFunc_common1_4cc(param, 4).
 *
 * THE BLOCKER WAS CONSTANT CSE ACROSS CALLS, and the whole `r == 0` arm is ONE
 * BASIC BLOCK -- gcc does not end a block at a `bl` -- so every lever in it is
 * global.  Written plainly the function is 192 lines against 184 with r8-r11
 * live: `-1` (seven uses), `0x80 << 9`, `0x80 << 8`, `0x80 << 7` and
 * `0xc0 << 8` (two or three uses each) are commoned into callee-saved
 * registers.  188 of 192 differ.
 *
 * NEITHER DOCUMENTED CURE APPLIED AS WRITTEN.  The reference-count cure fails
 * outright: giving each of the five __Func_80933f8 sites its OWN named local
 * per argument is BYTE-IDENTICAL to the bare-literal version (192 lines, 188
 * differing), because cse_main commons the five `-1` pseudos back together
 * before REG_N_REFS is ever consulted.  A constant shared across sites cannot
 * be split by naming it more times.  Only pinning the call-clobbered argument
 * register works, and it is needed at TWELVE sites.
 *
 * THE FIND, AND IT REPLACES THE BARRIER: HOW A PINNED FILL IS SPELLED DECIDES
 * THE ORDER OF ITS SEED MOVS, AND THE ONE-STATEMENT SPELLING IS THE RIGHT ONE.
 *
 *     q0 = 0x86; q1 = 1; q2 = 0xf0; q2 <<= 16; q3 = 1; q1 = -q1; q0 <<= 18;
 *
 * -- the batch-212 spelling, one statement per machine instruction -- emits the
 * seed movs SORTED BY THE POSITION OF EACH SEED'S FIRST CONSUMER (here r2, r1,
 * r0), never in register order, so it can never produce this ROM's
 * `mov r0 / mov r1 / mov r2`.  That is the two-state trap, and the neighbouring
 * ovl_30_c_c_c_c_a_b.c cures it with a volatile-asm barrier.  Nine barriers do
 * reach 5 of 184 here -- but they also relocate the `mov r2, r5` copy at two
 * other sites, because the barriers reschedule the whole block.
 *
 *     q0 = 0x86 << 18; q1 = -1; q2 = 0xf0 << 16; q3 = 1;
 *
 * needs no barrier at all.  Each large constant is split into mov+shift AFTER
 * expand, so every seed mov has a dependent and sits at depth 2 while every
 * shift, neg and small mov sits at depth 1.  The scheduler takes the whole
 * depth-2 class first, ties broken by insn order -- which is ARGUMENT ORDER --
 * and the ROM's `mov r0 / mov r1 / mov r2 / <ops>` falls out for free.  ALL
 * FIVE __Func_80933f8 sites, both __Func_80933d4 sites and all three
 * __MapActor_SetSpeed sites match on this spelling with nothing else.  Prefer
 * it to a barrier: it is shorter, it is checkable, and it does not perturb the
 * rest of the block.
 *
 * WHERE THE SEEDS TIE, STATEMENT ORDER IS THE LEVER.  `q0 = 0` is one
 * instruction, so it is depth-1 and orders among the ops by insn order.  The
 * first __Func_8092adc wants `mov r0` LAST and needs `q0 = 0;` written last;
 * the second wants it second and needs `q0 = 0;` written first.  Same callee,
 * opposite spellings, 2 differing between them.
 *
 * THE INVERSE RULE HOLDS FOR 0x84 << 1, AND IT IS FRAGILE.  That value is used
 * at two sites across three calls and the ROM DOES keep it in r5, split as
 * `mov r5, #0x84` ... three calls ... `lsl r5, #1`.  Left a bare literal at
 * both sites CSE hoists it with exactly that placement.  Every attempt to touch
 * it costs: naming it in the dominating block 32 differing, pinning r2 at
 * either use 32, and merely putting a volatile-asm barrier NEXT TO one of the
 * uses 34 -- the barrier stops the two uses being recognised as one expression
 * and the value is rematerialised at both.
 *
 * PROTOTYPE PRESENCE IS A PER-SITE LEVER HERE TOO, in both directions, exactly
 * as ovl_30_c_c_c_c_b.c and rom_7db0c8/ovl_30_c_c_c_c_a_b.c record.
 * OvlFunc_common1_1078, _15b8 and _5e4 are called through C's implicit
 * `int f()` and are deliberately undeclared below -- giving them prototypes
 * costs 7 differing.  __Func_80921c4 must be DECLARED -- removing its prototype
 * costs 3.  DO NOT "TIDY" EITHER.
 *
 * TWELVE PINS, MINIMISED BY MEASUREMENT: thirteen sites were pinned to reach
 * exact, then each was stripped individually.  Only the last __Func_80933f8
 * (the `0xa0 << 16` one before the closing __ActorMessage) was inert; it is
 * plain below.  A second round over the remaining twelve found none inert --
 * the cheapest strip still costs 2 differing, the dearest 186.
 *
 * gState's element type is inert here, unusually: `extern short gState[]` with
 * `gState[0xe1]` is byte-identical to the `unsigned char []` byte-offset form,
 * because thumb `ldrsh` has no immediate-offset encoding and the base must be
 * materialised either way.  The byte form is kept to match the sibling.
 */
extern unsigned char gState[];
extern void OvlFunc_common1_2c4(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int OvlFunc_common1_4cc(int a, int b);
extern void __MessageID(int id);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_8093fa0(void);
extern void __ActorMessage(int a, int b);
extern void __MapActor_SetSpeed(int a, int b, int c);
extern void __MapActor_SetAnim(int a, int b);
extern void __MapActor_WaitMovement(int a);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_TravelTo(unsigned char *e, int x, int y, int z);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092708(int a, int b, int c);
extern void OvlFunc_common1_1254(int a);
extern void __SetCameraTarget(int a, int b);
extern void OvlFunc_common1_588(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_955_20096d4(int a)
{
    unsigned char *gp;
    unsigned char *e;
    int r;

    gp = gState;
    if (*(short *)(gp + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
        return;
    }
    __CutsceneStart();
    r = OvlFunc_common1_4cc(a, 4);
    if (r == 0) {
        __MessageID(0x20aa);
        __Func_80933d4(0xc0 << 10, 0xc0 << 7);
        { PIN4; q0 = 0x86 << 18; q1 = -1; q2 = 0xf0 << 16; q3 = 1;
          __Func_80933f8(q0, q1, q2, q3); }
        __Func_8093530();
        __CutsceneWait(0x2d);
        { PIN2; q0 = 0x80 << 9; q1 = 0x80 << 6; __Func_80933d4(q0, q1); }
        { PIN4; q0 = 0x86 << 18; q1 = -1; q2 = 0xc0 << 16; q3 = 1;
          __Func_80933f8(q0, q1, q2, q3); }
        __Func_8093530();
        __ActorMessage(a, 0);
        OvlFunc_common1_1078(0, 0x9e << 2, 0x84 << 1);
        { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN2; q0 = 0; q1 = 0x9a << 2; __Func_80921c4(q0, q1, 0x84 << 1); }
        { PIN3; q1 = 0xc0 << 8; q2 = 0x14; q0 = 0; __Func_8092adc(q0, q1, q2); }
        __Func_8093fa0();
        { PIN2; q0 = 0x80 << 7; q1 = 0x80 << 4; __Func_80933d4(q0, q1); }
        { PIN4; q0 = 0x86 << 18; q1 = -1; q2 = 0xa0 << 16; q3 = 1;
          __Func_80933f8(q0, q1, q2, q3); }
        { PIN3; q0 = 0; q1 = 0x80 << 8; q2 = 0x80 << 7;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetAnim(0, 0xa);
        e = __MapActor_GetActor(0);
        __Actor_TravelTo(e, *(int *)(e + 8), *(int *)(e + 0xc) + (0x80 << 15),
                         *(int *)(e + 0x10));
        __MapActor_WaitMovement(0);
        __Func_8093fa0();
        { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
          __Func_80933f8(q0, q1, q2, q3); }
        __ActorMessage(a, 0);
        { PIN3; q0 = 0; q1 = 0xc0 << 9; q2 = 0xc0 << 8;
          __MapActor_SetSpeed(q0, q1, q2); }
        OvlFunc_common1_15b8(0, 0xf4 << 1, 0xf8);
        { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        __Func_8092708(0, 6, 0);
        __Func_80933f8(0x86 << 18, -1, 0xa0 << 16, 1);
        __ActorMessage(a, 0);
        OvlFunc_common1_1254(0);
        __SetCameraTarget(0, 0);
        OvlFunc_common1_588(a, 4);
    } else if (r == 1) {
        __MessageID(0x20a9);
        __ActorMessage(a, 0);
    }
    OvlFunc_common1_5e4(r, a, 4);
    __CutsceneEnd();
}
