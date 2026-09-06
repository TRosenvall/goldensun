// fakematch
/* OvlFunc_945_200e110  --  0x0200e110
 *   [asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_c_a_c_c.s, 2nd of 2]
 *
 * 257 instructions of straight-line cutscene: 63 call sites, two word stores
 * through one iwram pointer, no branches at all.  Built at the TREE DEFAULT
 * -O2 -- objcmp prints no `(built with: ...)` line and no Makefile rule
 * captures this stem, so `asm/%.o: src/%.c` applies.  NO FLAG GROUP IS
 * INVOLVED.  Relocations: 63 R_ARM_THM_CALL plus ONE R_ARM_ABS32 on
 * `iwram_3001ebc`; 0x1f78, 0x201, 0x202, 0x92c, 0x935 and 0x917 are plain
 * integers, settled by relocation count and not by line count.
 *
 * ============================================================================
 * THE SCORE THAT NOMINATED THIS AS A REJECT -- 24 shared symbols and 30
 * high-register references over FOUR distinct high registers -- COST NOTHING.
 * No high register is pinned, no flag is used, and the whole lever is twelve
 * CALL-CLOBBERED pins whose only job is to DELETE COMMONING CANDIDATES.
 * ============================================================================
 *
 * WHAT THE ROM HOLDS.  Seven callee-saved registers, one of them reused after
 * its first value dies -- there is no spare and so no rotation freedom:
 *
 *     r5  actor 2, then 0x80 << 8   r9   0xcc << 1  (0x198)
 *     r6  actor 1                   r10  actor 3
 *     r7  0xe0 << 1  (0x1c0)        r11  &iwram_3001ebc
 *     r8  actor 0
 *
 * THE 30 REFERENCES ARE 8 + 22, AND THE METRIC OVERSTATES BY EXACTLY THE
 * EIGHT.  `mov r7,r11 / mov r6,r10 / mov r5,r9 / mov r7,r8` at entry and
 * `mov r8,r3 / mov r9,r5 / mov r10,r6 / mov r11,r7` at exit are the
 * prologue/epilogue boilerplate any four-high-register frame carries and no
 * source spelling controls them.  This is a THIRD independent sighting of the
 * same 8 (the two landed siblings in this overlay report it too); read the
 * figure net, as 22.
 *
 * GCC REACHES THE ROM'S FRAME UNAIDED AND EXACTLY.  The plain unpinned draft
 * -- every call a literal argument list, only the four actor handles named --
 * emits the ROM'S PROLOGUE INSTRUCTION FOR INSTRUCTION:
 *
 *     push {r5, r6, r7, lr} / mov r7, fp / mov r6, sl / mov r5, r9
 *     push {r5, r6, r7} / mov r7, r8 / push {r7}
 *
 * SAME SEVEN CALLEE-SAVED REGISTERS, DIFFERENT TENANTS.  Line the two up:
 *
 *     value                ROM              plain C, unpinned
 *     actors 0..3          r8 r6 r5 r10     r11 r9 r10 r7   held, right values
 *     0xcc << 1            r9               rebuilt at both sites
 *     0xe0 << 1            r7               rebuilt at both stores
 *     &iwram_3001ebc       r11              re-loaded from the pool twice
 *     0x80 << 8 (late)     r5, 2nd life     rebuilt
 *     0xeb << 1, 0xcd << 1 rebuilt x3, x2   r5 / r6   COMMONED, wrongly
 *     0xd0 << 8, 0xb0 << 8 rebuilt x2, x2   r5 / r6   COMMONED, wrongly
 *
 * gcse spends r5 and r6 on 0x1d6/0x19a and then on 0xd000/0xb000 -- four
 * constants the ROM re-materialises at every use -- and so has nothing left
 * for the four the ROM does hold.  204 of 264 encodings differ and the output
 * is EIGHT BYTES SHORT (660 against 668, 260 instructions against 264).
 *
 * THE PIN IS AN EVICTION DEVICE HERE, NOT AN ORDERING ONE, AND THIS FUNCTION
 * MEASURES THAT DIRECTLY.  Ten of the twelve pins name NO r0 at all: they pin
 * only the constant register (`{ PINR1; q1 = 0x80; q1 <<= 8; f(a1, q1, 0); }`),
 * leaving the actor argument bare.  A call-clobbered pin forces the constant
 * to be rebuilt at that site, which removes the site from its CSE class; once
 * 0x1d6, 0x19a, 0xd000, 0xb000, 0x8000 and 0x10000 are gone as candidates the
 * allocator picks the ROM's seven tenants on its own.  Pinning r0 as well
 * (the natural `PIN2`/`PIN3` shape both landed siblings use) is EXACT but
 * INERT -- 26 named arguments against 16 for the same object.
 *
 * NEW: A `mov rLOW, rHIGH` THAT DELIVERS A *CALL RESULT* IS NOT A HOLE.  The
 * recorded hole test reads "wherever the ROM supplies an argument via
 * `mov rLOW, rHIGH`, gcc commoned that value, so a pin there would only
 * rematerialise it".  Twenty-two of this function's sites take their actor
 * argument that way and TEN OF THEM ARE PINNED IN THE FINAL SET.  The
 * mechanism the rule encodes is REMATERIALISABILITY, and a value returned by a
 * `bl` cannot be rematerialised at all -- the copy is forced, so the pin costs
 * nothing and the site stays eligible.  Apply the register test only to values
 * gcc could rebuild: constants and pool loads.  Under that reading the test is
 * as sharp here as on the siblings.
 *
 * THE HOLE TEST STILL COMPUTED THE ANSWER; NO SEARCH WAS RUN.  Twenty-one of
 * the 63 sites materialise an expensive constant inline.  Exactly ONE of those
 * takes a rebuildable value by `mov rLOW, rHIGH` -- site 8,
 * `OvlFunc_945_200c890(0x1b, 0xcc << 1, 0x8e, 0xc0 << 6)`, whose `mov r1, r9`
 * is the defining copy of the held 0x198.  "Nominated minus that one hole" is
 * 20 pins and lands at FOUR of 264, EXACT SIZE, RELOCATIONS IDENTICAL on the
 * first screen; the four are one argument window and one fill change closes
 * them.  Pinning site 8 anyway is the sharpest confirmation available: it
 * rematerialises 0x198, the ROM's r9 tenant vanishes, the push mask itself
 * becomes the first differing encoding (`ref b5e0 ours b560`) and the score
 * goes to 76 at FOUR BYTES LONG.
 *
 * THE FILL DIRECTION DOES NOT TRANSFER, AND THE WHOLE/SPLIT AXIS IS THE REAL
 * ONE.  Measured at the minimal twelve:
 *
 *     ascending, SPLIT shift (`q1 = 0x80; q1 <<= 8;`) .. EXACT   <- carried
 *     ROM argument-window replay ....................... EXACT (at 20 pins)
 *     descending, split shift .......................... 16 of 264, same size
 *     whole-value (`q1 = 0x80 << 8;`) .................. 190, relocations differ
 *
 * The whole-value row is the recorded tell in its cleanest form: folding the
 * shift into the initialiser hands gcse a plain CONST_INT it hoists again, and
 * the CSE-class membership collapses back to the unpinned one.  The SPLIT
 * shift is what keeps the value rebuilt.  Ascending and descending differ by
 * only 16 here, against 85 on the sibling ovl_..._c_c_c_a_c_a_b.c and 65 on
 * ovl_..._a_a_a_c_a_a.c -- direction is a weak lever on this function and
 * splitting is the strong one.
 *
 * ONE ARGUMENT WINDOW, AND IT DISAPPEARS UNDER MINIMISATION.  At the 20-pin
 * screen the only residue was site 7,
 * `OvlFunc_945_200c890(8, 0xec << 1, 0x90, 0xa0 << 7)`: the ROM builds BOTH
 * shifted arguments before touching r0/r2 (`mov r1 / mov r3 / lsl r1 / lsl r3
 * / mov r0 / mov r2`) and an ascending fill interleaves them.  Replaying the
 * ROM's window there is EXACT.  But site 7 is not in the minimal set -- once
 * the ten constant pins are the only ones present, gcc orders that call
 * correctly with no pin at all.  A lever that is necessary at one pin set and
 * absent from the minimum is the "sufficient not necessary" rule with a date
 * on it: minimise BEFORE writing a window lever up as required.
 *
 * TWELVE PINS AND SIXTEEN NAMED REGISTERS, MINIMAL BY MEASUREMENT FROM BOTH
 * ENDS.  Twenty candidates were stripped one at a time under objcmp, forward
 * and reversed, each to a fixpoint.  Both directions reach TWELVE and a second
 * round moves nothing -- but NOT the same twelve:
 *
 *     forward  22 24 25 26 27 28 31 32 38 39 55 57
 *     reverse  22 24 25 26 27 28 29 30 36 37 55 57
 *
 * 29/30 and 31 are the two ends of the 0xeb<<1 / 0xcd<<1 CSE classes, 36/37
 * and 38/39 the two ends of the 0xd0<<8 / 0xb0<<8 classes; either end alone is
 * exact.  "N pins is a size, not a set", again, and here the swap is visibly a
 * choice of WHICH member of each class to evict.  The forward set is carried.
 * Narrowing is stable: greedy over individual registers, run forward and
 * reversed, reaches the SAME 16 named registers both times -- unlike the
 * sibling ovl_..._c_c_c_a_c_a_b.c, where width was also a size and not a set.
 *
 * MEASURED AND INERT, deliberately NOT carried: a named local for the held
 * 0xcc << 1 (gcse forms r9 unaided once site 8 is left bare); a named `spd`
 * local for the late 0x80 << 8 in r5's second life, in the live-range shape
 * ovl_..._a_a_a_c_a_a.c needed -- here gcc splits r5 by itself; flat
 * constants (0x10000, 0x8000, 0x1d6 ...) in place of every shifted spelling
 * outside the pins; r0 pinned alongside the constant at the ten eviction
 * sites; ROM-window replay in place of the ascending fill.
 *
 * MEASURED AND HARMFUL:
 *
 *     no pins at all ............................ 204 of 264, EIGHT SHORT
 *     all 21 expensive sites (hole included) .... 76, FOUR LONG, push mask
 *     all 60 argument-bearing sites ............. 97, FOUR LONG
 *     20 pins, descending fill .................. 46
 *     12 pins, whole-value fill ................. 190, relocations differ
 *     --cflags -ffixed-r7 on the final C ........ 230 differing (tryc)
 *
 * THE `-ffixed-r7` TELL FIRES AND IS WRONG, AND THE RECORDED DISCRIMINATOR
 * FOR IT NEEDS A THIRD ARM.  docs/elevation.md says: count the ROM's
 * callee-saved registers against yours; SAME COUNT, ROTATED -> the flag; ONE
 * MORE -> a live-range conflict.  This function is same count, rotated, and
 * the flag is emphatically wrong -- r7 is one of the seven the ROM spends, so
 * reserving it leaves six where the ROM has seven (258 and 259 instructions
 * against 257, first difference at index 0, on the unpinned draft AND on the
 * finished C).  SAME COUNT + ROTATED does not imply the flag; it implies only
 * that the MEMBERSHIP is wrong, and the cheap cure is to check first whether
 * the rotation is a CSE-class difference -- which it was.
 *
 * `iwram_3001ebc` is `unsigned char *` and both stores are WORDS at a
 * register offset (0x1c0 exceeds the `str rd, [rn, #imm5*4]` range, which is
 * why the ROM holds the offset in r7 at all):
 *     *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x201;
 *
 * LANDING NEEDS A SPLIT.  The .s holds two functions and this is the SECOND,
 * so `python3 tools/split_s.py asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_c_a_c_c.s
 * OvlFunc_945_200e110` writes `..._c_c_a.s` (OvlFunc_945_200dd10, 0x410 bytes,
 * stays assembly) and `..._c_c_b.s` (this function), with no `_c` part.  The
 * whole object is 0x69c of .text with a ZERO-LENGTH .data and .bss.  Exactly
 * two checked-in lines name the .o and BOTH keep their `asm/` path after the
 * elevation:
 *   overlays/rom_7cb2c0/overlay.ld:94
 *       asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_c_a_c_c.o(.text)
 *   overlays/rom_7cb2c0/overlay.ld:104
 *       asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_c_a_c_c.o(.data)
 * Each becomes the two split stems in order (_a then _b).  Build via the
 * generic `asm/%.o: src/%.c` rule; add no explicit Makefile rule.
 *
 * Twelve register pins => fakematch; needs a fakematch.txt row
 *   OvlFunc_945_200e110  src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_c_a_c_c_b.c
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ClearFlag(int id);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8091e9c(int n);
extern int OvlFunc_945_200cfa8(int a, int b);
extern void OvlFunc_945_200c86c(int a);
extern void OvlFunc_945_200c880(int a, int b);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void OvlFunc_945_200e3ac(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PINR1 register int q1 __asm__("r1")
#define PINR12 PINR1; register int q2 __asm__("r2")

void OvlFunc_945_200e110(void)
{
    int a0, a1, a2, a3;

    a0 = OvlFunc_945_200cfa8(0, 0);
    a1 = OvlFunc_945_200cfa8(1, 0);
    a2 = OvlFunc_945_200cfa8(2, 0);
    a3 = OvlFunc_945_200cfa8(3, 0);
    __CutsceneStart();
    OvlFunc_945_200c8e8(0xa, 0, 0);
    OvlFunc_945_200c890(8, 0xec << 1, 0x90, 0xa0 << 7);
    OvlFunc_945_200c890(0x1b, 0xcc << 1, 0x8e, 0xc0 << 6);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x201;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __Func_80925cc(0x1b, 1);
    __MessageID(0x1f78);
    OvlFunc_945_200c86c(0x1b);
    __Func_809259c(a0, 2);
    __Func_809259c(a1, 2);
    __Func_809259c(a2, 2);
    __Func_80925cc(a3, 2);
    __CutsceneWait(0x14);
    __Func_8092adc(a0, 0, 0);
    { PINR1; q1 = 0x80; q1 <<= 8; __Func_8092adc(a1, q1, 0); }
    __Func_8092adc(a2, 0, 0);
    { PINR1; q1 = 0x80; q1 <<= 8; __Func_8092adc(a3, q1, 0x28); }
    { PINR12; q1 = 0x80; q1 <<= 9; q2 = 0x80; q2 <<= 8; __MapActor_SetSpeed(a0, q1, q2); }
    { PINR12; q1 = 0x80; q1 <<= 9; q2 = 0x80; q2 <<= 8; __MapActor_SetSpeed(a1, q1, q2); }
    { PINR12; q1 = 0x80; q1 <<= 9; q2 = 0x80; q2 <<= 8; __MapActor_SetSpeed(a2, q1, q2); }
    { PINR12; q1 = 0x80; q1 <<= 9; q2 = 0x80; q2 <<= 8; __MapActor_SetSpeed(a3, q1, q2); }
    __Func_809218c(a0, 0xeb << 1, 0xac);
    __Func_809218c(a1, 0xcd << 1, 0xac);
    { PINR1; q1 = 0xeb; q1 <<= 1; __Func_809218c(a2, q1, 0xcc); }
    { PINR1; q1 = 0xcd; q1 <<= 1; __Func_80921c4(a3, q1, 0xcc); }
    __MapActor_SetAnim(a0, 1);
    __MapActor_SetAnim(a1, 1);
    __MapActor_SetAnim(a2, 1);
    __Func_8092adc(a1, 0xd0 << 8, 0);
    __Func_8092adc(a0, 0xb0 << 8, 0);
    { PINR1; q1 = 0xd0; q1 <<= 8; __Func_8092adc(a3, q1, 0); }
    { PINR1; q1 = 0xb0; q1 <<= 8; __Func_8092adc(a2, q1, 0x14); }
    __Func_80925cc(0x1b, 1);
    OvlFunc_945_200c86c(0x1b);
    __MapActor_SetAnim(a0, 3);
    __MapActor_SetAnim(a1, 3);
    __MapActor_SetAnim(a2, 3);
    __MapActor_DoAnim(a3, 3);
    OvlFunc_945_200c86c(0x1b);
    __MapActor_SetAnim(a0, 3);
    __MapActor_SetAnim(a1, 3);
    __MapActor_SetAnim(a2, 3);
    __MapActor_DoAnim(a3, 3);
    __Func_8092adc(0x1b, 0, 0);
    OvlFunc_945_200c880(0, 0x80 << 8);
    __MapActor_DoAnim(0, 3);
    __MapActor_DoAnim(0x1b, 3);
    { PIN1; q0 = 0x1b; __MapActor_SetSpeed(q0, 0x80 << 9, 0x80 << 8); }
    __Func_80921c4(0x1b, 0xcc << 1, 0x84);
    { PIN1; q0 = 0x1b; __Func_80921c4(q0, 0xde << 1, 0x84); }
    __MapActor_SetPos(0x1b, 0, 0);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
    __MapTransitionOut();
    __WaitMapTransition();
    OvlFunc_945_200e3ac(0x92c, 0x935);
    OvlFunc_945_200e3ac(0x917, 0x99 << 4);
    __ClearFlag(0x8a << 4);
    __Func_8091e9c(0xa);
}
