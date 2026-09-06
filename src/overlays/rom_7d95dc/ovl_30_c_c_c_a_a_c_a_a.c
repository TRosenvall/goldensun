/* Cluster OvlFunc_953_20091c4 + OvlFunc_953_2009298
 *   [asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_a_a.s, lines 9-425 -- the WHOLE
 *   file: it holds exactly these two functions and no data, so NO SPLIT IS
 *   NEEDED.  overlays/rom_7d95dc/overlay.ld:34 names the object once,
 *   `asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_a_a.o(.text)`, and that is the
 *   ONLY line in any .ld naming this full path -- the .data list at line 56
 *   names ovl_30_c_c_c_c_c.o and nothing else, and there is no .bss section in
 *   this overlay at all.  Landing is one line: asm/ -> src/.]
 *
 * EXACT, both functions, measured as single-function extracts (objcmp --func
 * cannot isolate one function inside a multi-function candidate):
 *
 *   OK OvlFunc_953_20091c4 -- 212 bytes, 79 encodings and 21 relocations identical
 *   OK OvlFunc_953_2009298 -- 884 bytes, 342 encodings and 94 relocations identical
 *
 * Both verdicts hold against the real asm/ path and against a scratch copy of
 * the same .s, and objcmp prints no `(built with: ...)` line -- adjust=set(),
 * the tree default -O2 -mthumb -mthumb-interwork -fcall-used-r4.  NO FLAG GROUP
 * IS NEEDED.  The only two Makefile rules mentioning rom_7d95dc name other
 * objects literally (ovl_30_c_c_c_a_a_a_c_a_c_c.o and
 * ovl_30_c_c_c_a_a_a_c_a_a_a.o), so the cross-dir `asm/%.o: src/%.c` rule
 * applies and no wildcard can capture a future sibling.
 *
 * ---------------------------------------------------------------- 20091c4 --
 *
 * 72 instructions.  A flag-guarded two-armed cutscene: `__GetFlag(0x8a4)`
 * decides between a short talk and the full area-exit sequence, and a second
 * `__GetFlag(0x8a3)` picks the destination.  ONE PIN of nine pinnable sites.
 *
 * THE FLAG RESULT IS A NAMED LOCAL BECAUSE IT IS STORED.  `mov r5, r0 /
 * cmp r5, #0` then, 20 instructions into the else arm, `strb r5, [r0]` with
 * r0 = `__Func_8093554() + 0x55`.  r5 is provably 0 inside that arm, so by the
 * recorded rule "a value that is provably constant inside its branch is NOT
 * evidence" a bare `0` would be the default reading -- but here the value
 * SURVIVES A CALL in a callee-saved register, which is the discriminator that
 * promotes it.  `f = __GetFlag(0x8a4); ... p[0x55] = f;` is exact.
 *
 * THE ONE PIN IS AN ORDERING PIN, NOT A CSE KILL.  Everything else is exact as
 * plain literals; `__Func_8092adc(0x11, 0xc0 << 6, 0x14)` alone comes out
 * `mov r1,#0xc0 / lsl r1,#6 / mov r0,#0x11 / mov r2,#0x14` against the ROM's
 * `mov r1,#0xc0 / mov r0,#0x11 / lsl r1,#6 / mov r2,#0x14` -- the shift and the
 * r0 seed swapped, 2 differing with SIZE and RELOCATIONS both silent, which is
 * the recorded signature of the ordering class.  Uniform whole-value ASCENDING
 * fill closes it; the same block filled DESCENDING is 2 differing again (and a
 * different pair of encodings), so the direction is load-bearing, not the
 * bracket.
 *
 * WHAT NEEDED NOTHING.  `__Func_80933f8(0x87 << 18, -1, 0xd0 << 16, 1)` as a
 * bare call reproduces the ROM's seeds-then-shifts-then-neg order exactly; the
 * two `__MessageID` ids are bare literals (the reference object carries no
 * R_ARM_ABS32 for them, so neither is a message.sym symbol); the iwram pair is
 * the plain `*(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;` idiom and gcc's
 * own `add r3, #0x40` / `sub r3, #0x38` derivation of 0x200 and 0x1c8 off the
 * 0x1c0 index falls out of it.
 *
 * PROTOTYPES, 4 OF 17 LOAD-BEARING: __Func_80933d4 (4), __Func_80933f8 (4),
 * __Func_8093530 (4), and __Func_8093554 -- whose return type is what makes
 * `[0x55]` a byte store.  The other thirteen are inert and stay declared
 * because a complete prototype list is this tree's convention.
 *
 * ---------------------------------------------------------------- 2009298 --
 *
 * 334 instructions of straight-line cutscene script, no branches at all --
 * 58 pinnable constant-argument calls, three writes through iwram_3001ebc, one
 * actor fetch whose three fields are written in place.
 *
 * READ THE PROLOGUE BY CONTENT.  `push {r5, r6, lr} / mov r6, r10 / mov r5, r9 /
 * push {r5, r6} / mov r6, r8 / push {r6}` -- FIVE callee-saved registers, and
 * what they hold is the whole diagnosis:
 *
 *     r5   0xffff0000, then later 0x80 << 9   TWO unrelated values, one slot
 *     r6   0xc0 << 6                          a repeated script constant
 *     r8   &iwram_3001ebc                     the symbol address, 3 reloads
 *     r9   0xe4 << 1                          a store offset
 *     r10  0xe0 << 1                          a store offset
 *
 * Not one is a loop variable or a live pointer, so this is a pin function.
 * Plain C is 133 differing of 334 and six instructions SHORT.
 *
 * TWENTY PINS OF FIFTY-EIGHT, MINIMAL BY MEASUREMENT.  All 58 were pinned
 * first (that also matches, at 0), then stripped greedily with a re-test after
 * every drop and re-grown with an ADD pass, to a fixpoint from BOTH ends.  A
 * final strict pass over the 20 survivors drops none: each costs between 2 and
 * 19 differing.  The survivors are not "the repeated constants" --
 * __MapActor_SetAnim(0x10, ...) appears eight times and needs no pin at any of
 * them, __CutsceneWait eleven times and needs none -- while
 * __Func_80921c4(0x10, 0xa4, 0xe6 << 2) and its 0xb9 twin need one each.
 *
 * THE SWEEP HAD TO RUN FROM BOTH ENDS, AND HERE IS THE PROOF.  The drop pass
 * alone stopped at sixteen pins and 47 differing; adding back site 22
 * (`__Func_80921c4(0x10, 0xb9, 0xe5 << 2)`) took it to 31 and site 51
 * (`__Func_80921c4(0x10, 0xa2, 0xe5 << 2)`) to 29.  Those two are the two ends
 * of ONE two-site CSE class on 0xe5 << 2, and the greedy pass had removed the
 * first-use pin as redundant while the second was still in the set.  The same
 * thing happened a second time on 0xe6 << 2 (sites 19 and 20) after site 28 was
 * forced in.  A drop-only sweep cannot find these.
 *
 * THREE LEVERS BEYOND THE PINS, AND NONE OF THEM IS FINDABLE ALONE:
 *
 *   1. `a1` (0xc0 << 6) MUST BE PINNED TO r6, not merely named.  The ROM builds
 *      it in r6 AND builds a second copy in r1 for
 *      __Func_8092adc(0x11, 0xc0 << 6, 0) at the very same point:
 *
 *        mov r6,#0xc0 / mov r1,#0xc0 / mov r2,#0 / lsl r6,#6 / mov r0,#0x11 /
 *        lsl r1,#6 / bl __Func_8092adc / mov r1, r6
 *
 *      A plain `int a1` is CSE'd into the pinned argument (`mov r1, r5`) and
 *      the whole allocation shifts: 16 differing.  `register int a1
 *      __asm__("r6")` keeps both builds.  Moving the assignment after the call,
 *      or dropping `a1` for literals, are both inert at 29 differing -- the
 *      placement is not the lever, the hard register is.
 *
 *   2. r5's TWO ROLES MUST BE TWO LOCALS.  0xffff0000 and 0x80 << 9 share r5 in
 *      the ROM, and writing them as one recycled `int v` is 40 differing and
 *      six instructions short.  Two locals `v` and `w` is 0.  This is the
 *      template's recorded "one variable for r5's four roles (the merge lever)"
 *      result reproduced from the other side: the ROM sharing a register is the
 *      ALLOCATOR, not the source.
 *
 *   3. THE HALFWORD STORE WANTS THE TYPED FIELD AND NOTHING ELSE.
 *      `((struct Actor *)p)->f6 = 0xd0 << 8;` is exact.  The ROM builds 0xd000
 *      in r3, a SCRATCH register, which is the template's discriminator for the
 *      field over a named local, and 0xd000 >= 0x8000 makes a bare literal
 *      through an untyped `unsigned short *` pool.  With the pin set right it
 *      needs no register pin of its own -- an r3 pin on it is byte-identical,
 *      so the plain field ships.
 *
 * THE THREE ARE ENTANGLED AND THE MIDDLE ONE IS A PRECONDITION.  With the merge
 * lever in place (one `v`), the correct 20-pin set measures 40 differing, and
 * the natural conclusion is that the pins are wrong.  They are not: splitting
 * `v` takes the SAME pin set from 40 to 0.  Nothing here is diagnosable one
 * lever at a time, which is the file-sibling's lesson (rom_7db0c8's deliberate
 * pin-set hole) in its third form.
 *
 * THE PRESSURE IS THE POINT (NEW).  Killing a CSE is not free even when the ROM
 * does not perform it.  At an intermediate stage the only residue was a
 * two-site CSE on 0xd0 << 8 (site 28 and the halfword store) worth 12 of 334 --
 * and pinning site 28 to break it cost THIRTY, because removing the pseudo drops
 * the register pressure, `&iwram_3001ebc` moves out of r8 into a LOW register,
 * and the ROM's `mov r3, r8 / ldr r2, [r3]` pair at each of the three reload
 * sites collapses to a single `ldr r2, [r6]`.  The function then comes out SIX
 * INSTRUCTIONS SHORT -- a CSE kill made the output shorter, which reads exactly
 * backwards.  The cure was to restore the pressure honestly, by splitting `v`
 * into two locals so that five real values compete for the same five registers.
 * Diagnostic to keep: when a pin makes the function SHORTER, the pin is not
 * wrong -- something else that should be live is missing.
 *
 * WHAT NEEDED NOTHING.  The eleven __MapActor_SetAnim calls at slots 8..0x12 as
 * bare literals (including the descending-looking `mov r1,#0 / mov r0,#0x12` at
 * the last one, which falls out of gcc's own ordering); `p = __MapActor_GetActor
 * (0x10)` as a plain named pointer with three stores through it; the three
 * iwram blocks as the plain `*(int *)(iwram_3001ebc + (0xe0 << 1))` idiom --
 * naming the address as `unsigned char **pp` is WORSE here (36 differing) even
 * though the recorded "name the global's address" lever says otherwise, because
 * gcc already commons it; and `0xffff0000` as a plain `int` initialiser.
 *
 * UNIFORM ASCENDING FILL AT ALL TWENTY.  One statement per argument, ascending
 * q0..q3, whole value per statement.  It reproduces every emitted order the ROM
 * has here -- the `neg` triple at __Func_80933f8, `mov r2 / mov r0 / mov r1 /
 * lsl r2` at __Func_80921c4, `mov r1 / mov r0 / lsl r1 / mov r2` at
 * __Func_8092adc, `mov r1 / mov r2 / mov r0 / lsl r1` at __MapActor_SetSpeed.
 * No site wants descending.
 *
 * PROTOTYPES, 7 OF 23 LOAD-BEARING: dropping __MapActor_SetIdle costs 124,
 * __MapActor_SetAnim 32, __Func_8092adc 22, __Func_80921c4 19,
 * __MapActor_Surprise 6, __MapActor_SetSpeed 5, __MapActor_SetPos 3.  The other
 * sixteen are inert.
 *
 * MEASURED WORSE (against 342 encodings / 884 bytes):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   no pins at all                                          133 (-6 insns)
 *   no pins, one `v`, no a1 pin (the plain reading)          137
 *   correct pins, one recycled `v`                            40 (-6 insns)
 *   correct pins, `a1` a plain `int` local                    16
 *   correct pins, `unsigned char **pp` for the iwram base     36
 *   sixteen pins (drop-only greedy fixpoint, pre-lever shape)     47
 *   dropping the pin at site 22 / 36 / 27                 18 / 19 / 18
 *   dropping any other survivor                            2 to 16
 *
 *   INERT (tie at 0, so the plain form ships):
 *     all 58 sites pinned instead of the minimal 20
 *     an r3 register pin on the halfword store
 *
 * FLAGS DO NOT REACH ANY OF IT: with the final pin set, -fno-gcse,
 * -fno-rerun-cse-after-loop and -fno-cse-follow-jumps are byte-identical;
 * -fno-expensive-optimizations is 45 differing and -fno-schedule-insns2 is 179,
 * so the sweep is live.  This is local CSE, exactly as the recorded
 * "A constant hoisted across a call" entry says.
 */
struct Actor { unsigned char pad00[6]; unsigned short f6; };

extern unsigned char *iwram_3001ebc;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern unsigned char *__Func_8093554(void);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_8091e9c(int n);
extern void OvlFunc_953_2009c48(int slot);
extern unsigned char gScript_953__0200af88[];
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_Surprise(int a, int b);
extern void __MapTransitionIn(void);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void OvlFunc_953_2009c5c(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")


void OvlFunc_953_20091c4(void)
{
    int f;

    __CutsceneStart();
    f = __GetFlag(0x8a4);
    if (f != 0) {
        __Func_8092848(0x11, 0, 0x28);
        __MessageID(0x206f);
        OvlFunc_953_2009c48(0x11);
        { PIN3; q0 = 0x11; q1 = 0xc0 << 6; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
    } else {
        __Func_809259c(0x11, 2);
        __MessageID(0x206d);
        __ActorMessage(0x11, 0);
        __Func_8093554()[0x55] = f;
        __WaitFrames(1);
        __Func_80933d4(0x66666, 0xcccc);
        __Func_80933f8(0x87 << 18, -1, 0xd0 << 16, 1);
        __Func_8093530();
        *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
        *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x20;
        __MapTransitionOut();
        __WaitMapTransition();
        if (__GetFlag(0x8a3) != 0)
            __Func_8091e9c(0x46);
        else
            __Func_8091e9c(7);
    }
    __CutsceneEnd();
}

void OvlFunc_953_2009298(void)
{
    unsigned char *p;
    int v;
    int w;
    register int a1 __asm__("r6");

    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __PlaySound(0xf7);
    __MapActor_SetAnim(8, 2);
    __MapActor_SetAnim(9, 2);
    __MapActor_SetAnim(0xa, 2);
    __MapActor_SetAnim(0xb, 2);
    __MapActor_SetAnim(0xc, 2);
    __MapActor_SetAnim(0xd, 2);
    __MapActor_SetAnim(0xe, 0);
    __MapActor_SetAnim(0xf, 0);
    __MapActor_SetAnim(0x10, 0);
    __MapActor_SetAnim(0x11, 0);
    __MapActor_SetAnim(0x12, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x15), 0);
    v = 0xffff0000;
    *(int *)(__MapActor_GetActor(0x13) + 0x18) = v;
    *(int *)(__MapActor_GetActor(0x14) + 0x18) = v;
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    __WaitFrames(1);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x200;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x20;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __Func_80925cc(0x10, 1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x10; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0x10, 0xa4, 0xe2 << 2);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x10, 9);
    __CutsceneWait(0x28);
    __MapActor_SetAnim(0x10, 0xa);
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(0x10, 1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x10; q1 = 0xa4; q2 = 0xe6 << 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0xb9; q2 = 0xe6 << 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0xb9; q2 = 0xe5 << 2;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(0x10, 0xb);
    __CutsceneWait(0x28);
    __Func_80925cc(0x10, 1);
    __CutsceneWait(0x3c);
    __Func_80925cc(0x10, 3);
    __CutsceneWait(0x28);
    __MapActor_SetBehavior(0x10, gScript_953__0200af88);
    __CutsceneWait(0x50);
    { PIN2; q0 = 0x10; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0xe; q1 = 0xd0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xf; q1 = 0xa0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x11, 0, 0);
    { PIN3; q0 = 0x12; q1 = 0x80 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0xe; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0xf; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x11; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __MapActor_Surprise(0x12, 0x81 << 1);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0xe; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xf; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    a1 = 0xc0 << 6;
    { PIN3; q0 = 0x11; q1 = 0xc0 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_953_2009c5c(0x12, a1);
    __MapActor_SetIdle(0x10);
    p = __MapActor_GetActor(0x10);
    ((struct Actor *)p)->f6 = 0xd0 << 8;
    w = 0x80 << 9;
    *(int *)(p + 0x18) = w;
    *(int *)(p + 0x1c) = w;
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x10, 0);
    __CutsceneWait(0x28);
    __MapActor_SetAnim(0x13, 5);
    __MapActor_SetAnim(0x14, 5);
    __CutsceneWait(0x3c);
    __Func_8092adc(0x10, a1, 0x14);
    __MapActor_SetAnim(0x10, 8);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0xe, 4);
    __MapActor_SetAnim(0xf, 4);
    __MapActor_SetAnim(0x11, 4);
    __MapActor_DoAnim(0x12, 4);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0x10, 4);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x10; q1 = 0x80 << 10; q2 = w;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0xa2; q2 = 0xe5 << 2;
      __Func_80921c4(q0, q1, q2); }
    __Func_80921c4(0x10, 0xa2, 0x37a);
    __MapActor_SetAnim(0x13, 1);
    __MapActor_SetAnim(0x14, 1);
    { PIN3; q0 = 0x10; q1 = 0xb8; q2 = 0x35f;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0xb8; q2 = 0xc7 << 2;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(0x10, 0, 0);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x201;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    __MapTransitionOut();
    __WaitMapTransition();
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x80 << 1;
    __Func_8091e9c(0x45);
    __CutsceneEnd();
}
