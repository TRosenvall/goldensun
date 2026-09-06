/* ovl_314_c_c_a_c_a_a_c_c.c  --  OvlFunc_896_200978c  --  0x0200978c
 *   [asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c.s, the FIRST of FOUR
 *    functions in that .s -- a split IS needed; see LANDING at the bottom]
 *
 * 546 encodings of cutscene script (537 instructions plus its literal pool):
 * 146 calls, one 16-iteration counting
 * loop over a party member's halfword table, a null-guarded position copy, and
 * a four-arm message branch.  VERDICT:
 *
 *   OK OvlFunc_896_200978c -- 1400 bytes, 546 encodings and 146 relocations identical
 *
 * ...against BOTH a scratch copy of the reference and the REAL asm/ path, each
 * re-run to confirm.  Harness: scratch_elev/b242/f200978c/{gen,sweep2,sdiff,
 * minimise,objcmp2}.py -- one container invocation per whole sweep.
 *
 * ############################################################################
 * ## THIS MATCH DEPENDS ON A FLAG GROUP: -fno-gcse.  IT IS REQUIRED, IT IS   ##
 * ## THE ONLY FLAG THAT REACHES IT, AND NO SOURCE SPELLING SUBSTITUTES.      ##
 * ############################################################################
 *
 * The tree default for this TU is the bare `-O2 -mthumb -mthumb-interwork
 * -mcpu=arm7tdmi -fno-builtin -nostdinc -ffreestanding -fcall-used-r4`
 * (objcmp.cflags_for returns an EMPTY adjust set for
 * asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c.s -- no Makefile rule and no
 * wildcard names rom_78ef88 at all).  Under those flags this file is 545
 * instructions against 546 with 17 aligned differences.  ALL SEVENTEEN ARE ONE
 * DEFECT plus its literal-pool wake:
 *
 *     ref    ldr r6, =0x10b0 / adds r0, r6, #0 / bl __MessageID
 *            ... 4 calls, one conditional branch ...
 *            adds r0, r6, #1 / bl __Func_8019aa0
 *     ours   ldr r0, =0x10b0 / bl __MessageID
 *            ... same ...
 *            ldr r0, =0x10b1 / bl __Func_8019aa0
 *
 * -- one instruction short, one extra `.word 0x10b1` in the pool, and thirteen
 * `ldr [pc, #N]` and branch displacements dragged four bytes by it.
 *
 * WHY, READ FROM THE COMPILER RATHER THAN INFERRED.  cse.c's
 * `cse_end_of_basic_block` ends a block only at a CODE_LABEL
 * (/opt/camelot-gcc/gcc-2.96/gcc/cse.c:6572, `while (p && GET_CODE (p) !=
 * CODE_LABEL)`), so `m = 0x10b0` and a later `m + 1` sit in ONE cse block
 * whether or not a conditional jump separates them, and cse's `related_value`
 * turns 0x10b1 into `add rd, rn, #1` off the register already holding 0x10b0.
 * That is what the ROM has.  gcse's cprop is the pass that does NOT respect
 * that: it substitutes the constant across the CFG edge, the `plus` becomes a
 * bare CONST_INT, and the register dies.
 *
 * Reduced to two eight-line functions (probe.c in the harness directory) and
 * measured, with nothing else changed:
 *
 *   | m's use is ...                    | default -O2          | -fno-gcse      |
 *   |-----------------------------------|----------------------|----------------|
 *   | in a NESTED IF after the def      | ldr r0, =0x10b1      | add r0, r6, #1 |
 *   | in the SAME straight-line run     | add r5, r5, #1       | add r5, r5, #1 |
 *
 * NEW, AND IT IS THE CONVERSE OF A RECORDED RULE.  docs/elevation.md's batch-239
 * note (§"Pool loads come first...", the BOUNDED banner) says "once assignment
 * and both uses share ONE basic block, gcse's cprop substitutes the constant
 * back and the name is gone", and concludes that for a NAME a branch is
 * required.  Here the branch is what LOSES the name: cse would have kept the
 * register form, and it is precisely the block boundary that hands the insn to
 * cprop instead.  So gcse's cprop and cse's related_value disagree about the
 * same expression, and which one gets it is decided by whether a CODE_LABEL
 * intervenes.  The recorded rule and this one are the two halves of that.
 *
 * NO OTHER FLAG REACHES IT, measured one at a time on the final candidate:
 *
 *   -fno-cse-follow-jumps, -fno-cse-skip-blocks, -fno-rerun-cse-after-loop,
 *   -fno-expensive-optimizations, -fno-strength-reduce, -fno-strict-aliasing
 *       -- all 545 insns, 17 aligned, IDENTICAL to the default
 *   -fno-schedule-insns2   154 aligned      -O1   168 aligned
 *   -fno-gcse              EXACT
 *
 * AND `volatile` DOES NOT SUBSTITUTE, which matters because docs/elevation.md
 * §"`-fno-gcse` reaches a re-read that no `cse`-family flag does" records
 * "where a `-fno-gcse` rule and a `volatile` both match, prefer `volatile`".
 * They do not both match here: `volatile int m, m2;` at the default flags is
 * 550 instructions, 1408 bytes, a `sub sp, #8` frame and 526 encodings
 * differing.  The preference is unavailable, not declined.
 *
 * THE PROLOGUE, BY CONTENT.  `push {r5, r6, r7, lr}` with NO r4 confirms
 * -fcall-used-r4 is in force (r4 is still used, as the loop's hoisted 0x1ff --
 * a call-clobbered scratch inside a call-free loop).  THREE call-saved
 * registers means three values live across calls, and they are not the obvious
 * three: r5 is the counter `n`, THEN the `+0x5a` byte address in branch B,
 * THEN the 0xc0<<11 store value in the tail; r6 is the message base; r7 is the
 * actor pointer.  A one-variable-per-register reading of this prologue does not
 * start -- the whole match turned on splitting r6's tenants and r5's.
 *
 * TWO MESSAGE VARIABLES, NOT ONE.  This is the sibling's "ONE MATERIALISATION
 * IS NOT ONE VARIABLE" arriving in a second function and from the other side:
 * there the ROM had one `mov` for two source variables, here it has two `ldr`s
 * (0x10b0 in branch A, 0x10b2 in branch B) and the question is whether they are
 * one variable reused.  They are not.  With ONE `int m` the pseudo's live range
 * spans both arms, its `floor_log2(n_refs) * n_refs / live_length` priority
 * falls below the actor pointer's, the actor takes r6 and the message takes r7
 * -- 27 aligned differing, four instructions short.  With `m` and `m2` each
 * confined to its own arm, both are short-lived, both win r6 in their turn, the
 * actor gets r7, and it is exact.  The two never coexist, so the ROM cannot
 * tell them apart; only the register NUMBERS can.
 *
 * THE `orr` OPERAND ORDER: A NEW SPELLING, AND THE RECORDED ONE FAILS HERE.
 * docs/elevation.md §"`orr rd, rs` -- which operand becomes the destination"
 * has this function's exact pair of symptoms and even names a function with
 * both four instructions apart:
 *
 *     rom   ldrb r2, [r5] / mov r3, #0xfe / and r3, r2      CONSTANT is rd
 *     rom   ldrb r2, [r5] / mov r3, #1    / orr r3, r2      CONSTANT is rd
 *
 * The AND comes out right from the plain `a[0x5a] &= 0xfe` (and from the
 * longhand and the constant-first spelling -- all three tie).  The OR does not:
 * `|= 1`, `= 1 | x`, `= x | 1`, `*(a+0x5a) = *(a+0x5a) | 1`, naming the loaded
 * value, and a cast to int are ALL `ldrb r3 / mov r2, #1 / orr r3, r2`, because
 * fold canonicalises the constant to operand 1.  The section's remedy -- a
 * narrow local written first, `unsigned char one = 1; x = one | x;` -- is
 * WORSE here: 551 instructions and 37 aligned, the QImode local costing
 * zero-extensions; and an `int` local is inert exactly as the section says.
 *
 * What works is a COMPOUND ASSIGNMENT INTO A TEMPORARY SEEDED WITH THE
 * CONSTANT:
 *
 *     t1 = 1;  t1 |= a[0x5a];  a[0x5a] = t1;
 *
 * The mechanism is different from the section's, which is about the operand
 * ORDER surviving fold.  `t1 |= x` does not depend on order at all: C's
 * compound assignment makes `t1` the destination by definition, so the tie
 * `operand 0 == operand 1` lands on the constant's pseudo no matter what fold
 * does to the commutative operands.  That is why the TYPE is free here where
 * the recorded spelling needs a narrow one -- `int`, `unsigned int` and
 * `unsigned char` temporaries all tie.
 *
 * AND THE TEMPORARIES MUST BE SEPARATE.  With ONE `int t` serving both OR sites
 * the operand ROLES come out right but the two scratch registers swap
 * (`orrs r2, r3` where the ROM has `orrs r3, r2`): 9 aligned.  With `t1` in
 * branch B and `t2` in the tail it is exact.  Same shape as the two message
 * variables and as the sibling's split zero -- only the COUNT matters, the type
 * does not.
 *
 * TWENTY-FOUR PINS, AND A PIN THAT IS ACTIVELY HARMFUL.  The candidate set is
 * the 18 sites carrying a repeated expensive constant, widened by "same callee
 * AND same argument shape" to 23, plus FOUR hand-added from the residue: 27.
 * With all 27 the function is exact, so the sweep is pure minimisation, run
 * greedily to a fixpoint FROM BOTH ENDS.  Both directions converge on the SAME
 * 24: 8, 17, 22, 60, 61, 63, 74, 75, 78, 79, 80, 84, 85, 90, 110, 111, 113,
 * 123, 124, 127, 131, 135, 140, 145.
 *
 * The family rule paid THREE of five (63, 111, 123 required; 96 and 108 are
 * scaffolding the sweep strips, as is the CSE nomination 129).  It did not
 * scale unaided at this size: the two rules together MISSED FOUR REQUIRED
 * SITES, all found from the residue --
 *
 *   61  __MapActor_Emote(0x9, 0x105, 0x50)  0x105 pooled but used ONCE, and the
 *                                           only Emote in the function
 *   90  __Func_8092c40(0x1, 0x0)            BOTH ARGUMENTS CHEAP -- invisible to
 *                                           every constant rule -- and it wants
 *                                           the DESCENDING fill (ascending is
 *                                           the same 2 encodings as no pin)
 *  113  __MapActor_TravelTo(0x0, 0xda, 0x1d7)  0x1d7 pooled, used once
 *  131  __Func_80921c4(0x1, 0x84<<1, 0xf1<<1)  shape (I,S,S); the nominated
 *                                           80921c4 site 110 is (I,I,S), so the
 *                                           SHAPE family misses it
 *
 * Site 90 is the sibling's site 96 repeating exactly: a two-cheap-argument call
 * wanting the descending fill, reachable only from the residue.  Site 131 says
 * the family rule could be widened from callee+shape to CALLEE ONLY, which
 * catches it and costs three extra nominations (81, 117, 132) the sweep strips.
 *
 * A PIN IS NOT FREE.  Widening instead to "every site with an argument that
 * cannot be a Thumb `mov #imm8`" nominates 35 of the 134 calls and covers 23 of
 * the 24 -- but it is 544 instructions, TWO SHORT, and 15 aligned differing,
 * because six of its extra nominations (92, 99, 100, 102, 115, 118) are the
 * message sites, and a `register int q0 __asm__("r0")` fill at those forces `m`
 * to its value at the call and DESTROYS the local the whole match depends on.
 * Adding just those six to the exact 24 reproduces the 15.  This is the "when a
 * pin makes the output SHORTER, something that should be live is missing"
 * signature with a named cause; the rule survives as "any non-imm8 argument
 * EXCEPT where the argument is a named local", which at 30 sites is exact.
 *
 * THE RELOCATION LINE SORTS THE TWO PIN JOBS WITH NO MIDDLE, again.  Dropping
 * each of the 24 one at a time:
 *
 *   13 pins (61, 63, 79, 80, 84, 85, 90, 110, 111, 113, 123, 131, 145)
 *           -- EXACTLY 2 or 3 encodings, SIZE silent, RELOCATIONS SILENT
 *   11 pins (8, 17, 22, 60, 74, 75, 78, 124, 127, 135, 140)
 *           -- 15 to 492 encodings, RELOCATIONS ALWAYS differ, SIZE silent on
 *              5 of the 11
 *
 * Nothing landed in 4..14.  Thirteen ordering pins, eleven CSE pins, and SIZE
 * silent on nearly half the genuine CSE losses -- exactly as recorded; the
 * relocation line is the discriminator.
 *
 * UNIFORM WHOLE-VALUE ASCENDING FILL IS CORRECT.  One ascending statement per
 * pinned site reproduces every emitted order the ROM has -- `mov r1 / mov r2 /
 * mov r0 / lsl r1`, `mov r1 / mov r0 / lsl r1 / mov r2`, `mov r2 / mov r0 /
 * mov r1 / lsl r2`, `mov r1 / mov r2 / lsl r2 / lsl r1 / mov r0` -- with no
 * per-site tuning.  Respelling every `a << k` as its whole value is
 * byte-identical, so the shifts are gcc's own arithmetic; filling in the ROM's
 * OWN emitted register order at every pinned site is 40 aligned and WORSE.
 *
 * THE THREE-VALUE TEST IS AN `||` CHAIN, NOT A SWITCH.  The ROM's
 * `sub r3, #0xdc / cmp r3, #1 / bls` then `cmp r2, #0xdf / bne` reads like a
 * case decision tree, and docs/elevation.md §"A switch DECISION TREE means
 * three or more case labels" would send you there.  It is fold-const.c's
 * `fold_range_test` on a plain `||` chain: `v == 0xdc || v == 0xdd || v == 0xdf`
 * is exact, the explicit `(unsigned)(v - 0xdc) <= 1 || v == 0xdf` ties, and the
 * `switch (v) { case 0xdc: case 0xdd: case 0xdf: }` is 6 aligned differing --
 * gcc's own case-range merge picks a different comparison order.
 *
 * MEASURED WORSE (all against the final 24-pin set, one change at a time,
 * `aligned` = sequence-aligned differing instructions):
 *
 *   | change                                    | insns | aligned      |
 *   |-------------------------------------------|-------|--------------|
 *   | DEFAULT FLAGS (no -fno-gcse)              |  545  | 17           |
 *   | `volatile` on the message locals          |  550  | 526 enc      |
 *   | one shared message variable               |  544  | 27           |
 *   | message ids as plain literals             |  544  | 31           |
 *   | `m++;` then pass `m`                      |  546  | 16           |
 *   | plain `a[0x5a] |= 1`                      |  546  | 4            |
 *   | `unsigned char one = 1; x = one | x;`     |  551  | 37           |
 *   | one shared temp `t` for both OR sites     |  546  | 9            |
 *   | two actor locals instead of one           |  544  | 23           |
 *   | named `f = a + 0x5a` byte pointer         |  546  | 16           |
 *   | site 90 ascending instead of descending   |  546  | 2            |
 *   | ROM's own emitted register order at pins  |  546  | 40           |
 *   | `switch` for the three-value test         |  546  | 6            |
 *   | `v == 0xdf` tested first                  |  546  | 8            |
 *   | `q++` after the test instead of in it     |  546  | 4            |
 *   | `goto` back edge for the loop             |  544  | 15           |
 *   | `q = ...` before `n = 0`                  |  546  | 2            |
 *   | CSE-rule pins only (18)                   |  546  | 14           |
 *   | no pins at all                            |  548  | 89           |
 *   | six message sites pinned on top of the 24 |  544  | 15           |
 *   | 35-site "any non-imm8 argument" rule      |  544  | 17           |
 *   |-------------------------------------------|-------|--------------|
 *   | all 27 nominated pins                     |  546  | exact (tie)  |
 *   | 30-site rule minus the message sites      |  546  | exact (tie)  |
 *   | whole-value fill instead of `a << k`      |  546  | exact (tie)  |
 *   | `while` / `for` loop spelling             |  546  | exact (tie)  |
 *   | `i--;` in the body instead of `--i` in    |  546  | exact (tie)  |
 *   |   the condition                           |       |              |
 *   | `q++;` as its own statement               |  546  | exact (tie)  |
 *   | explicit `(unsigned)(v-0xdc) <= 1` range  |  546  | exact (tie)  |
 *   | `if (p)` / `if (p != (unsigned char *)0)` |  546  | exact (tie)  |
 *   | AND written via a temp as well            |  546  | exact (tie)  |
 *   | `unsigned char` temps                     |  546  | exact (tie)  |
 *   | `unsigned short` message locals           |  546  | exact (tie)  |
 *   | 0xc0<<11 hoisted into a local             |  546  | exact (tie)  |
 *   | `u = __GetUnit(1);` split out             |  546  | exact (tie)  |
 *
 * LANDING NEEDS A SPLIT, ONE LINKER LINE BECOMING TWO, AND A MAKEFILE RULE.
 *
 * The .s holds FOUR functions -- by its own annotations OvlFunc_896_200978c
 * (~537 instructions), _2009d04 (~516), _200a27c (~153), _200a400 (~238), all
 * four cutscene scripts of the same family -- so this .c cannot replace
 * the object as it stands.  EVERY line in the tree naming that .o, matched on
 * full path, is exactly ONE:
 *
 *     overlays/rom_78ef88/overlay.ld:32
 *         asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c.o(.text)
 *
 * There is NO .data and NO .rodata line for it anywhere, and none is needed:
 * overlays/rom_78ef88/overlay.map records `.data 0x0` and `.bss 0x0` for the
 * object (lines 67-68) against `.text 0x0200978c 0xee8` (line 204), and the
 * .ld's `.data` block names only ovl_314_c_c_c_c_a, _c_c_c_c_b, _c_c_c_c_c_b
 * and _c_c_c_c_c_c.  So the whole remap is that one `(.text)` line becoming
 * two, in order.
 *
 * `tools/split_s.py asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c.s
 * OvlFunc_896_200978c` is the cut.  The target is the FIRST function, so the
 * "before" part is empty and is not written; the pieces are `_c_c_b.s` (this
 * function) and `_c_c_c.s` (the other three).  No `.L` label crosses that
 * boundary -- every label in the file (.L17b6 .. .L250e) and the one
 * `.pool_aligned` sit strictly inside a single function -- so the splitter's
 * label-crossing refusal should not fire.  The suffixes `_c_c_a/_b/_c` are all
 * free in both asm/ and src/ (only `_c_a`, `_c_b`, `_c_c` exist).
 *
 * Then the -fno-gcse rule, which the tree already has a group and four
 * precedents for (Makefile:303 defines GCSE_CFLAGS; :306, :605, :620, :634 use
 * it):
 *
 *     asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c_b.o: \
 *         src/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c_b.c
 *         $(GCC296_CC) $(GCSE_CFLAGS) -S -o $(@:.o=.s) $<
 *         printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
 *         arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
 *
 * The split is what keeps that flag honest: it confines -fno-gcse to a TU
 * holding this one function, so the other three keep the tree default and
 * nothing else in the overlay is built differently.  Do the split first and
 * check `make compare` green BEFORE writing the .c, because a layout mistake
 * and a bad decompilation look identical at the end.
 *
 * Precedent in the same overlay and the same family:
 * ovl_314_c_c_a_c_a_a_c_a.c (batch 241) and ovl_314_c_c_a_b.c are already
 * elevated with their .ld lines untouched -- but both of those .s files held
 * ONE function, which is the difference here.
 */
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __SetCameraTarget(int a, int b);
extern unsigned char *__GetUnit(int party);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_800fe9c(void);
extern void __Func_8019aa0(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_896_200c248(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_896_200978c(void)
{
    unsigned char *p;
    unsigned short *q;
    unsigned char *a;
    int n;
    int i;
    int v;
    int m;
    int m2;
    int t1;
    int t2;

    __PlaySound(0xa1);
    __Func_80925cc(0xc, 0x3);
    __CutsceneWait(0x28);
    p = __MapActor_GetActor(0xc);
    if (p != 0)
        __MapActor_SetPos(0xd, *(int *) (p + 0x8), *(int *) (p + 0x10));
    __MapActor_SetPos(0xc, 0x0, 0x0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xd; q1 = 0xc0 << 6; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x5, 0x3);
    __Func_80925cc(0x5, 0x3);
    __CutsceneWait(0x28);
    __Func_80925cc(0x5, 0x2);
    OvlFunc_896_200c248(0x5, 0x14);
    __MapActor_DoAnim(0xd, 0x3);
    __CutsceneWait(0xa);
    __Func_809259c(0x5, 0x3);
    { PIN3; q0 = 0x9; q1 = 0x80 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x9, 0x2);
    OvlFunc_896_200c248(0x9, 0x28);
    __MapActor_DoAnim(0x5, 0x3);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x9; q1 = 0xb0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xd, 0x2);
    OvlFunc_896_200c248(0xd, 0x14);
    __Func_80925cc(0x5, 0x1);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0xd, 0x3);
    __CutsceneWait(0xa);
    OvlFunc_896_200c248(0xd, 0x28);
    __Func_809259c(0xa, 0x1);
    __MapActor_DoAnim(0xa, 0x3);
    __CutsceneWait(0xa);
    OvlFunc_896_200c248(0xa, 0xa);
    __Func_80925cc(0xb, 0x1);
    __MapActor_SetAnim(0xb, 0x3);
    OvlFunc_896_200c248(0xb, 0x50);
    __Func_80925cc(0xd, 0x2);
    OvlFunc_896_200c248(0xd, 0x28);
    __Func_80925cc(0x5, 0x2);
    __CutsceneWait(0xa);
    OvlFunc_896_200c248(0x5, 0xa);
    __Func_809259c(0xd, 0x2);
    __Func_80925cc(0x9, 0x2);
    __CutsceneWait(0x50);
    __MapActor_DoAnim(0x5, 0x4);
    __CutsceneWait(0x14);
    OvlFunc_896_200c248(0x5, 0x50);
    __MapActor_DoAnim(0xd, 0x4);
    OvlFunc_896_200c248(0xd, 0x50);
    __Func_80925cc(0x5, 0x2);
    __CutsceneWait(0x4);
    OvlFunc_896_200c248(0x5, 0x14);
    __Func_80925cc(0xa, 0x1);
    __MapActor_SetAnim(0xa, 0x3);
    OvlFunc_896_200c248(0xa, 0xa);
    __Func_80925cc(0xb, 0x1);
    OvlFunc_896_200c248(0xb, 0xa);
    __Func_80925cc(0xa, 0x1);
    OvlFunc_896_200c248(0xa, 0xa);
    { PIN3; q0 = 0x9; q1 = 0xc0 << 6; q2 = 0x50;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0x105; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0xb, 0x1);
    { PIN3; q0 = 0xb; q1 = 0xa0 << 7; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0xb, 0x2);
    OvlFunc_896_200c248(0xb, 0x14);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_80933f8(0x1050000, -0x1, 0xe9 << 17, 0x0);
    __Func_800fe9c();
    __WaitFrames(0x1);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xb0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0x0, 0x2);
    __Func_80925cc(0x1, 0x2);
    { PIN3; q0 = 0x0; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0xf4; q2 = 0xef << 1;
      __Func_809218c(q0, q1, q2); }
    __Func_80921c4(0x1, 0x82 << 1, 0xf5 << 1);
    __MapActor_WaitMovement(0x0);
    __MapActor_SetAnim(0x0, 0x1);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xb0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x0, 0x2);
    __MapActor_DoAnim(0x1, 0x4);
    __CutsceneWait(0x14);
    n = 0;
    q = (unsigned short *) (__GetUnit(0x1) + 0xd8);
    i = 0xe;
    do {
        v = *q++ & 0x1ff;
        if (v == 0xdc || v == 0xdd || v == 0xdf)
            n++;
    } while (--i >= 0);
    { PIN2; q1 = 0x0; q0 = 0x1;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0x0, 0x0) == 0) {
        m = 0x10b0;
        __MessageID(m);
        __MapActor_DoAnim(0x1, 0x3);
        __CutsceneWait(0xa);
        if (n <= 2) {
            OvlFunc_896_200c248(0x1, 0x1e);
            __Func_80921c4(0x1, 0xfc, 0xf3 << 1);
            __Func_80925cc(0x1, 0x2);
            __CutsceneWait(0xa);
            __Func_8019aa0(m + 1, 0x1, 0x0);
        } else {
            __MessageID(0x10b4);
            OvlFunc_896_200c248(0x1, 0x1e);
        }
    } else if (n <= 2) {
        m2 = 0x10b2;
        __MessageID(m2);
        __Func_80925cc(0x1, 0x3);
        __MapActor_DoAnim(0x1, 0x4);
        OvlFunc_896_200c248(0x1, 0xa);
        __MapActor_DoAnim(0x1, 0x4);
        __Func_80925cc(0x1, 0x1);
        __MapActor_SetSpeed(0x1, 0x80 << 10, 0x80 << 9);
        a = __MapActor_GetActor(0x0);
        a[0x5a] &= 0xfe;
        { PIN3; q0 = 0x1; q1 = 0xf4; q2 = 0xef << 1;
          __Func_80921c4(q0, q1, q2); }
        { PIN3; q0 = 0x0; q1 = 0xc0 << 9; q2 = 0xc0 << 8;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_Jump(0x0, 0x6, 0x0);
        { PIN3; q0 = 0x0; q1 = 0xda; q2 = 0x1d7;
          __MapActor_TravelTo(q0, q1, q2); }
        __MapActor_WaitMovement(0x0);
        __Func_8019aa0(m2 + 1, 0x1, 0x0);
        __Func_80925cc(0x0, 0x2);
        __Func_8092adc(0x0, 0x0, 0x1e);
        t1 = 0x1; t1 |= a[0x5a]; a[0x5a] = t1;
    } else {
        __MessageID(0x10b5);
        __Func_80925cc(0x1, 0x3);
        __MapActor_DoAnim(0x1, 0x4);
        OvlFunc_896_200c248(0x1, 0xa);
        __MapActor_DoAnim(0x1, 0x4);
        { PIN3; q0 = 0x0; q1 = 0xe0 << 8; q2 = 0x1e;
          __Func_8092adc(q0, q1, q2); }
    }
    { PIN2; q0 = 0x80 << 8; q1 = 0x80 << 5;
      __Func_80933d4(q0, q1); }
    __SetCameraTarget(0x1, 0x1);
    __Func_8093530();
    { PIN3; q0 = 0x1; q1 = 0x80 << 8; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x1, 0x2);
    __MapActor_SetSpeed(0x1, 0x80 << 8, 0x80 << 7);
    a = __MapActor_GetActor(0x1);
    a[0x5a] &= 0xfe;
    { PIN3; q0 = 0x1; q1 = 0x84 << 1; q2 = 0xf1 << 1;
      __Func_80921c4(q0, q1, q2); }
    t2 = 0x1; t2 |= a[0x5a]; a[0x5a] = t2;
    __Func_80921c4(0x1, 0x8b << 1, 0xf0 << 1);
    *(int *) (a + 0x30) = 0xc0 << 10;
    *(int *) (a + 0x34) = 0x80 << 10;
    __PlaySound(0x99);
    *(int *) (a + 0x28) = 0xc0 << 11;
    __MapActor_SetAnim(0x1, 0x7);
    { PIN3; q0 = 0x1; q1 = 0x9c << 1; q2 = 0xeb << 1;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(0x1, 0x1);
    __CutsceneWait(0x1e);
    __PlaySound(0x99);
    *(int *) (a + 0x28) = 0xc0 << 11;
    __MapActor_SetAnim(0x1, 0x7);
    { PIN3; q0 = 0x1; q1 = 0xab << 1; q2 = 0xeb << 1;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(0x1, 0x1);
    __CutsceneWait(0x1e);
    __PlaySound(0x99);
    *(int *) (a + 0x28) = 0xc0 << 11;
    __MapActor_SetAnim(0x1, 0x7);
    { PIN3; q0 = 0x1; q1 = 0xbc << 1; q2 = 0xeb << 1;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(0x1, 0x1);
}
