/* ovl_30_a_c_c_c_c_c_c_a_a.c  --  BOTH functions of the original TU.
 *
 * The batch-231 split cut this .s in two so that OvlFunc_965_2009238 could land
 * while OvlFunc_965_2009b10 was still asm.  Both are elevated now, so the split
 * is gone: this one translation unit is byte-identical to the WHOLE original
 * two-function ovl_30_a_c_c_c_c_c_c_a_a.s, and the two overlay.ld lines it
 * needed collapse back to the single one they replaced.  Each function's
 * write-up follows, in file order.
 */

/* OvlFunc_965_2009238  --  0x02009238
 *   [asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_a_a.s, 1st of 2 -- the split
 *    this line used to ask for has since COLLAPSED, see the banner above]
 *
 * 853 instructions of straight-line cutscene script: 251 calls, three
 * if/else diamonds on __Func_8091c7c and three null-guarded
 * __MapActor_GetActor blocks.  Same shape and the same levers as
 * OvlFunc_962_2008240 (src/overlays/rom_7ec19c/ovl_30_c_c_a.c), which was the
 * template.  VERDICT:
 *
 *   OK OvlFunc_965_2009238 -- 2264 bytes, 866 encodings and 254 relocations identical
 *
 * THE FLAGS ARE THE FIRST THING TO GET RIGHT, AND THE MAKEFILE SAYS THE WRONG
 * ONE.  `asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o` applies O1_CFLAGS and
 * its stem captures this TU, so both tryc and objcmp screen it at -O1 when the
 * reference is given by its real asm/ path.  Measured: 286 differing at -O1
 * against BYTE-IDENTICAL at -O2, on exactly the same source.  This is the
 * THIRD file in this one directory the wildcard is wrong for -- the Makefile
 * already carries explicit -O2 overrides for ovl_30_a_c_c_c_c_c_c_a_b and
 * ovl_30_a_c_c_c_c_c_c_c_c_c_b for the same reason.  LANDING THIS FUNCTION
 * REQUIRES A FOURTH SUCH OVERRIDE for whatever the split names the .c piece;
 * without it the screen is green and the build is red.  All measurements below
 * were taken through a scratch copy of the single function (scratch paths match
 * no Makefile rule, so they get the tree default -O2).
 *
 * A PIN FUNCTION, AND THE PROLOGUE SAYS SO.  `push {lr}` alone: nothing is kept
 * across a call anywhere, so named locals cannot be what the source had and
 * only pins work.  Plain C is 2268 bytes against 2264 and 726 of 866 differ,
 * opening at `push {r5,r6,r7,lr}` (b5e0) against the ROM's `push {lr}` (b500)
 * -- cse_main commons the repeated multi-instruction constants into pseudos
 * that straddle `bl` and gcc spills them into r5-r7.  Here the recorded
 * "LONGER proves constant-CSE" length tell DOES fire (+4 bytes), unlike the
 * template where the widened prologue exactly paid for the removed
 * rematerialisations.
 *
 * THE THIRTEEN REPEATED EXPENSIVE CONSTANTS, by use count:
 * 0x80 << 8 (14 -- 6 as __MapActor_SetSpeed's r2 and 8 as __Func_8092adc's r1),
 * 0xc0 << 8 (9), 0x80 << 7 (7), 0x80 << 9 (6), 0x81 << 1 (4), 0x101 (3, pool),
 * 0x13333 (3), 0x9999 (3), 0xa4 << 1 (3), 0x107 (2), 0x14ccc (2), 0xa666 (2),
 * 0xac << 1 (2), 0x9c << 1 (2), 0x8c << 17 (2).  Every 8-bit `mov` constant is
 * rematerialised for free and needs nothing.
 *
 * FIFTY-SIX PIN CANDIDATES, FORTY-FIVE REQUIRED.  The candidate set is the 50
 * sites carrying one of those values, plus six ORDERING-ONLY sites found in the
 * first residue (see below).  Every one of the 56 was then stripped
 * individually under objcmp, greedily, re-testing after each drop and running
 * to a fixpoint -- and the sweep was run from BOTH ends of the list, which
 * agreed on the same surviving 45.  The eleven that fall are sites 27, 62, 74,
 * 77, 104, 110, 122, 173, 190, 220 and 230 (numbering is the ordinal of the
 * `bl` in the reference; see scratch_elev/b231/f2009238/calls.txt).  Ten of the
 * eleven are a LATER use of a value whose earlier use is pinned, which is the
 * recorded "one pin at the first use covers the later ones".  The eleventh is
 * not, and it is the interesting one.
 *
 * 0x107 REVERSES THE POLARITY OF THE FIRST-USE RULE -- a SECOND instance of the
 * case recorded under "WHAT A PIN IS FOR, NOT WHERE THE BLOCKS ARE"
 * (OvlFunc_952_2008674).  Its two uses are __MapActor_Emote sites 62 and 166,
 * and they straddle two whole if/else diamonds, so gcc never commons them at
 * all.  Neither pin has any CSE work to do; the pin at 166 is buying pure
 * ARGUMENT ORDERING, because the ROM emits that site as
 * `mov r2,#0x32 / mov r0,#0xc / ldr r1,=0x107` and gcc's own order is
 * `mov r2 / ldr r1 / mov r0`.  So:
 *
 *   pin 166 only  -- exact                 (this file)
 *   pin 62 only   -- 2 encodings, an adjacent transposition at index 538
 *   pin neither   -- the SAME 2 encodings, at the same index
 *   pin both      -- exact, and 62 is therefore inert scaffolding: dropped
 *
 * The residue's shape is the diagnostic that separates the two jobs: the pool
 * load is PRESENT in both streams and merely transposed, so this is ordering,
 * not a missing rematerialisation.  "Pin the first use" is a rule about the CSE
 * job; it says nothing about the ordering job, and a minimiser that assumes it
 * would have kept the inert pin and dropped the required one.
 *
 * THE SIX ORDERING-ONLY PINS.  With all 50 constant sites pinned the residue was
 * 13 encodings at six sites, none of them a CSE victim: site 7
 * (__Func_80921c4, r0 wanted before the two `lsl`s), site 88
 * (__MapActor_Emote(0xe, 0x105, 0x3c), r0 before the pool load), site 141
 * (__MapActor_Emote(0xe, 0x80 << 1, 0x28), r0 between the `mov` and the `lsl`)
 * and the three __Func_8092c40 sites.  All four constants there are
 * SINGLE-USE, so no pin of theirs can be about CSE.  Adding them uniform
 * ascending (descending at 8092c40) took 13 to 0.
 *
 * __Func_8092c40 WANTS THE DESCENDING FILL -- the seventh function, and the
 * first with MORE THAN ONE such site: all three of sites 101, 142 and 179 want
 * `q1 = 0; q0 = N;`.  Writing them ascending costs exactly 6 encodings, two per
 * site, and the recorded "binary, not graded" tell holds: ascending here is
 * byte-identical to leaving all three unpinned.
 *
 * THE SHIFTED-BYTE SPELLING IS INERT ON THIS FUNCTION.  Every `mov #n / lsl #k`
 * build is written the ROM's way (`0x80 << 9`, `0xc0 << 8`, `0x8c << 17`, ...)
 * for documentary value, but respelling all 44 of them as whole values
 * (`0x10000`, `0xc000`, `0x1180000`) is BYTE-IDENTICAL.  That is consistent
 * with the recorded scope of the exception -- the spelling matters when the ROM
 * puts the `lsl` BETWEEN two movs and the source statement has to sit there
 * too.  Every fill here has its shift last, so sched2 lands it unaided, and it
 * reproduces all four of the ROM's transposed emitted orders from the one
 * uniform ascending spelling.
 *
 * ALL EIGHT POOLED VALUES ARE BARE LITERALS.  0x988, 0x98a, 0x2702, 0x101,
 * 0x105, 0x107, 0x13333, 0x9999, 0x14ccc and 0xa666 need no symbol: none is a
 * shifted byte gcc could build with mov+lsl, so each pools unaided.  objcmp
 * reports 254 relocations identical; the only non-`bl` relocations are the
 * three R_ARM_ABS32 iwram_3001ebc.  Nothing belongs in const.sym or message.sym
 * for this function.  Note 0x988 and 0x98a are two DISTINCT values, so the
 * "two symbols of equal value" caveat does not arise.
 *
 * THE THREE IF/ELSE DIAMONDS are the template's shape verbatim: the
 * `iwram_3001ebc` counter bump appears in BOTH arms, in the ROM's two different
 * positions relative to __ActorMessage, and written that way plain C emits the
 * pointer-then-0xec<<1 build with no help.  `bne` past the block means the
 * fallthrough is the `if` body, so the guard is `== 0`.  The three
 * __MapActor_GetActor blocks are likewise the template's
 * `p = __MapActor_GetActor(0); if (p != 0) __MapActor_TravelTo(...)`.
 *
 * LANDING.  The .s holds TWO functions -- this one and OvlFunc_965_2009b10, a
 * 914-instruction cutscene of the same family that is NOT solved here -- so a
 * whole-file .c replacement is not available and the file must be split.  The
 * single linker-script line naming the .o is
 * overlays/rom_7ef4f4/overlay.ld:48.  After the split BOTH pieces still match
 * the O1 wildcard, so the .c piece needs the explicit -O2 rule described above.
 *
 * Reproduce: scratch_elev/b231/f2009238/gen.py emits this file from a pin set
 * given as reference call-site numbers; minimise.py runs the greedy sweep and
 * variants.py the measured-worse table, each inside ONE container invocation.
 */

/* OvlFunc_965_2009b10  --  0x02009b10
 *   [asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_a_a_c.s, the 2nd of the two
 *    functions the batch-231 split left behind]
 *
 * 914 instructions of straight-line cutscene script: 263 calls, TWO if/else
 * diamonds on __Func_8091c7c, three null-guarded __MapActor_GetActor blocks and
 * three mid-function `.pool_aligned` dumps.  Its file-sibling
 * OvlFunc_965_2009238 (src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_a_a_b.c)
 * was the template and its lever set carried over WHOLE -- but the levers were
 * re-measured, not transplanted, and three of them came out differently.
 * VERDICT:
 *
 *   OK OvlFunc_965_2009b10 -- 2396 bytes, 925 encodings and 265 relocations identical
 *
 * ...against BOTH the scratch reference and the real asm/ path.
 *
 * THE SIBLING'S FLAG WARNING NO LONGER APPLIES, AND ITS HEADER IS STALE ON THE
 * POINT.  That header says "LANDING THIS FUNCTION REQUIRES A FOURTH SUCH
 * OVERRIDE" for the `rom_7ef4f4/ovl_30_a_c_c_c_c_c%` O1 wildcard.  Commit
 * fe6b158f narrowed the wildcard at source instead of adding the override, in
 * the same batch, so the note describes a proposal that was not taken.  This TU
 * takes the tree default -O2, and screening through the real asm/ path now
 * agrees with the scratch copy exactly -- which is the check that would have
 * caught the trap in the first place, and is worth running on every member of
 * this directory regardless.
 *
 * A PIN FUNCTION, AND THE PROLOGUE SAYS SO.  `push {lr}` alone: nothing is kept
 * across a call anywhere, so only pins work.  Plain C opens
 * `push {r5,r6,r7,lr}` (b5e0) against b500, with 834 of 925 differing.
 *
 * THE LENGTH TELL FIRES SHORT HERE, WHERE THE SIBLING'S FIRED LONG.  Plain C is
 * 2356 bytes against the ROM's 2396 -- FORTY BYTES SHORT, because the constants
 * cse_main commons are worth more than the three-register prologue costs.  The
 * sibling, same family and same shape, measured +4 LONG on exactly this
 * transformation.  Two of the recorded tell's three outcomes, on two functions
 * of one family in one file: length is not evidence, read the diff text.
 *
 * SEVENTY-SIX PIN CANDIDATES, SIXTY-FIVE REQUIRED.  The candidate set is the 71
 * sites whose argument list carries a constant the ROM builds more than once
 * and that is not a bare `mov #imm8` (gen.py computes it; see gen.py --info),
 * plus five ORDERING-ONLY sites -- 75, 95, 145, 148, 153 -- read out of the
 * first residue.  With all 76 the function is already exact, so the sweep is
 * pure minimisation: every site stripped individually under objcmp, greedily,
 * re-testing after each drop and running to a fixpoint, FROM BOTH ENDS.  Both
 * directions surviving the same 65.  The eleven that fall are sites 11, 110,
 * 116, 128, 132, 186, 191, 197, 219, 225 and 261.
 *
 * Eight of the eleven are the recorded "one pin at the first use covers the
 * later ones".  The other three are the interesting ones, and they are two
 * different things.
 *
 * 0xa0 << 7 NEEDS NO PIN AT ALL -- the "or nothing" job, on a value with TWO
 * uses (sites 186 and 219) in one region.  Being repeated and expensive makes a
 * value a CANDIDATE, not a requirement; gcc rematerialises this one unaided.
 *
 * 0x80 << 6 REVERSES THE FIRST-USE POLARITY, AND DOES IT ON THE CSE JOB.  Four
 * uses -- 128, then 168, 191 and 197 -- with the if/else diamond at site 155
 * between the first and the rest.  Measured:
 *
 *   pin 168 only        -- exact                       (this file)
 *   pin 128 only        -- 307 differing, `push {r5,lr}` (b520)
 *   pin both            -- exact, so 128 is inert scaffolding: dropped
 *
 * The recorded reversal (OvlFunc_952_2008674's 0x107, and the sibling's second
 * instance of it) was a pure ORDERING pin either side of a join, where neither
 * site had CSE work to do.  This one is the opposite: the pin at 168 is doing
 * REAL constant-CSE work for the group 168/191/197 -- leaving it open puts the
 * value in a callee-saved register and widens the prologue -- while the lone
 * use at 128, upstream of the diamond, needs nothing.  So the reversal is a
 * property of the CSE REGION, not of the ordering job: the rule reads "pin the
 * first use IN EACH REGION the diamonds cut the function into", and the
 * earliest use in the function may not be in the region that needs a pin.
 *
 * THE FIVE ORDERING-ONLY PINS.  All five carry SINGLE-USE constants, so no pin
 * of theirs can be about CSE:
 *
 *   75, 95   __Func_8092304(0xa, 0, -0x28) / (0xa, 0, -0x20).  The ROM splits
 *            the `mov #0x28 / neg` pair around the other two arguments.  5
 *            encodings without them.
 *   145      __MapActor_Emote(0xe, 0x103, 0x32) -- r0 wanted before the pool
 *            load.  2 encodings.
 *   148      __Func_8092c40(0xe, 0) -- descending fill.  2 encodings.
 *   153      __Func_8092adc(2, 0x80 << 5, 0) -- the ROM puts the `lsl` BETWEEN
 *            two movs.  2 encodings.
 *
 * SITES 75/95 AND 153 ARE THE SAME LEVER, AND IT IS THE ONE-STATEMENT WHOLE-
 * VALUE FILL.  `q2 = -0x28;` and `q1 = 0x80 << 5;` each split into two
 * instructions AFTER expand, so the seed `mov` sits at depth 2 and every other
 * argument's `mov`, plus the trailing `lsl`/`neg`, sits at depth 1; sched takes
 * the depth-2 class first and breaks ties by argument order.  That reproduces
 * both the ROM's `mov r2 / mov r0 / mov r1 / neg r2` and its
 * `mov r1 / mov r0 / lsl r1 / mov r2` with NO source-level placement of the
 * shift -- so the recorded "where the ROM emits the shift BETWEEN two movs the
 * source statement must sit there too" exception did not have to be invoked
 * even though the ROM's shape is exactly that.  Read that exception as scoped
 * to the one-statement-per-instruction fill it was measured on.
 *
 * __Func_8092c40 IS NOT UNIFORM WITHIN ONE FUNCTION.  The sibling found all
 * three of its 8092c40 sites wanting the descending fill.  Here there are two:
 * site 148 wants descending (ascending costs exactly 2 encodings, and ascending
 * is byte-identical to leaving it unpinned -- the recorded "binary, not graded"
 * tell) and site 184, the same call with the same arguments, wants NO PIN AT
 * ALL.  The descending fill is a property of the SITE, not of the callee.
 *
 * SITE 98 IS THE `-1` TRIPLE, AND IT IS LOAD-BEARING.  `__Func_80933f8(-1, -1,
 * -1, 0)` -- the ROM builds -1 three times, `mov #1` x3 then `neg` x3, three
 * consecutive `neg rN, rN`.  Dropping that one pin costs 363 differing and a
 * `push {r5,lr}`.  This is a confirming instance of the recorded "The `-1`
 * triple is fully reachable", reached the same way (pin the argument registers,
 * let each assignment carry its own negation) with no extra work.
 *
 * THE SHIFTED-BYTE SPELLING IS INERT, as on the sibling.  All 78 `mov #n / lsl`
 * builds are written the ROM's way for documentary value; respelling every one
 * of them as a whole value is BYTE-IDENTICAL.
 *
 * TRANSCRIBING THE ROM'S EMITTED ARGUMENT ORDER IS WRONG, and by a lot: filling
 * each pinned site in the order the ROM's `mov`s appear measures 94 differing
 * against 0 for the uniform ascending fill.  Same result the sibling family has
 * every time it is measured.
 *
 * ALL SEVEN POOLED VALUES ARE BARE LITERALS.  0x989, 0x272f, 0x301, 0x101,
 * 0x103, 0x13333 and 0x9999: none is a shifted byte gcc could build with
 * mov+lsl, so each pools unaided and none wants a symbol.  265 relocations
 * identical; the only non-`bl` ones are the four R_ARM_ABS32 iwram_3001ebc, one
 * per arm of the two diamonds.
 *
 * THE DIAMONDS AND THE TRAVEL BLOCKS are the template's shapes verbatim.  `bne`
 * past the block means the fallthrough is the `if` body, so the guard is
 * `== 0`; the iwram_3001ebc counter bump appears in BOTH arms, in the ROM's two
 * different positions relative to __ActorMessage, and plain C emits the
 * pointer-then-0xec<<1 build unaided.  The three GetActor blocks are
 * `p = __MapActor_GetActor(0); if (p != 0) __MapActor_TravelTo(...)`.
 *
 * THE SPLIT COLLAPSES.  With both functions of the original TU elevated, the
 * batch-231 split is no longer needed: combined.c holds both in one translation
 * unit and, compiled at the tree default -O2, is byte-identical to the WHOLE
 * original two-function .s --
 *
 *   OK COLLAPSE -- combined TU: 4660 bytes, 1791 encodings and 519 relocations
 *   identical to the ORIGINAL two-function ovl_30_a_c_c_c_c_c_c_a_a.s
 *
 * -- so the landing is one src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_a_a.c
 * replacing both ovl_30_a_c_c_c_c_c_c_a_a_b.c and
 * asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_a_a_c.s, and the two
 * overlays/rom_7ef4f4/overlay.ld lines 48-49 collapsing back to the single
 * `ovl_30_a_c_c_c_c_c_c_a_a.o(.text)` they replaced.  objcmp cannot check a
 * two-function candidate (its --func cannot isolate one), so collapse.py does
 * the whole-object comparison directly.
 *
 * Reproduce: gen.py emits this file from a pin set given as reference call-site
 * numbers (calls.txt / body.txt are the transcription, extract.py built them);
 * minimise.py runs the greedy sweep in either direction and variants.py the
 * measured-worse table, each inside ONE container invocation.  collapse.py
 * proves the collapse.
 *
 * MEASURED WORSE (all against the same reference, 925 encodings):
 *
 *   final 65 pins, scratch ref                OK
 *   final 65 pins, REAL asm/ path             OK
 *   all 76 pin candidates                     OK  (11 inert -- dropped)
 *   whole-value consts (no mov/lsl)           OK  (spelling inert)
 *   0x80<<6: pin BOTH 128 and 168             OK  (128 inert -- dropped)
 *   drop site 145 (r0 before pool load)       2 encodings
 *   drop site 148 (8092c40 descending)        2 encodings
 *   8092c40 ASCENDING fill at 148             2 encodings (== unpinned)
 *   drop site 153 (lsl between movs)          2 encodings
 *   drop 75+95 (neg split around movs)        5 encodings
 *   ROM's emitted arg order, not ascending    94 encodings
 *   0x80<<6: pin FIRST use 128 not 168        307 encodings, push {r5,lr}
 *   drop site 98 (the -1 TRIPLE)              363 encodings, push {r5,lr}
 *   no pins at all                            834 encodings, 2356 vs 2396 bytes
 */

extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __SetCameraTarget(int slot, int a);
extern void __Func_808e118(void);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int slot, int x, int y);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809233c(int a, int b, int c, int d);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_965_200a548(void);
extern void __Func_809218c(int slot, int x, int y);
extern void __Func_80922c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")


void OvlFunc_965_2009238(void)
{
    unsigned char *p;

    __SetFlag(0x988);
    __SetFlag(0x98a);
    __CutsceneStart();
    __Func_808e118();
    __MessageID(0x2702);
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x94 << 1; q2 = 0xb0 << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN4; q0 = 0xa; q1 = 0x10; q2 = 0; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 1; q1 = -8; q2 = 0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 2; q1 = 8; q2 = 0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 3; q1 = 0x18; q2 = 0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    __MapActor_WaitMovement(3);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80933d4(0xc0 << 10, 0xc0 << 7);
    { PIN4; q0 = 0x8c << 17; q1 = -1; q2 = 0x90 << 17; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xb, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_809259c(0xd, 2);
    __Func_80925cc(0xc, 2);
    __CutsceneWait(0x28);
    __Func_809259c(0xd, 2);
    __Func_80925cc(0xc, 2);
    __CutsceneWait(0x28);
    __Func_809259c(0xd, 2);
    __Func_80925cc(0xc, 2);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xc; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xc, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(0xd, 0, 0);
    __CutsceneWait(0x19);
    __Func_80925cc(0xd, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xc; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xd, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xb, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0x14);
    __MapActor_Emote(0xd, 0x107, 0x28);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xd; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xd; q1 = 0x101; q2 = 0x4b;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xb; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __Func_8092adc(0xe, 0x80 << 7, 0);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __Func_80933f8(0x8c << 17, -1, 0xa0 << 17, 1);
    __Func_8093530();
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xa, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xa, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xe; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(3, 4);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 1;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x1e);
        __Func_8092adc(0xa, 0x80 << 8, 0);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xa, 3);
        __CutsceneWait(0x1e);
        __ActorMessage(0xa, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0x1e);
        __Func_8092adc(0xa, 0x80 << 8, 0);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xa, 4);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0xa, 0);
    }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xe, 0, 0x10);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(0xa, 0xc0 << 8, 0);
    __CutsceneWait(0x23);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 3; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(1, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xe; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0xe;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xe, 4);
        __CutsceneWait(0x14);
        __ActorMessage(0xe, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xe, 4);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0xe, 0);
    }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xa; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xd, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xd; q1 = 0x14ccc; q2 = 0xa666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xd, 0, 0x10);
    __CutsceneWait(0x14);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xc; q1 = 0x14ccc; q2 = 0xa666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xc, 0, 0x10);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xc; q1 = 0x107; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xc, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xa; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(0xa, 0x80 << 8, 0);
    __CutsceneWait(0x19);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 0xa;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xa, 3);
        __CutsceneWait(0x1e);
        __ActorMessage(0xa, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xa, 4);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0xa, 0);
    }
    __CutsceneWait(0xa);
    __Func_8092adc(0xa, 0xc0 << 8, 0);
    __CutsceneWait(0x23);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __Func_8092adc(0xe, 0xb0 << 8, 0);
    __CutsceneWait(0x28);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_8092848(0xc, 0xd, 0x32);
    __MapActor_SetAnim(0xc, 3);
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xc; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0xc, 3);
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xc; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80922c4(0xc, 0x20, 0);
    __Func_8092304(0xd, 0x20, 0);
    __Func_80922c4(0xc, 0, 0x10);
    __Func_8092304(0xd, 0x10, 0);
    { PIN3; q0 = 0xd; q1 = 0xac << 1; q2 = 0x9c << 1;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xac << 1; q2 = 0xa8 << 1;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(0xd, 1);
    { PIN3; q0 = 0xc; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    __Func_8092adc(0xe, 0x80 << 7, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xa4 << 1; q2 = 0x9c << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xb; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xa4 << 1; q2 = 0xa4 << 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0xb, 0x80 << 8, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetAnim(2, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(2);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_SetAnim(3, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(3, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    __CutsceneWait(0xa);
    __CutsceneEnd();
}

void OvlFunc_965_2009b10(void)
{
    unsigned char *p;

    __SetFlag(0x989);
    __CutsceneStart();
    __Func_808e118();
    __MessageID(0x272f);
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x94 << 1; q2 = 0x9c << 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0, 0, 0);
    __CutsceneWait(0xa);
    __Func_809233c(0x1, 0, 0x10, 0);
    { PIN4; q0 = 0x2; q1 = -0x10; q2 = -0x8; q3 = 0;
      __Func_809233c(q0, q1, q2, q3); }
    __Func_809233c(0x3, -0x10, 0x18, 0);
    __MapActor_WaitMovement(0x3);
    __CutsceneWait(0x14);
    __Func_80933d4(0xc0 << 10, 0xc0 << 7);
    { PIN4; q0 = 0x8c << 17; q1 = -1; q2 = 0xa4 << 17; q3 = 0x1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0xa);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xa; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xa, 0);
    __MapActor_Jump(0xa, 0x4, 0xd);
    __MapActor_Jump(0xa, 0x4, 0x1e);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xb; q1 = 0x80 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x80 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x80 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __Func_80925cc(0xe, 0x2);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xd; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xd, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xc; q1 = 0x101; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x19);
    __ActorMessage(0xc, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(0x3, 0x2);
    __CutsceneWait(0x14);
    __ActorMessage(0x3, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x2, 0x2);
    __CutsceneWait(0x14);
    __ActorMessage(0x2, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x1; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x1, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xa, 0x4);
    __CutsceneWait(0x14);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 0x2);
    __CutsceneWait(0x19);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xa, 0x2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xa; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0; q2 = -0x28;
      __Func_8092304(q0, q1, q2); }
    __Func_8092adc(0xa, 0, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 0x3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xd; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0xc, 0x3);
    __MapActor_DoAnim(0xd, 0x3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xd; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xa, 0x3);
    __CutsceneWait(0x1e);
    __SetCameraTarget(0xa, 0x1);
    { PIN3; q0 = 0xa; q1 = 0; q2 = -0x20;
      __Func_8092304(q0, q1, q2); }
    OvlFunc_965_200a548();
    __ClearFlag(0x301);
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __CutsceneStart();
    { PIN3; q0 = 0xa; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x3, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xb; q1 = 0x80 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x80 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x80 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x80 << 1; q2 = 0x46;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80933f8(0x8c << 17, -1, 0x9c << 17, 0x1);
    __Func_8093530();
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xc; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xc, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xd, 0x4);
    __CutsceneWait(0x14);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x2, 0x2);
    __CutsceneWait(0x1e);
    __Func_8092adc(0x2, 0x80 << 6, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0x2, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(0xa, 0x80 << 7, 0);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x3; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x3; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(0, 0xc0 << 6, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0x3, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x1, 0x4);
    __CutsceneWait(0x14);
    __ActorMessage(0x1, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xe; q1 = 0x103; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 0xe;
      __Func_8092c40(q0, q1); }
    __Func_8092adc(0, 0, 0);
    { PIN3; q0 = 0xb; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x80 << 5; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x1e);
        __Func_80925cc(0xe, 0x2);
        __CutsceneWait(0x14);
        __ActorMessage(0xe, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0x1e);
        __Func_80925cc(0xe, 0x2);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0xe, 0);
    }
    __CutsceneWait(0xa);
    __Func_80925cc(0xa, 0x2);
    __CutsceneWait(0x14);
    __Func_8092304(0xa, 0, 0x10);
    { PIN3; q0 = 0xa; q1 = 0x80 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(0xe, 0xa0 << 8, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xe, 0x4);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xa, 0x3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 0x2);
    __CutsceneWait(0x14);
    __Func_8092c40(0xe, 0);
    __CutsceneWait(0x28);
    __Func_8092adc(0xa, 0xa0 << 7, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xa; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x1e);
        __Func_8092adc(0xa, 0x80 << 6, 0);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0xa, 0x3);
        __CutsceneWait(0x1e);
        __ActorMessage(0xa, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0x1e);
        __Func_8092adc(0xa, 0x80 << 6, 0);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0xa, 0x4);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0xa, 0);
    }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xf);
    __MapActor_DoAnim(0xa, 0x3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0x14);
    __Func_80925cc(0xe, 0x2);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xe; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 0x3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(0xa, 0xa0 << 7, 0);
    __CutsceneWait(0x19);
    __Func_80925cc(0xa, 0x2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xa; q1 = 0x9c << 1; q2 = 0x9c << 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0, 0, 0);
    __Func_8092adc(0xa, 0x80 << 8, 0);
    __CutsceneWait(0x19);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __Func_8092848(0, 0x1, 0);
    __Func_8092848(0x3, 0x2, 0);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0, 0x3);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x3, 0x3);
    __MapActor_DoAnim(0x2, 0x3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x1; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(0x1, 0x2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x1);
    __MapActor_SetPos(0x1, 0, 0);
    __MapActor_SetAnim(0x2, 0x2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x2);
    __MapActor_SetPos(0x2, 0, 0);
    __MapActor_SetAnim(0x3, 0x2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(3, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x3);
    __MapActor_SetPos(0x3, 0, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xb0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xd, 0xb0 << 8, 0);
    __CutsceneWait(0x1e);
    __CutsceneEnd();
}
