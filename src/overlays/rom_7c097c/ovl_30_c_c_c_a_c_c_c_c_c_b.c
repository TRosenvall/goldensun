/* OvlFunc_936_200a008  --  0x0200a008
 *   [asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_c_c_c.s, lines 7-687 of 1767]
 *
 * EXACT.  objcmp: "OK OvlFunc_936_200a008 -- 1720 bytes, 685 encodings and 162
 * relocations identical", against the asm/ path and against a scratch copy of
 * the reference alike, so no Makefile pattern rule is in play (the tree default
 * -O2 -fcall-used-r4; objcmp reports adjust=set()).
 *
 * 668 instructions of straight-line cutscene script: one null-guarded call, one
 * two-armed `if` around a save-counter bump, no loops.  Roughly 130 calls, most
 * of them two- or three-argument constant scripts.
 *
 * READ THE PROLOGUE BY CONTENT.  It is WIDE -- `push {r5, r6, r7, lr}` plus
 * `mov r7, r8 / push {r7}` plus `sub sp, #8` -- and that width says nothing on
 * its own.  What the four saved registers HOLD is the diagnosis:
 *
 *     r7   0xc0 << 6            a repeated script constant   -> named local a1
 *     r6   0xa0 << 7            a repeated script constant   -> named local a2
 *     r8   &iwram_3001ebc       a symbol address gcse commons across the body
 *     r5   0xfe / 1 / 2         a RECYCLED SCRATCH SLOT, four unrelated values
 *     sp   two words            the two stack arguments of __CopyMapTiles
 *
 * Not one of them is a loop variable or a live pointer, so this is a pin
 * function exactly like the `push {lr}` siblings in rom_79dd90 -- gcse commons
 * the repeated argument constants into pseudos whose ranges straddle a `bl`,
 * and only register pins destroy the CSE.  Plain C is 649 differing of 685 and
 * 36 bytes long.
 *
 * FORTY PINNED SITES OF 129 PINNABLE, MINIMAL BY MEASUREMENT.  All 129 were
 * pinned first; a greedy pass re-testing under objcmp after EVERY drop removed
 * 89, and a fixpoint pass over the 40 survivors removes NONE.  Every survivor
 * is load-bearing: dropping one costs between 2 and 656 differing.  The
 * expensive ones are the FIRST use of a commoned value -- site 1
 * (`__Func_80933f8(-1,-1,-1,0)`) costs 656, the first `__MapActor_SetSpeed`
 * costs 609, the first `__Func_8093040(0x4002,...)` costs 499 -- and the cheap
 * ones (2 differing) are buying argument ORDER, not CSE destruction, which is
 * why they survive next to an already-destroyed CSE.
 *
 * UNIFORM FILL IS THE ANSWER AT 39 OF THE 40 SITES.  One statement per argument,
 * ascending q0..q3, whole value per statement, reproduces every emitted order
 * the ROM has -- `mov r1 / mov r2 / mov r0 / lsl r1`, `mov r2 / mov r0 / mov r1
 * / lsl r2`, seeds-then-shifts, the `neg` triple at the head of the function.
 * Transcribing the ROM's emitted order buys nothing anywhere.  The ONE
 * exception is `__Func_8092c40(8, 0)`, which wants the DESCENDING fill
 * `q1 = 0; q0 = 8;` -- ascending is 2 differing.  That is the sixth function in
 * the corpus on which __Func_8092c40 has wanted descending, and it stays
 * binary: descending or nothing.
 *
 * THE `|= 1` PAIR NEEDS A CALLEE-SAVED REGISTER PIN, NOT A NARROW TYPE (NEW).
 * The ROM writes the flag bit into two actors around a call:
 *
 *     ldrb r3,[r0] / mov r5,#1 / orr r3,r5 / strb r3,[r0]     (slot 8)
 *     ldrb r3,[r0] / orr r5,r3 / strb r5,[r0]                 (slot 9)
 *
 * -- one materialisation of the 1, kept in r5 across `bl __MapActor_GetActor`,
 * with the VALUE as the `orr` destination at the first site and the CONSTANT at
 * the second.  Every plain spelling misses, and the two failure modes are
 * disjoint:
 *
 *   - any spelling that REASSIGNS the variable (`one |= *p`, `one = one | *p`,
 *     `one = *p | one`) gets the second site's operand order right and then
 *     const-propagates the 1 at the FIRST site, emitting `mov r2,#1` there and
 *     a second `mov r5,#1` after the call: 376 differing (and see the
 *     cancellation note below);
 *   - any spelling that does NOT reassign it (`*p = one | *p`, `*p = *p | one`,
 *     `*p |= one`, a fresh `e = one | *p`) keeps one pseudo and then ties `orr`
 *     to the loaded value at the second site: 19 differing.
 *
 * Eighteen combinations of the two sites were measured and they collapse to
 * exactly those two numbers -- the first site's spelling is inert, the second
 * site's is the switch.  The RECORDED lever for "the constant is rd" -- a
 * narrow `unsigned char` local -- is WORSE here, 386 differing, because it also
 * moves the `ldrb` destination; `unsigned short` behaves exactly like `int`.
 * What works is binding the value to the register the ROM uses:
 *
 *     { register int one __asm__("r5");
 *       one = 1; ...[0x5a] |= one; p = ...; one |= *p; *p = one; }
 *
 * With the pin in hand the spelling is inert again (aA, aB and aD all tie, and
 * `unsigned char` ties too), so the plain compound form ships.  r6 and r7 are
 * not substitutes -- r6 is 650 differing, r7 is 8.
 *
 * TWO PAIRS OF HALFWORD STORES TO THE SAME FIELD WANT OPPOSITE REMEDIES, and
 * they are four lines apart in the same function.  Both pairs store through
 * `(unsigned short *)(actor + 6)`; a bare literal pools in every case (157
 * differing), which is the HImode-literal rule.  The discriminator recorded in
 * "A narrow store of a literal" is confirmed here with an internal control:
 *
 *     pair 1   0xc0 << 6 / 0xa0 << 7   ROM: mov r7 / mov r6, CALLEE-SAVED
 *                                      -> named `int a1, a2`               OK
 *     pair 2   0 / 0x80 << 8           ROM: mov r3 (+lsl), SCRATCH
 *                                      -> typed field `->f6`               OK
 *                                      -> named int locals            5 differing
 *
 * The named locals are put in r5 by gcc, which is right for the pair the ROM
 * pushes and wrong for the pair it does not.  A named `unsigned short *q` plus
 * an int value ties with the typed field at 0; the field ships because it
 * spends no register.  `a1`/`a2` are then free to be read at the six
 * `__Func_8092adc` sites that use them -- byte-identical to spelling the
 * literal there, so that is naming, not a lever.
 *
 * THE SAVE-COUNTER BUMP IS AN r2/r3 EXCHANGE, AND IT IS REACHABLE (NEW).  The
 * `if (__Func_8091c7c(0,0) == 0)` has the SAME `+= 1` statement in both arms.
 * gcc reproduces the ROM exactly in the then arm and mirrors r2 and r3 in the
 * else arm:
 *
 *     ref   mov r3,r8 / ldr r2,[r3] / mov r3,#0xec / lsl r3,#1 / add r2,r2,r3
 *     ours  mov r2,r8 / ldr r3,[r2] / mov r2,#0xec / lsl r2,#1 / add r3,r3,r2
 *
 * ELEVEN source rewrites of the else arm -- named halfword pointer, named byte
 * pointer, named offset, pre-increment, `*w = *w + 1`, `w[0] = w[0] + 1`, an
 * extra cast, a `goto`-spelled branch, and a struct-typed global -- are ALL 8
 * differing.  That invariance says the lever is not a spelling.  Pinning the
 * ADDRESS to the register the ROM uses closes it:
 *
 *     { register unsigned short *a __asm__("r2");
 *       a = (unsigned short *)(iwram_3001ebc + (0xec << 1)); *a += 1; }
 *
 * Pinning the VALUE to r3 instead, or pinning both, tie at 0.  The SAME pin in
 * the then arm is INERT -- it ties at 0 with and without -- so it does not
 * ship, and the two arms are deliberately spelled differently.  This is the
 * recorded "r2/r3 exchange" four-member class reached by a register pin rather
 * than by the type lever that reached OvlFunc_946_200985c.
 *
 * A CANCELLING LENGTH DEFECT, WITH ITS MECHANISM (NEW).  At the 376-differing
 * stage the candidate was 1720 bytes against 1720 and 685 encodings against
 * 685 -- the length tell did not fire -- while 376 of 685 encodings differed.
 * The two extra `movs r5, #1` from the ORR defect were paid for exactly by two
 * `.short 0x0000` alignment slugs the ROM emits before its literal pools and
 * ours did not need.  So POOL ALIGNMENT PADDING is what absorbs a small odd
 * length change, and on a function with two mid-function pools the recorded
 * "our stream being shorter is a signature" can be silently cancelled.  The
 * cure is the recorded one: read the diff TEXT, not the count.
 *
 * WHAT NEEDED NOTHING.  The two `&= 0xfe` pairs are exact as bare
 * `__MapActor_GetActor(8)[0x5a] &= 0xfe;` -- no local, no named pointer; the
 * ROM's `mov r5,#0xfe / mov r3,r5 / and r3,r2` then `and r5,r3` falls out of
 * gcse on its own.  The two stack arguments of `__CopyMapTiles` need no named
 * local either, despite the ROM keeping the 2 in r5 across both calls.  0x1b05
 * and 0x4002 are plain literals, not symbols: the reference object carries
 * exactly two R_ARM_ABS32 records and both are `iwram_3001ebc`.
 * `iwram_3001ebc` is a scalar `unsigned char *`, and the ROM's `add r3, #0x41`
 * off the 0xe0<<1 index to build 0x201 falls out of the plain store.
 *
 * MEASURED WORSE (all against 685 encodings; aligned counts):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   no pins at all                                         649 (+36 bytes)
 *   all 129 pins, everything else plain                    423 (+8 bytes)
 *   one variable for r5's four roles (the merge lever)      430
 *   `int m1,o1,m2,o2` merged pairwise (orr into mask)       383
 *   ORR pair: any reassigning spelling, no r5 pin           376
 *   ORR pair: `unsigned char one`                           386
 *   ORR pair: r5 pin moved to r6                            650
 *   ORR pair: r5 pin moved to r7                              8
 *   halfword pair 2 as bare literals                        157
 *   halfword pair 2 as named int locals                       5
 *   else-arm bump, any of eleven rewrites                     8
 *   ascending fill at __Func_8092c40                          2
 *
 * LANDING NEEDS A SPLIT.  The .s holds TWO functions -- this one and
 * OvlFunc_936_200a6c0 at line 688 -- and no data.  overlays/rom_7c097c/
 * overlay.ld:51 names the single .o `asm/overlays/rom_7c097c/
 * ovl_30_c_c_c_a_c_c_c_c_c.o(.text)` (.text 0x0200a008, 0x117c), so the file
 * must be split before this function can land.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int a, int b);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093054(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int n);

struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
};

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")


void OvlFunc_936_200a008(void)
{
    unsigned char *p;
    int a1, a2;

    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __WaitFrames(1);
    { PIN3; q0 = 0; q1 = 0xc0 << 16; q2 = 0xab << 17;
      __MapActor_SetPos(q0, q1, q2); }
    __WaitFrames(1);
    __Func_80933d4(0x3333, 0x666);
    __Func_80933f8(0xc0 << 16, -1, 0xfc << 16, 1);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x28;
    __MapTransitionIn();
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0; q2 = 0x8b << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    a1 = 0xc0 << 6;
    *(unsigned short *)(__MapActor_GetActor(8) + 6) = a1;
    a2 = 0xa0 << 7;
    *(unsigned short *)(__MapActor_GetActor(9) + 6) = a2;
    __WaitFrames(1);
    __MapActor_GetActor(8)[0x5a] &= 0xfe;
    __MapActor_GetActor(9)[0x5a] &= 0xfe;
    __Func_809218c(8, 0xb8, 0xe8);
    __Func_80921c4(9, 0xc6, 0xe8);
    __MapActor_SetAnim(8, 1);
    __CutsceneWait(0x14);
    { register int one __asm__("r5");
      one = 1;
      __MapActor_GetActor(8)[0x5a] |= one;
      p = __MapActor_GetActor(9) + 0x5a;
      one |= *p;
      *p = one;
    }
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 4);
    __MessageID(0x1b05);
    __Func_8093040(8, 0, 0xa);
    __Func_8092adc(9, a2, 0xa);
    __MapActor_DoAnim(9, 3);
    __Func_8093040(9, 0, 0xa);
    { PIN2; q0 = 0; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(2, *(int *)(p + 8), *(int *)(p + 0x10));
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xd4; q2 = 0x86 << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x4002; q1 = 0; q2 = 0x14;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_80921c4(2, 0xca, 0xfe);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0xa);
    __Func_8093040(2, 0, 0xa);
    { PIN3; q0 = 8; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(8, 0, 0);
    { PIN3; q0 = 9; q1 = 0x80 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(8, a1, 0);
    __Func_8092adc(9, a1, 0x14);
    { PIN3; q0 = 2; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(2, 1);
    __Func_8093040(2, 0, 0xa);
    __Func_80925cc(8, 2);
    __Func_8093040(8, 0, 0xa);
    { PIN3; q0 = 8; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(8, 2);
    __Func_8093040(8, 0, 0xa);
    __Func_80925cc(9, 2);
    __Func_8093040(9, 0, 0x14);
    __Func_80925cc(2, 1);
    __CutsceneWait(0xa);
    { PIN3; q0 = 2; q1 = 0xc0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0x4002; q1 = 0;
      __Func_8093054(q0, q1); }
    __CutsceneWait(0xa);
    __Func_80925cc(8, 1);
    __MapActor_DoAnim(8, 4);
    __Func_8093040(8, 0, 0xa);
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xa0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(9, 1);
    __MapActor_DoAnim(9, 3);
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(9, 0, 0xa);
    { PIN3; q0 = 2; q1 = 0xc0 << 7; q2 = 0x3c;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(2, 0, 0xa);
    __Func_8092adc(8, 0, 0);
    { PIN3; q0 = 9; q1 = 0x80 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(8, a1, 0);
    __Func_8092adc(9, a1, 0xa);
    __Func_80925cc(8, 1);
    { PIN2; q1 = 0; q0 = 8;
      __Func_8092c40(q0, q1); }
    { PIN3; q0 = 2; q1 = 0xc0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    if (__Func_8091c7c(0, 0) == 0) {
        __Func_80925cc(2, 2);
        __Func_8093040(0x4002, 0, 0xa);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        { register unsigned short *a __asm__("r2");
          a = (unsigned short *)(iwram_3001ebc + (0xec << 1));
          *a += 1; }
        __MapActor_DoAnim(2, 4);
        __Func_8093040(0x4002, 0, 0xa);
    }
    __Func_80925cc(9, 2);
    __Func_8093040(9, 0, 0xa);
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(2, 4);
    __Func_8093040(2, 0, 0xa);
    { PIN2; q0 = 8; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    __Func_8093040(8, 0, 0xa);
    { PIN3; q0 = 2; q1 = 0xa0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(2, 3);
    __Func_8093040(2, 0, 0xa);
    { PIN2; q0 = 8; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __MapActor_Surprise(9, 0x81 << 1);
    __CutsceneWait(0x3c);
    __Func_80925cc(8, 2);
    __Func_8093040(8, 0, 0xa);
    __Func_80925cc(9, 2);
    { PIN3; q0 = 9; q1 = 0x80 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(9, 0, 0xa);
    __Func_8092adc(8, 0, 0xa);
    __MapActor_DoAnim(8, 3);
    { PIN3; q0 = 8; q1 = 0xc0 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __ActorMessage(8, 0);
    { PIN3; q0 = 9; q1 = 0xc0 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(2, 3);
    { PIN3; q0 = 2; q1 = 0xc0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0x4002; q1 = 0;
      __Func_8093054(q0, q1); }
    __CutsceneWait(0xa);
    __Func_80925cc(2, 1);
    __Func_8093040(0x4002, 0, 0xa);
    __MapActor_DoAnim(0, 3);
    __MapActor_DoAnim(2, 3);
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(2, 3);
    { PIN3; q0 = 8; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetSpeed(9, 0xcccc, 0x6666);
    ((struct Actor *)__MapActor_GetActor(8))->f6 = 0;
    ((struct Actor *)__MapActor_GetActor(9))->f6 = 0x80 << 8;
    __MapActor_GetActor(8)[0x5a] &= 0xfe;
    __MapActor_GetActor(9)[0x5a] &= 0xfe;
    __Func_809218c(8, 0xa8, 0xe8);
    __Func_80921c4(9, 0xd4, 0xe8);
    __MapActor_SetAnim(8, 1);
    __CutsceneWait(0x14);
    { register int one __asm__("r5");
      one = 1;
      __MapActor_GetActor(8)[0x5a] |= one;
      p = __MapActor_GetActor(9) + 0x5a;
      one |= *p;
      *p = one;
    }
    __Func_80921c4(2, 0xc0, 0xe8);
    __Func_8092adc(2, 0xc0 << 8, 0);
    __PlaySound(0xbc);
    __CopyMapTiles(0x24, 0x17, 0x2b, 0xc, 2, 2);
    __WaitFrames(5);
    __CopyMapTiles(0x27, 0x17, 0x2b, 0xc, 2, 2);
    __WaitFrames(5);
    __Func_80921c4(2, 0xc0, 0xde);
    __MapActor_SetPos(2, 0, 0);
    { PIN3; q0 = 0; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0, 0xc0, 0xde);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetAnim(8, 3);
    __MapActor_DoAnim(9, 3);
    __Func_809218c(8, 0xb8, 0xe8);
    __Func_80921c4(9, 0xc6, 0xe8);
    __Func_809218c(8, 0xbc, 0xd4);
    __Func_80921c4(9, 0xc2, 0xd4);
    __MapActor_SetPos(8, 0, 0);
    __MapActor_SetPos(9, 0, 0);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x18;
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x201;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(5);
    __CutsceneEnd();
}
