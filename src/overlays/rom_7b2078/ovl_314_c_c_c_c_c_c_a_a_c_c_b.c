// fakematch
/* ovl_314_c_c_c_c_c_c_a_a_c_c_b.c  --  OvlFunc_926_200aad0  --  0x0200aad0
 *   [asm/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_a_a_c_c.s, FIRST OF TWO --
 *    this .s has TWO `.thumb_func_start`s and a split IS required; see LANDING]
 *
 * 1939 instructions of straight-line cutscene script: 525 calls over 28 distinct
 * callees, THREE `__MapActor_GetActor` + `__MapActor_SetPos` guard blocks, TWO
 * `__MapActor_GetActor(0)->f5a` bit-twiddles, ONE `__Func_8091c7c` if/else whose
 * arms each bump the `iwram_3001ebc` counter, and THREE mid-function
 * `.pool_aligned` dumps.  Prologue is `push {lr}` -- a NARROW push, no
 * callee-saved register anywhere -- and ZERO r8-r11 traffic.  VERDICT:
 *
 *   OK OvlFunc_926_200aad0 -- 5000 bytes, 1958 encodings and 526 relocations identical
 *
 * ...against BOTH the single-function scratch reference and the REAL asm/ path.
 *
 * NO FLAG GROUP IS INVOLVED, AND THAT IS A POSITIVE FINDING.  `grep -n
 * rom_7b2078 Makefile` returns NOTHING: the directory has no explicit rule and
 * no wildcard reaches it, so the generic cross-dir `asm/%.o: src/%.c` applies
 * and the TU takes the tree default -O2.  objcmp printed no "(built with: ...)"
 * line when screened through the real asm/ path, which is the check that catches
 * the O1-wildcard trap.  Nothing to add to the Makefile.
 *
 * THE ONE LEVER THAT MATTERED WAS NOT A PIN -- IT WAS SPELLING THE BIT-TWIDDLE
 * AS A COMPOUND ASSIGNMENT.  The ROM's two byte sites are
 *
 *     bl __MapActor_GetActor / add r0, #0x5a / ldrb r2, [r0]
 *     mov r3, #0xfe / and r3, r2 / strb r3, [r0]        (and `orr` with #1)
 *
 * The recorded rule "a pointer-returning call whose result dies immediately must
 * not be named" says to keep the call anonymous, and that is right here.  But
 * the anonymous form has a trap the recorded entry does not spell out: written
 * out longhand as
 *
 *     __MapActor_GetActor(0)->f5a = 0xfe & __MapActor_GetActor(0)->f5a;
 *
 * the call expression appears TWICE and gcc emits TWO `bl`s.  That is +2
 * relocations, +10 encodings, and the extra live value forces `push {r5, lr}`
 * against the ROM's `push {lr}` -- 398 differing with SIZE and RELOCATIONS both
 * moving, and the very first encoding wrong.  Only the COMPOUND form
 *
 *     __MapActor_GetActor(0)->f5a &= 0xfe;
 *
 * evaluates the call once while still leaving it anonymous.  This single change
 * took the candidate from 428 differing to 34 and made SIZE and the encoding
 * COUNT exact.  Naming the pointer instead (`ap = ...; ap->f5a &= 0xfe;`) is the
 * recorded failure mode and measures 400 differing with SIZE differing.
 * SO THE TWO REQUIREMENTS ARE INDEPENDENT: evaluate once, AND stay anonymous;
 * only the compound assignment satisfies both.
 *
 * 173 CSE + 1 FAMILY CANDIDATES, 4 ORDERING EXTRAS, 170 REQUIRED.  The candidate
 * set is 173 sites nominated by the recorded CSE rule (an argument list carrying
 * a constant the ROM builds more than once and that is not a bare `mov #imm8`;
 * `gen.py --info` prints it), PLUS site 166 nominated by CALL-SITE FAMILY, PLUS
 * 4 sites -- 250, 302, 475, 490 -- that NO NOMINATION RULE REACHES.  With all
 * 178 the function is exact, so the sweep is
 * pure minimisation: every site stripped individually under objcmp, greedily,
 * re-testing after each drop, run to a fixpoint FROM BOTH ENDS.  Both directions
 * survive the SAME 170, in two rounds each.  The eight that fall are sites 3, 36,
 * 228, 248, 348, 417, 436 and 517 -- every one a LATER use of a value whose
 * earlier use is pinned, the recorded "one pin at the first use covers the later
 * ones".  They are INERT SCAFFOLDING AND DO NOT SHIP.
 *
 * THE FAMILY HEURISTIC PAID, AND ITS BOUND IS EXACTLY THE RECORDED ONE.  Run as
 * a first pass it earns its place cheaply: `GEN_FAMILY=1` nominates ONE extra
 * site over the CSE rule -- site 166, an `__MapActor_Emote(slot, id, n)` whose
 * `id` is used once but whose family-mates' ids are pooled and repeated -- and
 * that one site takes the residue from 34 differing WITH RELOCATIONS DIFFERING
 * to 9 with relocations SILENT.  It is also the single hardest site on the
 * function to find by eye (see the pool finding below), so the rule bought
 * precisely the thing reading the diff would have got wrong.
 *
 * AND IT IS NOT SUFFICIENT ON ITS OWN, at 1939 instructions as at 1649.  The
 * family-nominated set of 174 leaves 9 differing, and the whole of that 9 is
 * FOUR all-cheap-argument ordering sites -- 250, 475, 490 (2 each) and 302 (3).
 * The recorded warning that a family whose arguments are ALL CHEAP is nominated
 * by neither rule is exactly what happened: `__Func_8093054(N, 0)`,
 * `__Func_8092c40(3, 0)` and `__MapActor_SetPos(0xa, ...)` are invisible to both
 * the CSE rule and the family rule, and only the residue loop finds them.
 *
 * THE DESCENDING FILL IS A PROPERTY OF THE SITE, AND THIS FUNCTION PROVES IT
 * CLEANLY.  ALL FIVE `__Func_8093054` sites -- 49, 193, 250, 257, 475 -- emit
 * `m1 m0` in the ROM, i.e. every one of them READS as descending.  Only TWO of
 * them (250 and 475) actually need `q1 = 0; q0 = N;`: at 49, 193 and 257 gcc
 * already emits that order unaided and they are correctly left unpinned.  Had
 * the direction been written from the callee name, three needless pins would
 * have gone in.  `__Func_8092c40` appears ONCE, at 490, and also wants
 * descending.  The recorded "the tell is binary, not graded" holds exactly: the
 * ASCENDING fill at each of 250, 475, 490 measures 2 differing, and so does
 * DROPPING THAT PIN ALTOGETHER.  Ascending at a descending site is a non-pin.
 *
 * SITE 302 WANTS AN ASCENDING ORDERING PIN, AND THE ROM'S OWN ORDER IS WORSE.
 * `__MapActor_SetPos(0xa, 0xec << 17, 0x98 << 18)` is emitted by the ROM as
 * `m1 m2 m0 l1 l2` -- the plain `mov r0, #10` sits BETWEEN the two base movs and
 * the two shifts.  Transcribing that order measures 3 differing; so does leaving
 * the site unpinned.  The plain UNIFORM ASCENDING pin is exact.  All six fill
 * permutations were measured (0|1|2 exact, 1|0|2 = 2, 1|2|0 = 3, 0|2|1 = 4,
 * 2|1|0 = 4, 2|0|1 = 5), which is a third independent confirmation of the
 * recorded "UNIFORM ASCENDING IS CORRECT, NOT MERELY CHEAPER".
 *
 * NEW -- AN ORDERING PIN CAN READ AS A CSE LOSS WHEN IT SITS BESIDE A POOL DUMP.
 * The recorded discriminator states the mechanism as: "a lost rematerialisation
 * moves every following `bl`, so every relocation offset moves.  A pure argument
 * transposition moves no byte offset at all."  SITE 166 IS A COUNTER-EXAMPLE.
 * It is `__MapActor_Emote(1, 0x103, 0)`; `0x103` is used EXACTLY ONCE in the
 * whole function, so nothing is being commoned and nothing can be lost.  The
 * only defect without the pin is a two-instruction transposition -- the ROM has
 * `mov r0, #1 / ldr r1, =0x103`, gcc emits them the other way round -- and the
 * total encoding count is UNCHANGED at 1958.  Yet dropping the pin measures
 * 25 differing WITH RELOCATIONS DIFFERING, which the recorded rule reads as a
 * CSE loss, and 25 is exactly this function's CSE-band FLOOR.
 *
 * The mechanism is the literal pool.  Moving the pool-loading `ldr` two bytes
 * changes where gcc's Thumb pool machinery decides it must dump, and the dump
 * lands ONE INSTRUCTION EARLIER: at site 277 the reference emits
 * `bl __MapActor_Emote / b .L / .pool` where the unpinned candidate emits
 * `b .L / .pool / bl __MapActor_Emote`.  Every `bl` after that point moves, so
 * every relocation offset moves, and every downstream `ldr rN, [pc, #imm]`
 * re-encodes against the shifted pool base -- 25 encodings in total, of which
 * exactly 2 are the transposition itself and NONE is a rematerialisation.  The
 * pool concerned is the SECOND of the three, the one dumping after site 277.
 *
 * So the relocation line sorts pin JOBS by their EFFECT,
 * not by their CAUSE, and a transposition adjacent to a pool dump is filed on
 * the wrong side.  Read the encoding COUNT too: a genuine CSE loss changes it,
 * this did not.
 *
 * THE BANDS THEMSELVES HOLD, ON A FURTHER 170 DROPS.  Dropping each required pin
 * one at a time, from the final set:
 *
 *   20 ORDERING pins   RELOCATIONS silent, SIZE silent on 20 of 20,
 *                      encoding count EXACTLY 2 or 3 (fourteen 2s, six 3s)
 *  150 CSE pins        RELOCATIONS differ on 150 of 150,
 *                      encoding counts 25 to 1704, SIZE silent on 63 of 150
 *
 * A RESIDUE OF 4 TO 9 DID NOT OCCUR ONCE, on top of the recorded 138 -- the "no
 * middle" claim now rests on 308 drops.  SIZE was silent on 42% of the genuine
 * CSE losses, matching the recorded 42% and again discriminating nothing.  One
 * caveat worth recording: the 4-9 gap is a property of DROPPING A REQUIRED PIN.
 * A WRONG FILL ORDER at a kept pin lands there freely -- site 302 measures 4 and
 * 5 under two of its six permutations.  Do not read a residue of 5 as proof that
 * no pin is missing.
 *
 * THERE IS NO HOLE IN THE PIN SET, AND THAT WAS CHECKED, NOT ASSUMED.  The
 * template needed one (a symbol address the ROM loads once and copies, where
 * pinning all three CSE-nominated sites was wrong).  Here the full 178-pin set
 * is already exact, so no CSE-nominated site is harmful; minimisation only
 * removed sites that were redundant, never sites that were damaging.  This
 * function loads no symbol address at all -- its only relocations are the 525
 * `bl`s and the one R_ARM_ABS32 for `iwram_3001ebc` -- which is why the
 * template's failure mode has nothing to attach to.
 *
 * ALL NINE POOLED VALUES ARE BARE LITERALS.  0x101 (used 29 times), 0x6666 (6),
 * 0xcccc (5), 0x105 (4), 0x1969, 0x895, 0x3333, 0x103, and the `iwram_3001ebc`
 * address.  Every numeric one is either odd or has a base above 0xff once the
 * trailing zeros are shifted out (0x6666 = 0x3333 << 1, 0xcccc = 0x3333 << 2),
 * so NONE is a shifted byte gcc could build with `mov`/`lsl` and each pools
 * unaided.  The message id 0x1969 and the save bit 0x895 are both odd, so
 * neither wants a `_MSG_`/const symbol; message.sym and const.sym need nothing.
 *
 * THE STRUCTURAL BLOCKS ALL CAME OUT UNAIDED.  The three GetActor/SetPos guards
 * are the recorded shape verbatim -- `a = __MapActor_GetActor(0); if (a != 0)
 * __MapActor_SetPos(slot, a->f8, a->f10);` -- with `ldr r1, [r0, #8]` /
 * `ldr r2, [r0, #0x10]` reading the two fields WHOLE, and `beq` past the body
 * meaning the guard is `!= 0`.  Reusing ONE `struct Actor *` across all three
 * and giving each its own variable are BYTE-IDENTICAL here, so the recorded
 * "two results of the same call need two pointer variables" does not bite: that
 * rule is about a pointer that must survive, and none of these does.  The one
 * diamond is a plain `if (__Func_8091c7c(0, 0) == 0) { ... } else { ... }` with
 * a counter bump in each arm and NO flag variable -- the result is tested once
 * and never re-read, so the template's `int v` has no counterpart here.
 *
 * MEASURED-WORSE TABLE (against the scratch ref, one container run each):
 *
 *   final 170 pins, scratch ref                     OK
 *   final 170 pins, REAL asm/ path                  OK
 *   all 178 pin candidates (8 inert)                OK
 *   ONE actor pointer vs THREE                      OK  -- both spellings tie
 *   whole-value consts (no mov/lsl)                 OK  -- both spellings tie
 *   `unsigned char bit = 1` for the ORR             OK  -- inert here
 *   no pins at all                                1752 differing, SIZE, RELOC
 *   173 CSE pins only, no extras                    34 differing, RELOC
 *   174 family-nominated pins, no extras              9 differing
 *   drop all 5 non-CSE extras                       34 differing, RELOC
 *   BITDOUBLE: `X = k & X` (two bl)                398 differing, SIZE, RELOC
 *   NAMEBIT: name the byte-write pointer           400 differing, SIZE, RELOC
 *   descending fill EVERYWHERE                     900 differing, RELOC
 *   ROM's emitted arg order EVERYWHERE             238 differing
 *   178 pins with NO descending sites                6 differing
 *   ASCENDING at 250 / 475 / 490 individually        2 differing each
 *   drop pin 250 / 475 / 490 individually            2 differing each
 *   ROM's emitted order at 302                       3 differing
 *   drop pin 302                                     3 differing
 *   drop pin 166                                    25 differing, RELOC
 *
 * The whole-value row is worth keeping: with the pins in place `0x80 << 8` and
 * `0x8000` produce the SAME OBJECT, so the recorded whole-value spelling is a
 * free choice here rather than a lever.  The mov/lsl spelling is kept because it
 * is what the reference reads as.
 *
 * TRYC WAS NOT USED AS A SCREEN.  The reference keeps THREE literal pools inside
 * the function body (`b .L / .pool_aligned / .L:` after sites 107, 277 and 383),
 * which is precisely the case tryc's `=value` normalisation is blind to, and the
 * pool placement is the thing site 166 turns on.  Every number above is objcmp.
 *
 * LANDING -- A SPLIT IS REQUIRED, AND THE DIRECTORY ALREADY HAS THE PRECEDENT.
 * The .s holds TWO functions: OvlFunc_926_200aad0 (lines 8-1965) and
 * OvlFunc_926_200be58 (lines 1971-2237).  EXACTLY ONE linker line names the .o,
 * and it is a `.text` line:
 *
 *   overlays/rom_7b2078/overlay.ld:62
 *       asm/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_a_a_c_c.o(.text)
 *
 * There is NO `.data`, `.rodata` or `.bss` line naming this .o anywhere in the
 * tree (`grep -rn ovl_314_c_c_c_c_c_c_a_a_c_c` over everything outside .git finds
 * that one line and nothing else), and the .s itself declares no section beyond
 * its text.  So unlike the recorded single-function-plus-.data case there is no
 * second line to re-point.
 *
 * The target is the FIRST function, so the split is TWO ways, not three, and
 * `tools/split_s.py` writes no `_a`:
 *
 *   python3 tools/split_s.py \
 *     asm/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_a_a_c_c.s OvlFunc_926_200aad0
 *
 * leaves this function in ovl_314_c_c_c_c_c_c_a_a_c_c_b.s, OvlFunc_926_200be58
 * in ovl_314_c_c_c_c_c_c_a_a_c_c_c.s, and rewrites overlay.ld:62 into the two
 * lines naming those objects IN THAT ORDER.  `make compare` must be green BEFORE
 * any .c is written -- a layout mistake and a bad decompilation look identical
 * at the end.  Then this file lands at
 * src/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_a_a_c_c_b.c and
 * ovl_314_c_c_c_c_c_c_a_a_c_c_b.s is `git rm`ed; the `_c_c_c.s` remainder STAYS
 * in asm/ and stays committed.  The identical two-way shape is already in the
 * tree three lines up, at overlay.ld:44-45 (ovl_314_c_c_a_c_c_c_a_a_c_c_b.o +
 * ..._c_c_c.o), from the same directory.
 *
 * TWO THINGS THE SPLIT DOES NOT BREAK, both checked: the `.L` label sets of the
 * two functions are DISJOINT and each is self-contained (this one defines and
 * references exactly .L2b9c/.L2bb0/.L2bc4/.L2ed8/.L34d8/.L38e8/.L3d28/.L3d56, and
 * no branch crosses the boundary), and `src/overlays/rom_7b2078/exports.s:3`
 * exports `OvlFunc_926_200be58` -- the OTHER function -- which stays in asm/ and
 * keeps its `.export_func` intact.  OvlFunc_926_200aad0 is not exported.
 *
 * Because this file uses the register-pin idiom it is a fakematch, and landing
 * must add the registry line
 *   OvlFunc_926_200aad0  src/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_a_a_c_c_b.c
 * to fakematch.txt, as both the template and the in-directory sibling
 * ovl_314_c_c_c_c_c_c_a_a_c_b.c already have.
 *
 * Harness: scratch_elev/b241/f200aad0/ -- extract.py (ref.s -> calls.txt; the
 * MARK regex is widened over the b240 copy to cover add/and/orr/ldrb/strb, which
 * this function's byte sites use and which otherwise leak into the next call's
 * argument list), mkbody.py (calls.txt -> body.txt, assert-heavy: it refuses to
 * run unless it folds exactly 3 ACTORPOS blocks, 2 bit sites and 2 bumps, and it
 * REBUILDS site 417's argument list by hand because the ROM interleaves the
 * `strb` of the preceding bit site between that call's two `lsl`s), gen.py,
 * sweep.py, minimise.py, diffcand.py (patched to print a running CALL-SITE number
 * beside every diff line, which is what turned the residue into a site list),
 * run.sh, classify via the minimise logs.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x5a - 0x14];
    unsigned char f5a;
};

extern unsigned char *iwram_3001ebc;

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int a);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __LearnInnateMove(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093054(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void OvlFunc_926_200c1ec(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_926_200aad0(void)
{
    struct Actor *a;

    __CutsceneStart();
    { PIN3; q0 = 0x0; q1 = 0x6666; q2 = 0x3333;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_809218c(0x0, 0xec << 1, 0x86 << 2);
    __MapTransitionIn();
    __WaitMapTransition();
    __MapActor_WaitMovement(0x0);
    __Func_8092adc(0x9, 0x0, 0x14);
    __Func_80925cc(0x9, 0x2);
    __CutsceneWait(0x14);
    __MessageID(0x1969);
    __Func_8093040(0x9, 0x0, 0x14);
    { PIN3; q0 = 0x0; q1 = 0x80 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x0, 0x3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x8; q1 = 0x80 << 7; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x0, 0x1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x9; q1 = 0xd0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(0x1, a->f8, a->f10);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(0x2, a->f8, a->f10);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(0x3, a->f8, a->f10);
    { PIN3; q0 = 0x0; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0xe8 << 1; q2 = 0xfc << 1;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xf0 << 1; q2 = 0xfc << 1;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xf8 << 1; q2 = 0xf8 << 1;
      __Func_809218c(q0, q1, q2); }
    __Func_809218c(0x3, 0xe0 << 1, 0xf8 << 1);
    __MapActor_WaitMovement(0x0);
    __MapActor_WaitMovement(0x2);
    __MapActor_WaitMovement(0x3);
    { PIN3; q0 = 0x3; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitMovement(0x1);
    { PIN3; q0 = 0x1; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x0; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093054(0x1, 0x0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x1; q1 = 0xa0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x3, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x2; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x2, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x3);
    { PIN3; q0 = 0x8; q1 = 0xc0 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_8092adc(0x0, 0x0, 0x0);
    { PIN3; q0 = 0x1; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x80 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0x2, 0x2);
    { PIN2; q0 = 0x2; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __Func_8093040(0x2, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_80925cc(0x2, 0x2);
    __CutsceneWait(0x14);
    __Func_8093040(0x2, 0x0, 0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_8092848(0x0, 0x2, 0x32);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x2, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x3; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x3, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_80925cc(0x3, 0x1);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x0; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xa0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x1, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x4);
    __CutsceneWait(0x14);
    __Func_809280c(0x8, 0x2, 0x0);
    __CutsceneWait(0xa);
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_809280c(0x0, 0x2, 0x0);
    __Func_809280c(0x1, 0x2, 0x0);
    __Func_809280c(0x3, 0x2, 0x0);
    { PIN3; q0 = 0x2; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x2, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __MapActor_DoAnim(0x2, 0x3);
    __CutsceneWait(0x14);
    __Func_809259c(0x1, 0x2);
    __CutsceneWait(0x14);
    __Func_8093040(0x1, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809280c(0x8, 0x1, 0x0);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x2; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x2, 0x0, 0x14);
    __Func_809280c(0x8, 0x1, 0x0);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_809259c(0x2, 0x2);
    __CutsceneWait(0x14);
    __Func_8093040(0x2, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_80925cc(0x3, 0x2);
    __CutsceneWait(0x14);
    __Func_8093040(0x3, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_809259c(0x0, 0x2);
    __Func_809259c(0x1, 0x2);
    __Func_809259c(0x2, 0x2);
    __Func_80925cc(0x3, 0x2);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x8, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_80925cc(0x8, 0x2);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x2, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x1; q1 = 0x103; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(0x1, 0x2);
    __CutsceneWait(0x3c);
    __Func_8093040(0x1, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x0; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x1, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0x81 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(0x8, 0x1);
    __CutsceneWait(0x3c);
    __MapActor_DoAnim(0x8, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_809259c(0x0, 0x1);
    __Func_809259c(0x1, 0x1);
    __Func_809259c(0x2, 0x1);
    __Func_80925cc(0x3, 0x1);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x8, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x1; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093054(0x1, 0x0);
    __CutsceneWait(0x14);
    __Func_80925cc(0x2, 0x1);
    __CutsceneWait(0x14);
    __Func_8093040(0x2, 0x0, 0x14);
    { PIN3; q0 = 0x1; q1 = 0xa0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x2, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x8; q1 = 0x80 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_809259c(0x0, 0x2);
    __Func_809259c(0x1, 0x2);
    __Func_809259c(0x2, 0x2);
    __Func_80925cc(0x3, 0x2);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x8, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x2; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x2, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN2; q0 = 0x0; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x1; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x2; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x3; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    __Func_8093040(0x3, 0x0, 0x14);
    __Func_8092adc(0x8, 0xa0 << 7, 0x14);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x0; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x1, 0x1);
    __CutsceneWait(0x14);
    __Func_8093040(0x1, 0x0, 0x14);
    __Func_809280c(0x0, 0x1, 0x0);
    __Func_809280c(0x2, 0x1, 0x0);
    __Func_809280c(0x3, 0x1, 0x0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_SetAnim(0x2, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x3, 0xd0 << 8, 0x0);
    __CutsceneWait(0x14);
    { PIN2; q1 = 0x0; q0 = 0x8;
      __Func_8093054(q0, q1); }
    { PIN3; q0 = 0x0; q1 = 0x81 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x81 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x81 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x81 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x1; q1 = 0xc0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093054(0x1, 0x0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x3; q1 = 0x80 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x3, 0x0, 0x14);
    { PIN3; q0 = 0x1; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xe0 << 8; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x2, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x0; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x0; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x2, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    __CutsceneWait(0x14);
    __Func_80925cc(0x8, 0x1);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_8092adc(0x0, 0x0, 0x0);
    { PIN3; q0 = 0x1; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x80 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x80 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x8; q1 = 0x80 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x8, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0xa; q1 = 0xec << 17; q2 = 0x98 << 18;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_8093040(0xa, 0x0, 0x14);
    { PIN3; q0 = 0x0; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0x80 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x0; q1 = 0x80 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x80 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x80 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x80 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x9, 0x0, 0x0);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xa; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xec << 1; q2 = 0x86 << 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xf4 << 1; q2 = 0x80 << 2;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x2; q1 = 0xc0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x0, 0x0, 0x1e);
    { PIN3; q0 = 0x0; q1 = 0xe4 << 1; q2 = 0x80 << 2;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x0; q1 = 0x80 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xa, 0x2);
    __CutsceneWait(0x14);
    __Func_8093040(0xa, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_809259c(0xa, 0x2);
    { PIN2; q0 = 0xa; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    __Func_8093040(0xa, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0xa; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0xa, 0x0, 0x14);
    __Func_809259c(0x8, 0x1);
    { PIN3; q0 = 0x8; q1 = 0x80 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x0; q1 = 0xe0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xa0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x3, 0xe0 << 8, 0x0);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0xa, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0xa, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_809259c(0x11, 0x1);
    __CutsceneWait(0x14);
    __Func_8093040(0xa, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0x80 << 1; q2 = 0x1e;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    __MapActor_SetAnim(0x0, 0x3);
    __CutsceneWait(0x2);
    __MapActor_SetAnim(0x2, 0x3);
    __CutsceneWait(0x1);
    __MapActor_SetAnim(0x3, 0x3);
    __CutsceneWait(0x5);
    __MapActor_DoAnim(0x1, 0x3);
    __Func_809280c(0x8, 0x0, 0x0);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0xa; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xec << 1; q2 = 0xfc << 1;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x14);
    __Func_809259c(0xa, 0x1);
    __CutsceneWait(0x14);
    __Func_8093040(0xa, 0x0, 0x14);
    __Func_809280c(0x8, 0xa, 0x0);
    { PIN3; q0 = 0x8; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0x80 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x80 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitMovement(0x8);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0xa; q1 = 0xec << 1; q2 = 0x8e << 2;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0xec << 1; q2 = 0x86 << 2;
      __Func_80921c4(q0, q1, q2); }
    __Func_80925cc(0x1, 0x2);
    __MapActor_WaitMovement(0xa);
    __Func_8093040(0x1, 0x0, 0x14);
    __Func_809259c(0x8, 0x2);
    { PIN3; q0 = 0x8; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xd0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    __Func_809259c(0x0, 0x2);
    __Func_809259c(0x1, 0x2);
    __Func_809259c(0x2, 0x2);
    __Func_80925cc(0x3, 0x2);
    { PIN3; q0 = 0x8; q1 = 0xec << 1; q2 = 0x80 << 2;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0x8, 0x0, 0x0);
    __Func_8092adc(0x0, 0x0, 0x0);
    { PIN3; q0 = 0x2; q1 = 0x80 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __MapActor_DoAnim(0x2, 0x3);
    __CutsceneWait(0x14);
    __MapActor_GetActor(0)->f5a &= 0xfe;
    __Func_80921c4(0x0, 0xe0 << 1, 0x80 << 2);
    __CutsceneWait(0x1);
    __MapActor_GetActor(0)->f5a |= 1;
    __CutsceneWait(0x14);
    OvlFunc_926_200c1ec();
    __CutsceneWait(0x3c);
    __LearnInnateMove(0x2, 0x90);
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x2, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x0; q1 = 0x80 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x80 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x9, 0xc0 << 6, 0x0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x8; q1 = 0xec << 1; q2 = 0x8a << 2;
      __Func_80921c4(q0, q1, q2); }
    __Func_80925cc(0x8, 0x2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x8; q1 = 0xd0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    __MapActor_DoAnim(0x8, 0x3);
    __CutsceneWait(0x14);
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0xf0 << 1; q2 = 0x87 << 2;
      __Func_80921c4(q0, q1, q2); }
    __Func_8093040(0x8, 0x0, 0x14);
    { PIN3; q0 = 0x8; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0xec << 1; q2 = 0x98 << 2;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0xec << 1; q2 = 0x88 << 2;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xec << 1; q2 = 0x98 << 2;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_WaitMovement(0x9);
    { PIN3; q0 = 0x9; q1 = 0xec << 1; q2 = 0x98 << 2;
      __Func_809218c(q0, q1, q2); }
    __MapActor_SetPos(0xa, 0x0, 0x0);
    __MapActor_WaitMovement(0x8);
    __MapActor_SetPos(0x8, 0x0, 0x0);
    __MapActor_WaitMovement(0x9);
    __MapActor_SetPos(0x9, 0x0, 0x0);
    { PIN3; q0 = 0x2; q1 = 0xf4 << 1; q2 = 0x82 << 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x3, 0x1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x3; q1 = 0x80 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x3, 0x0, 0x14);
    __Func_8092adc(0x0, 0x0, 0x0);
    { PIN3; q0 = 0x2; q1 = 0xa0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x2, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x2, 0x0, 0x14);
    { PIN3; q0 = 0x3; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x1, 0x1);
    { PIN3; q0 = 0x1; q1 = 0xc0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0x0; q0 = 0x1;
      __Func_8093054(q0, q1); }
    __MapActor_DoAnim(0x0, 0x3);
    __CutsceneWait(0x14);
    __Func_80925cc(0x3, 0x2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x80 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x3, 0x0, 0x14);
    { PIN3; q0 = 0x0; q1 = 0x101; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(0x3, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x3, 0x0, 0x14);
    { PIN3; q0 = 0x0; q1 = 0x81 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0x0; q0 = 0x3;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x1, 0x3);
        __CutsceneWait(0x14);
        __Func_8093040(0x1, 0x0, 0x14);
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
    } else {
        __CutsceneWait(0x14);
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
        __MapActor_DoAnim(0x3, 0x3);
        __CutsceneWait(0x14);
        __Func_8093040(0x3, 0x0, 0x14);
    }
    __Func_80925cc(0x1, 0x1);
    __Func_8093040(0x1, 0x0, 0x14);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x1; q1 = 0xe0 << 1; q2 = 0x80 << 2;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xe0 << 1; q2 = 0x80 << 2;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(0x3, 0x0, 0x0);
    __MapActor_WaitMovement(0x1);
    __MapActor_SetPos(0x1, 0x0, 0x0);
    __CutsceneWait(0x14);
    __Func_809280c(0x0, 0x2, 0x14);
    __CutsceneWait(0x1e);
    __Func_809280c(0x2, 0x0, 0x14);
    __Func_8093040(0x2, 0x0, 0x14);
    { PIN3; q0 = 0x0; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809259c(0x2, 0x2);
    __MapActor_Surprise(0x2, 0x81 << 1);
    __CutsceneWait(0x3c);
    __MapActor_DoAnim(0x2, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x2, 0x0, 0x14);
    { PIN3; q0 = 0x2; q1 = 0xe0 << 1; q2 = 0x80 << 2;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(0x2, 0x0, 0x0);
    __CutsceneEnd();
    __SetFlag(0x895);
}
