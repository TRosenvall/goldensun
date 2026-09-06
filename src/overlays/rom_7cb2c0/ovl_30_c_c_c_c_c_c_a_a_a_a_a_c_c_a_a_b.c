/* OvlFunc_945_2009b34  --  0x02009b34
 *   [asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a_a.s, 1st of 2]
 *
 * 377 instructions of straight-line cutscene behind one save-flag test: 106
 * calls, no loops, no inner branches, and one `unsigned char` bit-flip pair on
 * an actor.  Built at the TREE DEFAULT -O2 -- objcmp prints no
 * `(built with: ...)` line and no Makefile rule captures this stem, so
 * `asm/%.o: src/%.c` applies.  NO FLAG GROUP IS INVOLVED.
 *
 * ============================================================================
 * THE HIGH REGISTERS WERE NEVER THE PROBLEM.  THIS WAS SELECTED AS THE
 * WORST HIGH-REGISTER CASE IN THE CORPUS -- 26 references over FOUR DISTINCT
 * high registers -- AND THE HIGH REGISTERS COST NOTHING.
 * ============================================================================
 *
 * WHAT THE ROM HOLDS.  Six callee-saved registers carry EIGHT constants, two
 * of the registers being reused after their first value dies:
 *
 *     r5   0xa00e   -> gScript_945__0200e6a8    r9    0xb0 << 8
 *     r6   0x800c                               r10   0xd0 << 8
 *     r8   0x200d   -> 0xc0 << 6                r11   0x80 << 8
 *
 * Of the 26 high-register references, EIGHT are the prologue/epilogue
 * save-restore boilerplate that any four-high-register frame carries
 * (`mov r6, r11` ... `mov r11, r3`).  The other 18 are five definitions and
 * thirteen uses.  THE METRIC OVERSTATES BY THE SAVE/RESTORE COUNT and should
 * be read net of it: this function's real figure is 18, not 26.
 *
 * GCC REACHES ALL FOUR HIGH REGISTERS UNAIDED, ON THE FIRST DRAFT, WITH NO
 * REGISTER PIN ANYWHERE ABOVE r2.  The plain unpinned C -- every call spelled
 * as a literal argument list, nothing named -- comes out 289 of 393 differing
 * and already writes `mov r8, r3` / `mov sl, r3` / `mov r9, r3` / `mov fp, r3`
 * in the right shape.  The final matching candidate contains exactly three
 * `__asm__` register declarations and they are r0, r1 and r2.  The recorded
 * strike of the r8-r11 reject holds and this is a second, stronger instance
 * of it: allocation INTO the high registers was never in question.
 *
 * WHAT WAS ACTUALLY WRONG WAS *WHICH VALUES* GOT COMMONED, AND IT SHOWED UP
 * AS ONE EXTRA LOW REGISTER.  Unpinned gcc spends SEVEN callee-saved
 * registers where the ROM spends six -- `push {r5, r6, r7, lr}` against
 * `push {r5, r6, lr}`, which is the very first differing encoding
 * (`ref b560 ours b5e0`).  Line up the two sets:
 *
 *     ours     r5 0xa00e   r6 0x800c   r7 0x102   r8 0x3000
 *              r9 0x101    r10 0x103   r11 0xd000
 *     rom      r5 0xa00e   r6 0x800c   r8 0x200d/0x3000
 *              r9 0xb000   r10 0xd000  r11 0x8000
 *
 * gcse commons 0x101, 0x103 and 0x81 << 1 -- which the ROM re-materialises at
 * every site -- and then has no register left for 0x200d, 0xb0 << 8 and
 * 0x80 << 8, which the ROM does hold.  Seven candidates, seven registers,
 * three of them the wrong three.  The cure is to DELETE CANDIDATES, not to
 * pin registers: a call-clobbered pin on a site forces the constant to be
 * rebuilt there and it stops being a commoning candidate at all.  Once the
 * three wrong values are gone the allocator picks the ROM's six on its own.
 *
 * So: the residue was a CSE-SET mismatch that a naive prologue read reports as
 * a register-pressure problem.  "The ROM saves one FEWER callee-saved register
 * than you do" is a COMMONING tell, and it is the mirror of the recorded
 * "ONE MORE -> live range" rule in the sibling
 * src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_c_a_a.c.  Count both ways.
 *
 * TWENTY-TWO PINS, MINIMAL BY MEASUREMENT FROM BOTH ENDS.  Twenty-five sites
 * were nominated by the hole test (every site where the ROM materialises an
 * expensive constant inline; every `mov rLOW, rHIGH` site skipped as a hole
 * by construction).  Stripping one at a time finds four individually inert
 * (sites 19, 66, 89, 103); greedy removal with a re-test after every drop
 * reaches 22 FORWARD and 22 REVERSED, and a second one-at-a-time round over
 * the 22 moves nothing.  IT IS NOT ONE SET: forward keeps site 103 and drops
 * 66, reverse keeps 66 and drops 103 -- `__Func_8092adc(0xe, 0x80<<7, 0x28)`
 * and `__Func_8092adc(0, 0x80<<7, 0)`, the two ends of one CSE class, either
 * alone exact.  "N pins is a size, not a set", again.
 *
 * THE UNIFORM ASCENDING FILL IS EXACT HERE AND THE TEMPLATE'S LEVER INVERTS.
 * The immediate neighbour ovl_30_..._c_c_a_b.c (same overlay, same .s family,
 * landed the day before) records "REPLAY THE ROM'S ARGUMENT WINDOW, DO NOT
 * FILL ASCENDING", with ascending 472 differing and eight bytes SHORT.  On
 * this function, measured at the same 22 pins:
 *
 *     uniform ascending fill ................. EXACT   <- carried
 *     ROM argument-window replay ............. EXACT
 *     uniform descending fill ................ 52 of 393, same size
 *     whole-value fill (`q1 = 0x81 << 1;`) ... 224 of 393, 4 bytes SHORT
 *     no pins at all ......................... 289 of 393, 4 bytes LONG
 *
 * Ascending and the ROM replay TIE, so the simpler one is carried.  The
 * whole-value row is the recorded shorter-output tell running backwards in its
 * cleanest form: folding the shift into the initialiser hands gcse a plain
 * CONST_INT it can hoist, r7 comes back (`b5e0`) and the output loses four
 * bytes.  The SPLIT shift (`q1 = 0x81; q1 <<= 1;`) is what keeps it rebuilt.
 *
 * THE LAST TWO ENCODINGS WERE THE `orr`, NOT THE REGISTERS, AND THE RECORDED
 * LEVER CLOSED THEM.  At the 22-pin set the function is 1032 bytes, 393
 * encodings, with exactly two differing:
 *
 *     rom    ldrb r2, [r0] / mov r3, #1 / orr r3, r2     the CONSTANT is rd
 *     ours   ldrb r3, [r0] / mov r2, #1 / orr r3, r2     the VALUE is rd
 *
 * The `&= 0xfe` site four instructions earlier is already right, exactly the
 * asymmetry docs/elevation.md records under "`orr rd, rs` -- which operand
 * becomes the destination".  Seven spellings, and the class discriminator is
 * sharp:
 *
 *     unsigned char one = 1; p[0x5a] = one | p[0x5a];  EXACT   <- carried
 *     u = p[0x5a] | 1; p[0x5a] = u;  (name the result) EXACT
 *     same two, reached through `bp = &GetActor(0)[0x5a]` EXACT
 *     __MapActor_GetActor(0)[0x5a] |= 1;               2 differing
 *     p = GetActor(0); p[0x5a] |= 1;                   2 differing
 *     bp = &GetActor(0)[0x5a]; *bp |= 1;               2 differing
 *     int one = 1; p[0x5a] = one | p[0x5a];            2 differing
 *
 * The NARROW type is the lever and the named pointer is only scaffolding to
 * avoid a second call: `int one` is 2, `unsigned char one` is 0, and both
 * pointer spellings behave identically under either type.  That confirms
 * "## The constant-as-destination lever: `orr` wants a NARROW local" and adds
 * the case where the object is a CALL RESULT, which the recorded entries do
 * not cover -- there the plain aggregate form is unavailable and a pointer
 * must be named, but naming it is inert on its own.
 *
 * NEW: THE ROM'S TWO POOL LOADS OF 0xa00e IN ONE BASIC BLOCK NEED ONLY THE
 * PIN HERE, NOT THE NAMED COPY.  `__Func_8093040(0xa00e, 0, 0x14)` loads
 * 0xa00e from the pool and two calls later `ldr r5, =0xa00e` loads it again
 * for the held copy.  ovl_30_..._a_a_a_c_a_a.c records this shape as needing
 * BOTH a call-clobbered pin on the literal site AND a named `msg = ...;`
 * assigned after it, "either half alone is 4 of 350".  On this function the
 * PIN ALONE IS EXACT and the named copy is inert scaffolding (measured: with
 * `msg`, EXACT; without `msg`, EXACT; without the pin, 17 differing).  Move
 * `msg` to BEFORE the pinned call and it collapses -- 351 of 393, TWELVE
 * bytes short, r7 back in the prologue -- so the ORDER constraint is real and
 * only the second half of the recipe is conditional.  The difference from the
 * recorded case is that there the held copy was reached through `msg` at four
 * later sites inside the same block; here nine later sites read the plain
 * literal and gcse forms the copy itself.
 *
 * MEASURED AND INERT, deliberately NOT carried: named locals for all six
 * ROM-held constants (the shape the c8ac sibling ovl_30_..._a_a_a_a_a_c_b.c
 * uses); `x = 0xe8 << 17; m = -1;` hoisted out of the OvlFunc_945_200c8ac
 * call, which THAT sibling needed and this one does not; `msg = 0xa00e;` in
 * either legal slot; `&= ~1` for `&= 0xfe`; `!= 0` on the flag test; the
 * three extra pins at sites 19, 66 and 89.
 *
 * THE ACTOR IS `unsigned char *` AND ONLY THE `|=` SITE NEEDS A LOCAL.  The
 * `&= 0xfe` site keeps the recorded expression form
 * `__MapActor_GetActor(0)[0x5a] &= 0xfe;` -- `add r0, #0x5a` in place, no copy.
 *
 * Every constant is a bare literal: objcmp reports 108 relocations identical
 * and the ONLY non-call relocation in the function is the single ABS32 on
 * `gScript_945__0200e6a8`.  0x1d56, 0x911, 0x922, 0x200d, 0x800c, 0xa00e and
 * the speed words are plain integers.
 *
 * LANDING NEEDS A SPLIT.  The .s holds TWO functions and this is the first;
 * overlays/rom_7cb2c0/overlay.ld:58 is the only checked-in line naming the .o,
 * and the .s has no .data, .rodata, .bss or `.word` block anywhere -- there is
 * nothing to carry across.  Split into `..._c_c_a_a_a` (this function, C) and
 * `..._c_c_a_a_b` (OvlFunc_945_2009f3c, asm) and build via the generic
 * `asm/%.o: src/%.c` rule; add no explicit Makefile rule.
 *
 * Twenty-two register pins => fakematch; needs a fakematch.txt row.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_808e118(void);
extern void __MessageID(int id);
extern void __Func_80933d4(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int x, int z);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_WaitScript(int slot);
extern void OvlFunc_945_200c86c(int n);
extern void OvlFunc_945_200c880(int slot, int v);
extern void OvlFunc_945_200c8ac(int a, int b, int c, int d);
extern unsigned char gScript_945__0200e6a8[];

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_945_2009b34(void)
{
    unsigned char *p;
    unsigned char one = 1;

    if (__GetFlag(0x911)) {
        __CutsceneStart();
        __Func_808e118();
        { PIN2; q0 = 0x26666; q1 = 0x4ccc;
          __Func_80933d4(q0, q1); }
        OvlFunc_945_200c8ac(0x5b70000, -1, 0xe8 << 17, 0x10000014);
        __Func_80925cc(0xd, 1);
        __MessageID(0x1d56);
        OvlFunc_945_200c86c(0x200d);
        OvlFunc_945_200c880(0xc, 0xd0 << 8);
        { PIN3; q0 = 0xc; q1 = 0x81; q1 <<= 1; q2 = 0x14;
          __MapActor_Emote(q0, q1, q2); }
        __Func_809259c(0xc, 2);
        OvlFunc_945_200c86c(0x800c);
        __Func_80925cc(0xe, 1);
        { PIN3; q0 = 0xa00e; q1 = 0; q2 = 0x14;
          __Func_8093040(q0, q1, q2); }
        OvlFunc_945_200c880(0xc, 0);
        { PIN3; q0 = 0xc; q1 = 0x101; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0xe; q1 = 0x103; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __Func_809259c(0xe, 3);
        OvlFunc_945_200c86c(0xa00e);
        __MapActor_Surprise(0xc, 0x81 << 1);
        __CutsceneWait(0x28);
        __Func_809259c(0xc, 3);
        OvlFunc_945_200c86c(0x800c);
        __Func_80925cc(0xe, 1);
        OvlFunc_945_200c86c(0xa00e);
        OvlFunc_945_200c880(0xe, 0xb0 << 8);
        OvlFunc_945_200c86c(0xa00e);
        OvlFunc_945_200c880(0xc, 0xd0 << 8);
        { PIN3; q0 = 0xc; q1 = 0x80; q1 <<= 1; q2 = 0x1e;
          __MapActor_Emote(q0, q1, q2); }
        __Func_809259c(0xc, 1);
        OvlFunc_945_200c86c(0x800c);
        __MapActor_DoAnim(0xd, 4);
        OvlFunc_945_200c86c(0x200d);
        __Func_809259c(0xd, 2);
        OvlFunc_945_200c86c(0x200d);
        __MapActor_SetAnim(0xc, 4);
        OvlFunc_945_200c86c(0x800c);
        __MapActor_DoAnim(0xe, 4);
        OvlFunc_945_200c86c(0xa00e);
        OvlFunc_945_200c880(0xe, 0x80 << 8);
        __Func_809259c(0xe, 2);
        __Func_8093040(0xa00e, 0, 0x14);
        __Func_8092adc(0xc, 0, 0);
        { PIN3; q0 = 0xc; q1 = 0x81; q1 <<= 1; q2 = 0x50;
          __MapActor_Emote(q0, q1, q2); }
        __Func_8093040(0x800c, 0, 0x14);
        { PIN3; q0 = 0xe; q1 = 0x103; q2 = 0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0xd; q1 = 0x103; q2 = 0x3c;
          __MapActor_Emote(q0, q1, q2); }
        __Func_809259c(0xe, 2);
        OvlFunc_945_200c86c(0xa00e);
        OvlFunc_945_200c880(0xe, 0xb0 << 8);
        __Func_80925cc(0xe, 1);
        OvlFunc_945_200c86c(0xa00e);
        OvlFunc_945_200c880(0xd, 0xc0 << 6);
        { PIN3; q0 = 0xd; q1 = 0x101; q2 = 0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0xc; q1 = 0x101; q2 = 0x3c;
          __MapActor_Emote(q0, q1, q2); }
        __Func_80925cc(0xd, 1);
        OvlFunc_945_200c86c(0xd);
        { PIN3; q0 = 0xe; q1 = 0x103; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __Func_80925cc(0xe, 1);
        OvlFunc_945_200c86c(0xa00e);
        __Func_8092adc(0xc, 0xd0 << 8, 0);
        { PIN3; q0 = 0xd; q1 = 0xa0; q1 <<= 7; q2 = 0x28;
          __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(0xc, 0, 0);
        OvlFunc_945_200c880(0xd, 0xc0 << 6);
        __Func_80925cc(0xc, 2);
        __Func_8093040(0x800c, 0, 0x14);
        __Func_8092adc(0xe, 0x80 << 7, 0x28);
        OvlFunc_945_200c86c(0xa00e);
        __Func_809259c(0xc, 2);
        __Func_80925cc(0xd, 2);
        __CutsceneWait(0x3c);
        __Func_80925cc(0xd, 1);
        OvlFunc_945_200c86c(0xd);
        __MapActor_DoAnim(0xe, 3);
        OvlFunc_945_200c86c(0xa00e);
        { PIN3; q0 = 0xc; q1 = 0x81; q1 <<= 1; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __Func_80925cc(0xc, 2);
        OvlFunc_945_200c86c(0x800c);
        __MapActor_DoAnim(0xd, 3);
        OvlFunc_945_200c86c(0xd);
        __Func_8092adc(0xe, 0xb0 << 8, 0x28);
        __MapActor_SetAnim(0xe, 3);
        __MapActor_DoAnim(0xd, 3);
        { PIN3; q0 = 0xe; q1 = 0x19999; q2 = 0xcccc;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0xd; q1 = 0x19999; q2 = 0xcccc;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetBehavior(0xe, gScript_945__0200e6a8);
        __MapActor_SetBehavior(0xd, gScript_945__0200e6a8);
        __CutsceneWait(0x14);
        { PIN3; q0 = 0xc; q1 = 0x80; q1 <<= 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __MapActor_SetSpeed(0, 0x26666, 0x13333);
        __MapActor_GetActor(0)[0x5a] &= 0xfe;
        { PIN3; q0 = 0; q1 = 0xb8; q2 = 0x82; q2 <<= 2;
          __Func_80921c4(q0, q1, q2); }
        __CutsceneWait(1);
        p = __MapActor_GetActor(0);
        p[0x5a] = one | p[0x5a];
        __Func_8092adc(0, 0x80 << 8, 0x14);
        { PIN3; q0 = 0; q1 = 0x80; q1 <<= 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __MapActor_Jump(0xc, 4, 0x14);
        { PIN3; q0 = 0; q1 = 0xa0; q1 <<= 8; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        __Func_809259c(0xc, 2);
        OvlFunc_945_200c86c(0xc);
        { PIN3; q0 = 0xc; q1 = 0x19999; q2 = 0xcccc;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetBehavior(0xc, gScript_945__0200e6a8);
        __CutsceneWait(0x28);
        { PIN3; q0 = 0; q1 = 0x80; q1 <<= 7; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __MapActor_WaitScript(0xc);
        __SetFlag(0x922);
        __CutsceneEnd();
    }
}
