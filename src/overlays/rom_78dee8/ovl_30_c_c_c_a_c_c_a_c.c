// fakematch
/* ovl_30_c_c_c_a_c_c_a_c.c  --  OvlFunc_895_2008f8c  --  0x02008f8c
 *   [asm/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_a_c.s, the ONLY function in
 *    that .s -- no split is needed and none is proposed]
 *
 * 645 instructions of straight-line cutscene script: 174 calls, ONE if/else
 * diamond on __Func_8091c7c, SIX null-guarded __MapActor_GetActor blocks and
 * one mid-function `.pool_aligned` dump.  Newly visible: the substring bug
 * fixed in decd6fa9 hid this file from candidate ranking, so nobody had looked
 * at it.  Selected on two independent signals -- 31 shared symbols with
 * src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_a_a.c and ZERO r8-r11 traffic
 * anywhere in the reference.  VERDICT:
 *
 *   OK OvlFunc_895_2008f8c -- 1680 bytes, 649 encodings and 175 relocations identical
 *
 * ...against BOTH the scratch reference and the REAL asm/ path.  The template's
 * levers were treated as SUFFICIENT NOT NECESSARY and every one re-measured;
 * the table at the bottom is the result.
 *
 * NO FLAG GROUP IS INVOLVED, AND THAT IS A POSITIVE FINDING, NOT AN OMISSION.
 * No Makefile wildcard matches `asm/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_a_c`
 * -- the directory's only explicit rule is line 752, for the unrelated stem
 * ovl_30_c_c_a_c_b -- so the generic `asm/%.o: src/%.c` at Makefile:146 applies
 * and the TU takes the tree default -O2.  objcmp printed no "(built with: ...)"
 * line when screened through the real asm/ path, which is the check that caught
 * the rom_7ef4f4 O1-wildcard trap, and it is clean here.  Nothing to add to the
 * Makefile.
 *
 * A PIN FUNCTION, AND THE PROLOGUE SAYS SO BY CONTENT.  `push {lr}` alone, and
 * nothing is kept across a call anywhere in 645 instructions, so named locals
 * cannot be what the source had and only pins work.  Plain C opens
 * `push {r5,r6,r7,lr}` (b5e0) against the ROM's b500, 539 of 649 differing.
 *
 * THE LENGTH TELL SHOWS ALL THREE OUTCOMES INSIDE THIS ONE FUNCTION, which is
 * the strongest form of the recorded "length is not evidence, read the diff
 * text".  Plain C is 1668 bytes against 1680 -- TWELVE SHORT, six instructions
 * fewer, because the constants cse_main commons are worth more than the
 * three-register prologue costs.  But dropping the single pin at site 10 or 11
 * from the final set is 1684 -- FOUR LONG.  And sixteen of the 48 required pins
 * (12, 16, 18, 34, 35, 55, 62, 65, 73, 74, 86, 99, 106, 109, 112, 126) measure
 * EQUAL length while still differing in 23 to 412 places.  Same function, same
 * transformation family, all three signs.
 *
 * FIFTY-TWO PIN CANDIDATES, FORTY-EIGHT REQUIRED.  The candidate set is the 50
 * sites whose argument list carries a constant the ROM builds more than once
 * and that is not a bare `mov #imm8` (gen.py computes it; `gen.py --info`
 * prints it), plus TWO ordering-only sites, 104 and 120.  With all 52 the
 * function is already exact, so the sweep is pure minimisation: every site
 * stripped individually under objcmp, greedily, re-testing after each drop and
 * running to a fixpoint, FROM BOTH ENDS.  Both directions survive the same 48.
 * The four that fall are sites 42, 68, 69 and 148, and all four are the plain
 * recorded "one pin at the first use covers the later ones" -- 42 is a repeat
 * of site 18 with an identical argument list, 68 a later 0x4ccc, 69 a later
 * -0x1, 148 a later 0xa0 << 8.  They are INERT SCAFFOLDING and do not ship.
 *
 * NEW: THE RESIDUE'S RELOCATION LINE SORTS THE TWO PIN JOBS, CLEANLY AND WITH
 * NO MIDDLE.  Dropping each of the 48 required pins one at a time partitions
 * them perfectly:
 *
 *   10 pins  (104, 116, 120, 128, 129, 130, 131, 137, 141, 147)
 *            -- EXACTLY 2 encodings, SIZE silent, RELOCATIONS silent
 *   38 pins  -- 23 to 600 encodings, RELOCATIONS ALWAYS differ,
 *               SIZE differs on 22 of the 38 and is silent on 16
 *
 * The mechanism is why it is exact rather than a tendency.  A lost
 * rematerialisation either changes the instruction count or swaps a mov+lsl for
 * a pooled load, and either way every following `bl` moves in the object, so
 * every relocation OFFSET moves.  A pure argument-order transposition moves no
 * byte offset at all.  So `XX RELOCATIONS differ` is a BINARY read of "did the
 * stream change lengthwise anywhere ahead of here", and that is exactly the
 * CSE-versus-ordering question.  SIZE is NOT the discriminator -- it is silent
 * on sixteen genuine CSE losses.  The template's write-up says "the residue's
 * SHAPE is the diagnostic that separates the two jobs"; this sharpens it to a
 * line objcmp already prints, so no diff has to be read to classify a pin.
 *
 * NEW: NOMINATE BY CALL-SITE FAMILY AS WELL AS BY REPEATED CONSTANT, AND THE
 * RESIDUE ROUND DISAPPEARS.  The recorded candidate rule -- sites carrying a
 * repeated expensive constant -- is a CSE rule, and it systematically misses the
 * SINGLETON member of an otherwise-uniform family of call sites, which needs the
 * same pin as its siblings for ORDERING.  Here __Func_8092adc is called 31 times
 * with the shape (imm, shifted-byte, imm); 30 of the 31 carry a repeated shift
 * and are nominated, and site 104 -- `__Func_8092adc(8, 0xc0 << 6, 0xa)`, the
 * one whose shifted byte is used ONCE -- is not.  It was worth 2 encodings, and
 * no predicate over its own argument list could have found it.
 *
 * Adding "same callee AND same argument shape as an already-nominated site"
 * nominates exactly three extra sites (52, 59, 104), and combining that with the
 * recorded "write __Func_8092c40 descending on sight" nominates 120 as well.
 * MEASURED: that 54-site set, with no residue read at all, is
 *
 *   OK OvlFunc_895_2008f8c -- 1680 bytes, 649 encodings and 175 relocations identical
 *
 * and minimises to the same 48.  So the whole first-residue round this function
 * cost is avoidable on the next one.  Two of the three extras (52, 59) are
 * over-nominations the sweep removes, which is the right way for a nomination
 * rule to be wrong.
 *
 * SITE 120 IS __Func_8092c40 WANTING THE DESCENDING FILL -- the recorded lever's
 * ninth function, and here it is the whole of the second half of the residue.
 * `__Func_8092c40(1, 0)` wants `q1 = 0; q0 = 1;`.  Both its arguments are bare
 * `mov #imm8`, so it carries no expensive constant at all and the CSE-based
 * candidate rule cannot see it; the site is reachable only from the residue or
 * from the callee's name.  The recorded "binary, not graded" tell holds exactly:
 *
 *   descending at 120  -- exact                       (this file)
 *   ascending at 120   -- 2 encodings
 *   no pin at 120      -- the SAME 2 encodings
 *
 * SITES 104 AND 120 ARE INDEPENDENT, MEASURED AS A FULL 2x2.  The standing
 * warning that a one-lever-at-a-time sweep can call a lever harmful was checked
 * rather than assumed: neither = 4 encodings, 104 alone = 2, 120 alone = 2,
 * both = 0.  Perfectly additive, no interaction, and the one-at-a-time reading
 * happens to be right here.  It cost one sweep to know that.
 *
 * THE ELSE ARM RE-PINS THREE VALUES WHOSE FIRST USES ARE UPSTREAM OF THE
 * DIAMOND, which is the recorded "pin the first use IN EACH REGION the diamonds
 * cut the function into" with a single diamond and four regions.  Sites 128,
 * 129 and 130 pin 0xc0 << 8, 0xe0 << 8 and 0xa0 << 8, all three of which are
 * already pinned before the diamond (37, 24, 25); dropping the three costs 6
 * encodings.  The post-join region behaves the same way at 141 and 147 -- and
 * NOT at 148, which is the recorded "or nothing" case: 0xa0 << 8 has six uses
 * in this function and its one post-join use needs no pin, because gcc
 * rematerialises it unaided.  Being repeated and expensive makes a value a
 * CANDIDATE, not a requirement.
 *
 * SITES 126 AND 137 ARE BOTH REQUIRED FOR ONE VALUE IN ONE REGION, and the new
 * relocation test says why without reading a diff.  Both are 0x80 << 8 inside
 * the else arm, 126 first.  Dropping 137 is 2 encodings with relocations
 * SILENT: the CSE job is done by 126 and 137's pin is buying only the ROM's
 * `mov r1 / mov r0 / lsl r1 / mov r2` interleave.  Dropping 126 is 51
 * encodings with relocations DIFFERING: that one is the CSE job.  So the CSE
 * job is per-REGION and the ordering job is per-SITE, and a "later use" pin can
 * still be required for the second reason.
 *
 * _MSG_fe0 IS LOAD-BEARING AND ITS NEIGHBOUR IS NOT, which is a clean
 * confirming instance of the recorded "a _MSG_ symbol's tell is a length
 * difference, and only for shifted bytes".  The two arms of the one diamond
 * call __MessageID with 0xfe0 and 0xfe1, ADJACENT IDS.  0xfe0 is 0x7f << 5, so
 * gcc would synthesise it as mov+lsl and the ROM's pooled load means a symbol:
 * `extern int _MSG_fe0;` with `(int) (&_MSG_fe0)`, already defined at
 * message.sym:35.  0xfe1 is odd, and 0xfd6 (site 76) is 0x7eb << 1 with a base
 * over 0xff, so neither is a shifted byte, both pool unaided, and neither wants
 * a symbol.  Writing 0xfe0 as the bare literal measures 191 encodings, 1676
 * bytes against 1680, and relocations differing -- the R_ARM_ABS32 disappears.
 * The question really is "can gcc build it with mov+lsl", not "is this an id".
 *
 * THE SIX GETACTOR BLOCKS COME IN TWO WIDTHS, and the ROM says which.  The
 * three at the bottom are the template's shape verbatim --
 * `p = __MapActor_GetActor(0); if (p != 0) __MapActor_TravelTo(slot,
 * *(short *)(p + 0xa), *(short *)(p + 0x12));` -- `ldrsh` through a `mov r3`
 * index.  The three at the TOP feed __MapActor_SetPos instead and the ROM uses
 * plain `ldr r1, [r0, #8]` / `ldr r2, [r0, #0x10]`, so they are
 * `*(int *)(p + 8)` and `*(int *)(p + 0x10)`: the same two struct fields read
 * WHOLE rather than by their high halves.  SetPos takes the 16.16 position,
 * TravelTo the integer part.  `bne` past the diamond means the fallthrough is
 * the `if` body, so its guard is `== 0`.
 *
 * THE SHIFTED-BYTE SPELLING IS INERT, as on both template functions.  All 45
 * `mov #n / lsl #k` builds are written the ROM's way for documentary value;
 * respelling every one as a whole value (`0xe000`, `0x1b80000`, `0x3000`) is
 * BYTE-IDENTICAL.  Every fill here has its shift last, so sched2 lands it
 * unaided from the one uniform ascending spelling -- including site 65, where
 * the ROM emits `lsl r0` LAST of seven setup instructions, and site 104, where
 * it emits the `lsl` BETWEEN two movs.  The recorded "where the ROM puts the
 * shift between two movs the source statement must sit there too" exception did
 * NOT have to be invoked; the pin's ascending fill reproduces that order by
 * itself, which agrees with the template's bound on that exception.
 *
 * TRANSCRIBING THE ROM'S EMITTED ARGUMENT ORDER IS WRONG, again: filling each
 * pinned site in the order the ROM's `mov`s appear measures 77 differing
 * against 0 for the uniform ascending fill.  UNIFORM ASCENDING IS CORRECT, not
 * merely cheaper.
 *
 * ALL FIFTEEN POOLED VALUES ARE BARE LITERALS EXCEPT _MSG_fe0.  0x9999, 0x4ccc,
 * 0x13333, 0x2666, 0x26666, 0x101, 0xfd6, 0xfe1, 0x804, 0x12f, 0x6310000,
 * 0x6550000, 0x6b60000 and 0x6840000: none is a shifted byte gcc could build
 * with mov+lsl (the four 0x6xx0000 have bases over 0xff), so each pools unaided.
 * 175 relocations identical -- 174 `bl` and the one R_ARM_ABS32 for _MSG_fe0.
 * Nothing belongs in const.sym for this function.
 *
 * TRYC WAS NOT USED AS A SCREEN.  The reference keeps its literal pool INSIDE
 * the function (`b .L1400` / `.pool_aligned` / `.L1400:` after site 117), which
 * is precisely the case tryc's `=value` normalisation is blind to, and tryc
 * infers its reference from a src/ path this candidate does not have.  Every
 * number above is objcmp, run against the scratch reference and re-run against
 * the real asm/ path.
 *
 * LANDING.  The .s holds ONE function, so this file replaces it whole and no
 * split is required.  EXACTLY ONE linker-script line names the object, matched
 * on full path -- overlays/rom_78dee8/overlay.ld:33,
 * `asm/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_a_c.o(.text)` -- and it needs NO
 * EDIT, because objects built from src/ still land under asm/ via
 * `asm/%.o: src/%.c`.  The trap of a `.ld` naming a section the object does not
 * have was checked and does NOT fire here: that overlay.ld's `.data` (line 44)
 * and `.bss` (line 48) lines name a DIFFERENT object,
 * asm/overlays/rom_78dee8/ovl_30_c_c_c_c_c.o, and this object's own .data and
 * .bss are both zero-length and named by no line anywhere.  `grep -rn
 * 'ovl_30_c_c_c_a_c_c_a_c\.o'` over every .ld and the Makefile returns that one
 * line and nothing else; the same basename in overlays/rom_7c097c/overlay.ld
 * and overlays/rom_79c0c4/overlay.ld is a DIFFERENT full path and a different
 * overlay.  So the landing is: add this .c, delete
 * asm/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_a_c.s, change nothing else.
 *
 * Reproduce: scratch_elev/b239/f2008f8c/gen.py emits this file from a pin set
 * given as reference call-site numbers (calls.txt / body.txt are the
 * transcription; extract.py and mkbody.py built them).  minimise.py runs the
 * greedy sweep in either direction, variants.py the measured-worse table and
 * sweep.py an arbitrary spec list, each inside ONE container invocation.
 *
 * MEASURED WORSE (all against the same reference, 1680 bytes / 649 encodings):
 *
 *   final 48 pins, scratch ref                OK
 *   final 48 pins, REAL asm/ path             OK
 *   all 52 pin candidates                     OK  (4 inert -- dropped)
 *   family-heuristic 54 pins, no residue read  OK  (see the NEW note above)
 *   whole-value consts (no mov/lsl)           OK  (spelling inert)
 *   drop site 104 (lsl between movs)          2 encodings
 *   drop site 120 (8092c40 descending)        2 encodings
 *   8092c40 ASCENDING fill at 120             2 encodings (== unpinned)
 *   drop site 137 (2nd 0x80<<8 in else arm)   2 encodings, relocations SILENT
 *   drop site 116 (4th 0x80<<7)               2 encodings, relocations SILENT
 *   drop BOTH 104 and 120                     4 encodings (additive, 2x2 run)
 *   drop 128,129,130 (else-arm re-pins)       6 encodings
 *   drop site 65 (0xdb<<19, lsl r0 last)      23 encodings, push {r5,lr}
 *   drop site 126 (1st 0x80<<8 in else arm)   51 encodings, relocations DIFFER
 *   ROM's emitted arg order, not ascending    77 encodings
 *   _MSG_fe0 as the bare literal 0xfe0        191 encodings, 1676 vs 1680
 *   0xe0<<8: pin only the 1st use (24)        294 encodings, push {r5,lr}
 *   0xc0<<8: pin only the 1st use (26)        465 encodings, 1668 vs 1680
 *   no pins at all                            539 encodings, 1668 vs 1680,
 *                                             push {r5,r6,r7,lr}
 */

extern int _MSG_fe0;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_895_2008f8c(void)
{
    unsigned char *p;

    __CutsceneStart();
    __MapTransitionIn();
    __WaitMapTransition();
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(0x8, *(int *)(p + 8), *(int *)(p + 0x10));
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(0x5, *(int *)(p + 8), *(int *)(p + 0x10));
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(0x1, *(int *)(p + 8), *(int *)(p + 0x10));
    { PIN3; q0 = 0x8; q1 = 0x9999; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x5; q1 = 0x9999; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x9999; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(0x1, 0x2);
    __MapActor_SetAnim(0x5, 0x2);
    __MapActor_SetAnim(0x8, 0x2);
    { PIN3; q0 = 0x1; q1 = -0x10; q2 = 0x0;
      __Func_809228c(q0, q1, q2); }
    __Func_809228c(0x5, 0x10, 0x0);
    { PIN3; q0 = 0x8; q1 = 0x0; q2 = -0x10;
      __Func_809228c(q0, q1, q2); }
    __MapActor_WaitMovement(0x8);
    __MapActor_SetAnim(0x8, 0x1);
    __MapActor_SetAnim(0x0, 0x0);
    __MapActor_SetAnim(0x1, 0x0);
    __MapActor_SetAnim(0x5, 0x0);
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x5; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0xc0 << 8; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x80 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x5, 0x0, 0x0);
    { PIN3; q0 = 0x0; q1 = 0x80 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x80 << 8; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x80 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x5; q1 = 0x80 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x0, 0x0, 0x0);
    { PIN3; q0 = 0x8; q1 = 0x80 << 7; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x5; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0xc0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x8, 0x2);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x8, 0x2);
    __Func_809228c(0x8, 0x0, -0x10);
    __MapActor_WaitMovement(0x8);
    __MapActor_SetAnim(0x8, 0x1);
    __CutsceneWait(0x6);
    { PIN3; q0 = 0x8; q1 = 0x80 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0xc0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x8, 0x2);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x8, 0x2);
    __Func_809228c(0x8, 0x0, -0x20);
    __MapActor_WaitMovement(0x8);
    __MapActor_SetAnim(0x8, 0x1);
    { PIN2; q0 = 0x80 << 10; q1 = 0x80 << 7;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0x6310000; q1 = -0x1; q2 = 0x96 << 16; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0xa);
    __Func_80933d4(0x13333, 0x2666);
    { PIN4; q0 = 0x6550000; q1 = -0x1; q2 = 0xc8 << 15; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    { PIN4; q0 = 0x6b60000; q1 = -0x1; q2 = 0xc8 << 15; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __MapActor_SetAnim(0x8, 0x1);
    { PIN4; q0 = 0xdb << 19; q1 = -0x1; q2 = 0x96 << 16; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x28);
    __Func_80933d4(0x26666, 0x4ccc);
    __Func_80933f8(0x6840000, -0x1, 0x80 << 17, 0x1);
    __Func_8093530();
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x5; q1 = 0xa0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_Emote(0x1, 0x101, 0x14);
    __MessageID(0xfd6);
    __Func_8093040(0x1, 0x0, 0xa);
    { PIN3; q0 = 0x8; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(0x8, 0x2);
    __Func_8093040(0x8, 0x0, 0xa);
    __Func_809259c(0x0, 0x2);
    __Func_809259c(0x1, 0x2);
    __Func_809259c(0x5, 0x2);
    { PIN2; q0 = 0x0; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x1; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x5; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x28);
    __Func_80925cc(0x8, 0x2);
    __CutsceneWait(0x14);
    __ActorMessage(0x8, 0x0);
    __MapActor_DoAnim(0x8, 0x4);
    __Func_8093040(0x8, 0x0, 0xa);
    __Func_8092848(0x0, 0x5, 0x0);
    __CutsceneWait(0x28);
    __Func_809259c(0x0, 0x1);
    __Func_80925cc(0x5, 0x1);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x5; q1 = 0xa0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x5, 0x2);
    __Func_8093040(0x5, 0x0, 0xa);
    __MapActor_DoAnim(0x8, 0x4);
    __Func_8093040(0x8, 0x0, 0xa);
    { PIN3; q0 = 0x8; q1 = 0xc0 << 6; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x28);
    { PIN3; q0 = 0x8; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x8, 0x1);
    __CutsceneWait(0xa);
    { PIN2; q0 = 0x8; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN3; q0 = 0x8; q1 = 0x80 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0xc0 << 8; q2 = 0x3c;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x8, 0x2);
    __CutsceneWait(0xa);
    __Func_8093040(0x8, 0x0, 0xa);
    { PIN3; q0 = 0x8; q1 = 0x80 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_Jump(0x8, 0x2, 0x14);
    __Func_8093040(0x8, 0x0, 0x28);
    __Func_8092adc(0x1, 0x0, 0x14);
    { PIN2; q1 = 0x0; q0 = 0x1;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __MessageID((int) (&_MSG_fe0));
        __Func_80925cc(0x1, 0x1);
        __Func_8093040(0x1, 0x0, 0xa);
    } else {
        __MessageID(0xfe1);
        { PIN3; q0 = 0x5; q1 = 0x80 << 8; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        __Func_8093040(0x5, 0x0, 0xa);
        { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0x5; q1 = 0xa0 << 8; q2 = 0x3c;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0x0; q1 = 0x81 << 1; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __Func_80925cc(0x1, 0x1);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0x1, 0x3);
        __CutsceneWait(0xa);
        __Func_8092adc(0x1, 0x0, 0x0);
        { PIN3; q0 = 0x5; q1 = 0x80 << 8; q2 = 0x1e;
          __Func_8092adc(q0, q1, q2); }
        __Func_80925cc(0x1, 0x1);
        __CutsceneWait(0xa);
        __Func_8093040(0x1, 0x0, 0xa);
    }
    { PIN3; q0 = 0x8; q1 = 0x9999; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(0x8, 0x2);
    __Func_809228c(0x8, 0x0, 0x30);
    __MapActor_WaitMovement(0x8);
    __MapActor_SetAnim(0x8, 0x1);
    __CutsceneWait(0x6);
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x5, 0xa0 << 8, 0x0);
    __MapActor_WaitMovement(0x8);
    __MapActor_SetAnim(0x8, 0x1);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x5, 0x3);
    __MapActor_DoAnim(0x0, 0x3);
    __CutsceneWait(0x6);
    __MapActor_SetAnim(0x1, 0x2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(0x1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_SetAnim(0x5, 0x2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(0x5, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_SetAnim(0x8, 0x2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(0x8, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x8);
    __MapActor_SetPos(0x1, 0x0, 0x0);
    __MapActor_SetPos(0x5, 0x0, 0x0);
    __MapActor_SetPos(0x8, 0x0, 0x0);
    __MapActor_SetAnim(0x8, 0x1);
    __MapActor_SetAnim(0x1, 0x1);
    __MapActor_SetAnim(0x5, 0x1);
    __SetFlag(0x804);
    __ClearFlag(0x12f);
    __CutsceneEnd();
}
