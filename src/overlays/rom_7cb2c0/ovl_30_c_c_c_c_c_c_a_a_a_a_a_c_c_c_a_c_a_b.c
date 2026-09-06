/* OvlFunc_945_200b8ac  --  0x0200b8ac
 *   [asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a_c_a.s,
 *    1st of 2]
 *
 * 433 instructions of straight-line cutscene: 112 call sites, three guarded
 * actor fetches, one pool dump, no other control flow.  Built at the tree
 * default -O2 -- no Makefile rule matches this stem, so `asm/%.o: src/%.c`
 * applies and objcmp prints no `(built with: ...)` line.  NO FLAG GROUP IS
 * INVOLVED.
 *
 * ============================================================================
 * THE HIGH REGISTERS WERE NEVER THE BLOCKER, AND THIS FUNCTION WAS PICKED TO
 * TEST THAT.  It scored the worst band the candidate metric has: `hi` = 23
 * high-register references over `hiv` = 4 DISTINCT high registers, which the
 * batch-242 correction ("hi high with hiv at 3 or 4 is the real reject") names
 * as genuine pressure.  It was passed over four rounds on that score.  It
 * matched from a standing start with a COMPUTED pin set and no register lever
 * of any kind.
 *
 * WHAT THE ROM HOLDS.  Seven constants survive calls, filling the ENTIRE
 * callee-saved set -- there is no spare register and so no rotation freedom:
 *
 *     r5  0x80 << 7   0x4000     r8   0xdc << 1   0x1b8
 *     r6  0x80 << 1   0x100      r9   0xa0 << 7   0x5000
 *     r7  0xb0 << 8   0xb000     r10  0xd0 << 8   0xd000
 *                                r11  0x80 << 8   0x8000
 *
 * THE 23 REFERENCES ARE 8 + 4 + 11.  EIGHT are the prologue/epilogue save and
 * restore (`mov r7, r11` ... `mov r11, r7`), which no source spelling controls
 * and which appear the moment ANY value lands high.  FOUR are the defining
 * copies, ELEVEN are argument copies.  The metric counts the boilerplate.
 *
 * gcc REACHES ALL FOUR UNAIDED, ON THE FIRST COMPILE, FROM PLAIN C.  The
 * unpinned draft emits `mov fp, r2` / `mov r9, r3` / `mov sl, r2` / `mov r8,
 * r3` with nothing in the source asking for it, and pushes r5/r6/r7 plus the
 * same hand-rolled high save.  IT GETS THE COUNT RIGHT AND THE MEMBERSHIP
 * WRONG:
 *
 *     value              ROM           plain C, unpinned
 *     0xdc << 1          r8            r11 (fp)          held, right value
 *     0xb0 << 8          r7            r9                held, right value
 *     0x100              r6            r10, as `sub r2, r2, #192` off 0x1c0
 *     0xcc << 1          rebuilt x5    r8                COMMONED, wrongly
 *     0xcccc / 0x6666    pooled x5     r6 / r7           COMMONED, wrongly
 *     0xa0<<7, 0xd0<<8, 0x80<<8, 0x80<<7   held          NOT held
 *
 * So the residue was a CSE-CLASS MEMBERSHIP difference wearing high-register
 * clothing.  Four callee-saved slots were spent by both sides on the first
 * try; they were spent on different constants.  Ordering pins fixed it because
 * a pin REMATERIALISES its value at the site and thereby removes that site
 * from its CSE class -- pinning the five `0xcc << 1` sites and the five
 * `SetSpeed` pool pairs evicts them, and gcse then spends the freed registers
 * on exactly the seven values the ROM holds.
 * ============================================================================
 *
 * THE HOLE TEST COMPUTED THE ANSWER; NO SEARCH WAS RUN.  45 of the 112 sites
 * carry an expensive argument (a `mov`+`lsl` build or a pool load).  18 of
 * those 45 receive an argument by `mov rLOW, rHIGH` and are therefore HOLES BY
 * CONSTRUCTION -- the ROM already commoned that value and a pin could only
 * rematerialise it.  "Nominated minus holes" is 27 pins and is EXACT on the
 * first screen:
 *
 *     no pins ............................ 324 of 442, 4 bytes SHORT
 *     nominated minus holes (27) ......... EXACT
 *     all 45 nominated ................... 405 of 442, 16 bytes SHORT
 *     every arg-bearing site (111) ....... 405 of 442, 16 bytes SHORT
 *
 * The last two rows are the same object: pinning the 66 all-cheap sites on top
 * of the 45 changes nothing, which is the recorded "an all-cheap site needs no
 * ordering pin" measured rather than assumed.  Neither `__Func_8092c40` nor
 * `__Func_8093054` occurs here, so the by-name exception does not arise.
 *
 * REPLAY THE ROM'S ARGUMENT WINDOW.  Each pinned site is written in the ROM's
 * emitted order INCLUDING THE SHIFTS (`q1 = 0xcc; q0 = 0x0; q1 <<= 1;`),
 * transcribed mechanically by extract.py into windows.txt.  On the 27-pin set:
 *
 *     ROM window replay .................. EXACT
 *     ascending fill, shifts inline ......   2 differing, exact size
 *     descending fill, shifts inline .....  85 differing, exact size
 *
 * This is the b244 headline holding at a much smaller amplitude, and the
 * DIRECTION of its failure inverts: there ascending was 472 and 8 bytes SHORT
 * while descending was 2; here ascending is 2 and descending is 85, and NEITHER
 * loses bytes.  Take "replay the window" as the rule and "descending" as a
 * coincidence of that function -- the two are twins by overlay, callee set and
 * shape, and the lever still inverted between them.
 *
 * TWENTY-THREE PINS, MINIMAL BY MEASUREMENT FROM BOTH ENDS.  The 27 candidates
 * were stripped one at a time under objcmp, forward and reverse, each to a
 * fixpoint.  Both directions reach the SAME 23 and a second round moves
 * nothing.  The four inert ones are sites 4 (`OvlFunc_945_200c8ac`, the only
 * four-argument call, whose `neg` and two shifts gcc already orders correctly),
 * 17 (`__MessageID(0x1e27)`), 82 (`OvlFunc_945_200c880(0x1b, 0xc0 << 6)`) and
 * 112 (`__SetFlag(0x926)`) -- three of them single-argument sites, where a pin
 * has no order to constrain.
 *
 * THE PIN SET IS ALSO MINIMAL IN WIDTH, AND WIDTH IS A SIZE AND NOT A SET.
 * Greedily narrowing each site takes 69 named arguments to 41; ten sites need
 * only r0.  Run forward and reversed the total is 41 BOTH TIMES with a
 * DIFFERENT set -- forward holds 12:1/13:2, reverse holds 12:2/13:1, and the
 * same swap happens at 64/72 and across 101/102/108.  "N is a size, not a set"
 * reaches the WIDTH axis, not just the count axis.  The forward set is carried.
 *
 * MEASURED AND HARMFUL -- the high-register levers, tried BECAUSE this run was
 * about them:
 *
 *     seven held constants as named locals, no pins ..... 359 (vs 324 bare)
 *     the four high-register values named, no pins ...... 343 (vs 324 bare)
 *     seven held constants named, ON TOP of the 23 pins .. 400, 4 bytes LONG
 *
 * Naming the held values is worse in every combination, and it BREAKS a
 * finished match.  The mechanism is the recorded one running in reverse: a name
 * is a pseudo with a live range, and seven of them fused across 112 call sites
 * ask for an eighth callee-saved register that does not exist.  No `-ffixed-`
 * flag, no `register int __asm__("r8")`, and no live-range split was needed or
 * tried on the match path.
 *
 * THE ACTOR POINTER IS `unsigned char *` and the three guarded fetches read
 * `*(int *)(p + 8)` / `*(int *)(p + 0x10)`; none of the three needs a pin.
 * `iwram_3001ebc` is `unsigned char *` and the one store is a WORD:
 * `*(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x80 << 1;`.  That 0x100 is the
 * SAME constant as the `__MapActor_Emote` id at sites 45 and 79 -- the ROM
 * commons the stored value with the argument, which is why it must be spelled
 * `0x80 << 1` and left bare.
 *
 * The `0x1e27` message id, the `0x926` save bit, the `0xcccc`/`0x6666` speeds
 * and the `0x1000001`/`0x103`/`0x101` script words are all bare literals:
 * objcmp reports 113 relocations identical and the only symbol this function
 * carries is `iwram_3001ebc` (one R_ARM_ABS32; every other record is an
 * R_ARM_THM_CALL).
 *
 * LANDING NEEDS A SPLIT.  The .s holds two functions and this is the first;
 * overlays/rom_7cb2c0/overlay.ld:64 is the ONLY checked-in line naming the .o,
 * and the .s has no .data, .rodata or .bss directive at all (the map shows both
 * sections at zero length, and no linker script mentions either).  The
 * `.pool_aligned` block before `.L3cec` is THIS function's pool and stays with
 * it.  Split into `..._c_a_c_a_a` (this, from C) and `..._c_a_c_a_b.s`
 * (OvlFunc_945_200bd10, 0xdc bytes), replace ld line 64 with the two stems in
 * order, and add no explicit Makefile rule.
 *
 * Twenty-three register pins => fakematch; needs a fakematch.txt row.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __MapTransitionIn(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __Func_809218c(int slot, int a, int b);
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_809259c(int slot, int a);
extern void __Func_80925cc(int slot, int a);
extern void __Func_8092adc(int slot, int a, int b);
extern void __Func_8093040(int slot, int a, int b);
extern void OvlFunc_945_200c86c(int n);
extern void OvlFunc_945_200c880(int slot, int v);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void OvlFunc_945_200c8ac(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_945_200b8ac(void)
{
    unsigned char *p;

    __CutsceneStart();
    OvlFunc_945_200c8e8(0x19, 0x0, 0x0);
    OvlFunc_945_200c8e8(0x18, 0x1, 0x0);
    OvlFunc_945_200c8ac(0xdc << 17, -0x1, 0xa8 << 16, 0x1000001);
    OvlFunc_945_200c890(0x1b, 0xdc << 1, 0xa4, 0xa0 << 7);
    OvlFunc_945_200c890(0x8, 0xd6 << 1, 0xbe, 0xd0 << 8);
    OvlFunc_945_200c890(0x9, 0xe2 << 1, 0xbe, 0xb0 << 8);
    __MapActor_SetAnim(0x9, 0x1);
    OvlFunc_945_200c890(0x0, 0xdc << 1, 0x86, 0x80 << 8);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x80 << 1;
    __MapTransitionIn();
    { PIN3; q0 = 0x0; q1 = 0xcccc; q2 = 0x6666;
          __MapActor_SetSpeed(q0, q1, q2); }
    { PIN1; q0 = 0x0;
          __Func_80921c4(q0, 0xcc << 1, 0x86); }
    { PIN2; q1 = 0xcc; q0 = 0x0; q1 <<= 1;
          __Func_80921c4(q0, q1, 0x94); }
    { PIN1; q0 = 0x0;
          __Func_80921c4(q0, 0xd4 << 1, 0x94); }
    { PIN2; q1 = 0x80; q0 = 0x0; q1 <<= 7;
          __Func_8092adc(q0, q1, 0x14); }
    __Func_80925cc(0x1b, 0x1);
    __MessageID(0x1e27);
    OvlFunc_945_200c86c(0x1b);
    __Func_80925cc(0x8, 0x1);
    OvlFunc_945_200c86c(0x8);
    __MapActor_DoAnim(0x1b, 0x3);
    OvlFunc_945_200c86c(0x1b);
    OvlFunc_945_200c880(0x1b, 0xd0 << 8);
    p = __MapActor_GetActor(0x0);
    if (p != 0) {
        __MapActor_SetPos(0x1, *(int *)(p + 8), *(int *)(p + 0x10));
    }
    { PIN3; q0 = 0x1; q1 = 0xcccc; q2 = 0x6666;
          __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0x1, 0xdc << 1, 0x94);
    { PIN2; q1 = 0x80; q0 = 0x1; q1 <<= 7;
          __Func_8092adc(q0, q1, 0x0); }
    p = __MapActor_GetActor(0x1);
    if (p != 0) {
        __MapActor_SetPos(0x2, *(int *)(p + 8), *(int *)(p + 0x10));
    }
    { PIN3; q0 = 0x2; q1 = 0xcccc; q2 = 0x6666;
          __MapActor_SetSpeed(q0, q1, q2); }
    { PIN1; q0 = 0x2;
          __Func_80921c4(q0, 0xe4 << 1, 0x94); }
    { PIN2; q1 = 0x80; q0 = 0x2; q1 <<= 7;
          __Func_8092adc(q0, q1, 0x0); }
    p = __MapActor_GetActor(0x2);
    if (p != 0) {
        __MapActor_SetPos(0x3, *(int *)(p + 8), *(int *)(p + 0x10));
    }
    { PIN3; q0 = 0x3; q1 = 0xcccc; q2 = 0x6666;
          __MapActor_SetSpeed(q0, q1, q2); }
    { PIN1; q0 = 0x3;
          __Func_80921c4(q0, 0xec << 1, 0x94); }
    { PIN2; q1 = 0x80; q0 = 0x3; q1 <<= 7;
          __Func_8092adc(q0, q1, 0x14); }
    OvlFunc_945_200c8e8(0x0, 0x0, 0x3c);
    OvlFunc_945_200c8e8(0x1, 0x80 << 7, 0x14);
    OvlFunc_945_200c8e8(0x2, 0x1, 0x14);
    __Func_8092adc(0x1b, 0xa0 << 7, 0x14);
    OvlFunc_945_200c86c(0x1b);
    __Func_809259c(0x9, 0x1);
    __MapActor_Emote(0x9, 0x80 << 1, 0x28);
    OvlFunc_945_200c86c(0x9);
    __Func_809259c(0x1, 0x3);
    { PIN1; q0 = 0x1;
          __MapActor_Emote(q0, 0x103, 0x3c); }
    __MapActor_DoAnim(0x1b, 0x3);
    OvlFunc_945_200c86c(0x1b);
    __Func_80925cc(0xa, 0x1);
    __MapActor_SetAnim(0xa, 0x3);
    OvlFunc_945_200c86c(0xa);
    __MapActor_SetAnim(0x8, 0x3);
    __MapActor_SetAnim(0x9, 0x3);
    __MapActor_SetAnim(0xb, 0x3);
    __MapActor_SetAnim(0xc, 0x3);
    __MapActor_DoAnim(0xd, 0x3);
    OvlFunc_945_200c8e8(0x0, 0x0, 0x28);
    OvlFunc_945_200c8e8(0x2, 0x1, 0x0);
    OvlFunc_945_200c8e8(0x1, 0x80 << 7, 0x14);
    __MapActor_DoAnim(0x1b, 0x4);
    OvlFunc_945_200c86c(0x1b);
    { PIN1; q0 = 0x8;
          __MapActor_Emote(q0, 0x81 << 1, 0x3c); }
    __Func_809259c(0x8, 0x1);
    OvlFunc_945_200c86c(0x8);
    __MapActor_DoAnim(0x1b, 0x3);
    OvlFunc_945_200c86c(0x1b);
    __Func_8092adc(0x8, 0x0, 0x0);
    __Func_8092adc(0x9, 0x80 << 8, 0x28);
    { PIN2; q1 = 0x81; q0 = 0x8; q1 <<= 1;
          __MapActor_Emote(q0, q1, 0x0); }
    { PIN2; q1 = 0x81; q0 = 0x8; q1 <<= 1;
          __MapActor_Emote(q0, q1, 0x28); }
    __Func_80925cc(0x1b, 0x1);
    __MapActor_SetAnim(0x1b, 0x3);
    __Func_8093040(0x1b, 0x0, 0x14);
    __MapActor_SetAnim(0x8, 0x3);
    __MapActor_DoAnim(0x9, 0x3);
    __CutsceneWait(0x28);
    __MapActor_Emote(0x9, 0x80 << 1, 0x14);
    OvlFunc_945_200c880(0x9, 0xb0 << 8);
    OvlFunc_945_200c86c(0x9);
    OvlFunc_945_200c880(0x1b, 0xc0 << 6);
    { PIN1; q0 = 0x1b;
          __MapActor_Emote(q0, 0x101, 0x3c); }
    __Func_8093040(0x1b, 0x0, 0x3c);
    { PIN1; q0 = 0x1b;
          __MapActor_Emote(q0, 0x83 << 1, 0x14); }
    OvlFunc_945_200c880(0x1b, 0xb0 << 8);
    __MapActor_DoAnim(0x1b, 0x3);
    OvlFunc_945_200c86c(0x1b);
    OvlFunc_945_200c8e8(0x3, 0x2, 0x50);
    OvlFunc_945_200c880(0x8, 0xd0 << 8);
    __Func_809259c(0x8, 0x2);
    OvlFunc_945_200c86c(0x8);
    __MapActor_DoAnim(0x9, 0x3);
    __Func_809259c(0x9, 0x2);
    OvlFunc_945_200c86c(0x9);
    OvlFunc_945_200c880(0x1b, 0xa0 << 7);
    __MapActor_DoAnim(0x1b, 0x3);
    __Func_80925cc(0x1b, 0x1);
    OvlFunc_945_200c86c(0x1b);
    { PIN2; q0 = 0x1b; q1 = 0xcccc;
          __MapActor_SetSpeed(q0, q1, 0x6666); }
    { PIN1; q0 = 0x1b;
          __Func_80921c4(q0, 0xcc << 1, 0x9e); }
    { PIN2; q1 = 0xcc; q0 = 0x1b; q1 <<= 1;
          __Func_80921c4(q0, q1, 0x94); }
    __Func_8092adc(0x1b, 0x0, 0x14);
    __Func_80925cc(0x1b, 0x1);
    OvlFunc_945_200c86c(0x1b);
    OvlFunc_945_200c8e8(0x1, 0x80 << 8, 0x14);
    OvlFunc_945_200c8e8(0x2, 0x1, 0x0);
    { PIN2; q1 = 0xcc; q0 = 0x1b; q1 <<= 1;
          __Func_80921c4(q0, q1, 0x86); }
    __Func_809218c(0x1b, 0xdc << 1, 0x86);
    __CutsceneWait(0x28);
    OvlFunc_945_200c8e8(0x9, 0xa, 0x0);
    __SetFlag(0x926);
}
