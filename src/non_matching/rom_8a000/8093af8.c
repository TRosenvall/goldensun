/* Func_8093af8 -- NON-MATCHING.  HOLDS.  Batch 343 brief C re-derived the
 * figure and CORRECTED WHAT IT MEASURES.
 *
 *   STREAM figure            29 of 124   (ref 124 encodings / ours 124)
 *   16-BIT INSTRUCTIONS      ref 116 / ours 115   ** UNEQUAL, gap 1 **
 *   32-bit entries           8 / 8   EQUAL   (pool words AND `bl` calls)
 *   pad halfwords            ref 0 / ours 1
 *   size, relocations        EQUAL
 *   first differing index    82
 *   ALIGNED residue          110 of 124 aligned-equal, 22 differing in 7 hunks
 *   PINS 0 (tools/shimcount.py prints NO ROWS).  No devices, no flags.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/8093af8.c \
 *     asm/rom_8a000/rom_93304_a_c_c_c_c_a.s --func Func_8093af8
 *
 * SPLIT SHAPE: none.  tools/piece.py -> rom_93304_a_c_c_c_c_a.s, 1 function,
 * all 1 parked.  tools/datacheck.py exits 0 silently (no data section).
 *
 * ###########################################################################
 * ## THE 29 IS **NOT A DISTANCE**.  BATCH 334'S INFERENCE WAS WRONG AND SO
 * ## IS EVERY BRIEF THAT HAS REPEATED IT.  READ THIS BEFORE USING THE FIGURE.
 * ###########################################################################
 *
 * Batch 334 wrote: "a PAD absorbs the halfword, so the STREAM lengths are equal
 * and this park does NOT belong on a misalignment list."  **The premise is
 * right and the inference does not follow.**  objcmp says so itself, in a note
 * it prints unprompted on this very park:
 *
 *     XX INSTRUCTION COUNT  ref 116, ours 115
 *        NOTE: size and encoding count MATCH -- a pad is absorbing the
 *        difference, so the positional figure below measures MISALIGNMENT,
 *        not distance.  Re-screen with tools/tryc.py --align.
 *
 * Two corrections of detail, both measured:
 *   * THE ABSORBER IS A PAD HALFWORD, NOT A 32-BIT ENTRY.  The 32-bit entries
 *     are 8 against 8, EQUAL.  Anyone told "a 32-bit entry absorbs it" has the
 *     wrong object.
 *   * EQUAL STREAM LENGTH WITH UNEQUAL INSTRUCTION COUNTS DOES NOT MAKE
 *     POSITIONS LINE UP.  tools/aligncmp.py -v locates both ends:
 *         the MISSING INSTRUCTION is at ref indices 97-103 (our 96-102)
 *         the PAD is at OUR index 117  ("hunk insert ref[118:118] ours[117:118]
 *                                        ours 0000 .short 0x0000")
 *     so the ~20 stream positions BETWEEN them are shifted by one.  The 29 is
 *     part distance and part phase, and nothing can be concluded from its
 *     value alone.
 *
 *   >> USE THE ALIGNED NUMBER FOR THIS PARK: **110 of 124 aligned-equal, 22
 *      differing/ins/del in 7 hunks** (tools/aligncmp.py).  Quote the 29 only
 *      with the 116/115 beside it. <<
 *
 * ###########################################################################
 * ## AND THE TWO RESIDUES ARE **ONE DEFECT**, NOT TWO.  BATCH 334'S
 * ## "INDEPENDENT OF THE r2/r3 PHASE" IS REFUTED.
 * ###########################################################################
 *
 * Batch 334 located the missing instruction correctly -- the ROM copies `best`
 * out of r10 for the null test AND AGAIN for the dereference, we do it once --
 * and then wrote "it is independent of the r2/r3 phase".  IT IS NOT.  The two
 * are the same decision seen twice:
 *
 *     rom    mov r3,r10 / mov r0,#0 / cmp r3,#0 / beq END
 *            mov r2,r10 / ldr r3,[r2,#0x50]
 *     ours   mov r2,sl  / mov r0,#0 / cmp r2,#0 / beq END
 *                         ldr r3,[r2,#80]
 *
 * The ROM's null-test scratch is **r3**, and the very next insn is
 * `ldr r3,[r2,#0x50]` -- which DESTROYS r3.  So the ROM *cannot* reuse the
 * test's copy as the dereference base and must materialise a fresh `mov r2,r10`.
 * Ours lands the test's copy in **r2**, and `ldr r3,[r2,#80]` uses r3 as the
 * DESTINATION and r2 as the BASE, so our r2 survives the branch and reload has
 * nothing left to materialise.
 *
 *   >> SO FIXING THE r2/r3 RELOAD-SCRATCH PHASE SHOULD PRODUCE THE MISSING
 *      INSTRUCTION FOR FREE, and the park has ONE blocker, not two.  That also
 *      means the instruction-count gap is NOT separately reachable and should
 *      not be attacked on its own -- which is what the previous header's
 *      "NAMED REMAINING QUESTION" sent the next agent off to do. <<
 *
 * ###########################################################################
 * ## THE PREVIOUS HEADER'S OPEN QUESTION, ANSWERED FROM THE DUMPS.
 * ###########################################################################
 *
 * It asked: "what keeps OUR low copy of `best` alive across the `beq`?  If the
 * copy is reload's ... If instead the copy is an allocator-assigned pseudo ...
 * Those are different files; name which before editing C again."
 *
 * ANSWER: **`best` is an ALLOCATOR-assigned pseudo and the low copies are
 * RELOAD'S.**  `.17.lreg` has `Register 34 used 6 times across 140 insns; set 2
 * times; user var; crosses 2 calls; pref LO_REGS; pointer` and `.18.greg` has
 * `34 in 10` -- so `best` lives in r10 for its whole range, with LO_REGS as its
 * PREFERRED class that it did not get.  Every low copy is therefore reload
 * satisfying a Thumb operand constraint, not an allocation.
 *
 * AND THE COUNT IN THE PREVIOUS HEADER IS OFF BY ONE, which matters for anyone
 * reading `.19.flow2`: we emit **two** reads of sl, not one, and the ROM emits
 * **three**.  Ours are `insn 379  (set (reg:SI 2 r2) (reg/v:SI 10 sl))` in
 * bb 18, the null-test copy, and `insn 245  (set (reg/v:SI 0 r0)
 * (reg/v:SI 10 sl))` in bb 20, the RETURN VALUE.  Both survive unchanged into
 * `.23.sched2` and `.26.mach`.  The one we lack is the DEREFERENCE BASE, and
 * per the section above it is lacking because our test copy is in r2.
 *
 *   >> SO THE FILE TO READ IS reload1.c's SCRATCH CURSOR, not its post-reload
 *      CSE: nothing is deleting a second re-materialisation, because reload
 *      never needed one.  batch 342's finding applies directly -- a scratch
 *      residue that ALTERNATES is ONE CURSOR PHASE, not N choices
 *      (allocate_reload_reg takes the next register off a round-robin cursor,
 *      reload1.c:5003, :4937, reset per function at :821), so with {r2,r3} the
 *      PERIOD IS TWO and ONE EXTRA RELOAD ANYWHERE EARLIER INVERTS EVERY LATER
 *      SITE AT ONCE. <<
 *
 * That last point is in tension with this park's own measured fact 3 ("ADDING A
 * RELOAD EARLIER DID NOT SHIFT THE PHASE", an extra `if (ref->f0c ==
 * 0x12345678) return 0;` at the top, +3 instructions, left `ldr r2,=0xffffd001`
 * unchanged).  BOTH ARE MEASUREMENTS AND BOTH SHOULD BE BELIEVED; the
 * reconciliation to test is that the added reload was in a DIFFERENT BASIC
 * BLOCK from the contested sites and the cursor is not what that probe moved.
 * That is the next experiment, and it is cheap: add a reload inside the LOOP
 * BODY, where sites 1-11 already agree with the ROM register for register, and
 * see whether site 12 onward inverts.
 *
 * ###########################################################################
 * ## AUTOSWEEP: BOTH CHEAP AXES RUN, AND ITS 28 IS A **POSITIONAL ARTIFACT**.
 * ###########################################################################
 *
 * FLAG AXIS -- nothing.  Four groups EXACTLY INERT at 29 (-fno-gcse,
 * -fno-strict-aliasing, -fno-strength-reduce, -fno-rerun-cse-after-loop,
 * -fno-rerun-loop-opt); -O1 107, -fno-schedule-insns2 49, -ffixed-r7 122.
 *
 * ANCHOR AXIS -- autosweep reports `anchor18` at **28**, "Improved 29 -> 28".
 * **DO NOT BELIEVE IT AS AN IMPROVEMENT.**  anchor18 is
 * `do { } while (0);` after `ang = (short)(ang - ref->f06);` inside the
 * `if (r > 0x17)` arm.  Measured both ways:
 *
 *     body       objcmp stream    instructions    aligncmp
 *     parked     29 of 124        116 / 115       110 equal, 22 diff, 7 hunks
 *     anchor18   28 of 124        116 / 115       110 equal, 22 diff, 6 hunks
 *
 * The ALIGNED residue is IDENTICAL -- same 110, same 22.  The anchor merely
 * coalesced two adjacent hunks, which moves `first` from 82 to 83 and drops the
 * positional count by one.  It does not fix the instruction count and it
 * removes nothing.  **This is exactly the trap the misalignment correction
 * above is about, caught on this park's own cheapest axis**, and it is why a
 * positional figure on a different-length stream must never be ranked.
 * The parked body is retained unchanged; the anchor buys nothing real.
 * 2 of 26 anchor placements produced NO ROW (untested, not inert).
 *
 * ###########################################################################
 * ## BATCH 344, BRIEF B.  THE GAP IS 100% POST-LOOP, AND THE MISSING
 * ## INSTRUCTION IS A CONSEQUENCE OF THE r2/r3 PHASE *BY REGISTER CLASS*,
 * ## NOT BY HOPE.  THE PARK'S "FOR FREE" CLAIM NOW HAS A MECHANISM.
 * ###########################################################################
 *
 * GAP ARITHMETIC, counted per REGION as batch 344's brief requires (16-bit
 * instructions; the 8/8 32-bit entries are 2 `bl` plus 6 pool words):
 *
 *     region                                ref   ours   gap
 *     pre-loop   push .. `mov r8, r2`        17     17     0
 *     loop body  .L93b1a .. `bge .L93b1a`    79     79     0
 *     post-loop  `mov r3,r10` .. `bx r1`     20     19    -1
 *     TOTAL                                 116    115    -1
 *
 * So the loop body -- 68% of the function and where 11 of the 13 reload sites
 * live -- is instruction-for-instruction EXACT, and the whole gap is the
 * post-loop block.  Nothing in the loop needs to be added or removed; only the
 * registers inside it are swapped from site 12 on.  **Do not spend a round
 * looking for a missing statement in the loop.**
 *
 * THE MISSING INSTRUCTION IS `mov r2, r10`, and here is WHY it exists in the
 * ROM and not here.  Both sides need a LOW copy of `best` (which lives in r10
 * for its whole range -- `.18.greg` says `34 in 10`) twice in the post-loop:
 * once for `cmp ..., #0` and once as the base of `ldr rD, [base, #0x50]`.  The
 * two uses are in DIFFERENT basic blocks separated by the `beq`, but the second
 * block carries NO code label (the fallthrough target; the label is the `beq`'s
 * own destination further down), so reload's inheritance is NOT forgotten
 * across the edge and the second reload CAN inherit the first one's register.
 *
 *     ROM    test copy lands in r3;  the dereference is `ldr r3, [r2, #0x50]`,
 *            whose OUTPUT is r3.  Inheriting r3 for the base would collide
 *            with the output, so the inheritance is refused and reload has to
 *            materialise a fresh copy -> `mov r2, r10`.
 *     ours   test copy lands in r2;  the dereference is `ldr r3, [r2, #80]`,
 *            output r3, base r2.  r2 still holds `best`, the inheritance is
 *            accepted, and there is nothing left to materialise.
 *
 * `.26.mach` shows our side exactly: insn 379 `(set (reg:SI 2 r2) (reg/v:SI 10
 * sl))` at the head of bb 18, and insn 235 in bb 19 is
 * `(set (reg:SI 3 r3) (mem/s:SI (plus:SI (reg:SI 2 r2) (const_int 80))))`
 * carrying a `REG_EQUIV` that names r2 -- r2 is live across the bb18 -> bb19
 * edge.  So the park's "fixing the phase should produce the missing
 * instruction for free" is upgraded from a guess to a derivation: it is forced,
 * because r3 is the dereference's destination in both compilations.
 *
 *   >> THE COROLLARY, AND IT IS THE TARGETING FACT: THERE IS EXACTLY ONE
 *      BLOCKER AND IT IS THE r2/r3 CURSOR PARITY AT SITE 12.  Any probe that
 *      does not move that parity cannot move this park, whatever its figure. <<
 *
 * AND WHERE SITE 12 ACTUALLY IS, with both streams side by side:
 *
 *     ROM    lsl r3,#16 | asr r0,r3,#16 | ldr r3,=0xffffd001 | cmp r0,r3 | blt
 *            ldr r2,=0x2fff | cmp r0,r2 | bgt
 *     ours   lsl r3,r3,#16 | ldr r2,=0xffffd001 | asr r0,r3,#16 | cmp r0,r2
 *            | blt | ldr r3,=0x2fff | cmp r0,r3 | bgt
 *
 * The `ldr` sitting ABOVE the `asr` in our stream is sched2, not reload:
 * reload emits a constant reload immediately before its `cmp`, which is AFTER
 * the `asr`, where r3 is dead and therefore FREE.  So r3 was available and the
 * cursor still chose r2 -- this is `allocate_reload_reg`'s round robin
 * (`i = last_spill_reg; i++` at reload1.c:5003 and :5009-5013, cursor updated
 * at :4937, reset once per function at :821) and NOT an availability
 * constraint.  The ROM is one advance further along, and because
 * `last_spill_reg` is a single per-function global the extra advance must lie
 * between site 11 and site 12, i.e. between the `bl atan2` and the
 * `ldr =0xffffd001`.  That window is `lsl / lsr / cmp r6,#0x17 / ble /
 * ldrh r3,[r7,#6] / sub / lsl / asr` -- eight instructions, identical on both
 * sides, with one basic-block boundary in it.
 *
 * ---- A SECOND POSITIONAL ARTIFACT, CAUGHT THE SAME WAY AS anchor18 ----
 *
 * A use-site `volatile` on the halfword read,
 * `if (*(volatile short *)best->f50->f28 == kind)`, makes the INSTRUCTION
 * COUNTS EQUAL (116/116) and the stream figure 29 -> **17 of 124**.  IT IS NOT
 * AN IMPROVEMENT.  Aligned: **109 equal / 23 differing in 8 hunks**, against
 * the parked body's 110 / 22 in 7.  The counts equalise for the wrong reason --
 * `volatile` loses the `ldrsh rD,[rB,rI]` form and spends three instructions
 * (`ldrh / lsl / asr`) where the ROM spends two (`mov r2,#0 / ldrsh r3,[r3,r2]`)
 * -- and the `mov r2, r10` is STILL missing.  So the gap closed by +1 in one
 * place and the real defect grew.  **Equal instruction counts are a
 * precondition for reading the positional figure, not evidence of progress**,
 * and on this park the ALIGNED pair is the only number to rank.
 *
 * NINE POST-LOOP TAIL SPELLINGS MEASURED THIS BATCH, every one 29 of 124 at
 * 116/115 with relocations clean, i.e. EXACTLY INERT:
 *   the parked spelling (control) | the dereference through a second name
 *   taken before the test (`a = best;`) | `struct Sprite *s = best->f50;`
 *   named inside the guard | `short *p = best->f50->f28;` named inside the
 *   guard | a `volatile` qualifier on the f50 POINTER read
 *   (`*(struct Sprite *volatile *)&best->f50`) | the TEST on a copy and the
 *   dereference on `best` | `do { } while (0);` between the test and the
 *   dereference | `res` assigned through `iwram_3001e64 + (best -
 *   iwram_3001e64)`.  Only the halfword `volatile` moved anything, and that
 *   move is the artifact above.  This corroborates the park's standing finding
 *   that the tail is not reachable by spelling.
 *
 * ###########################################################################
 * ## EVERYTHING BELOW IS CARRIED FORWARD AND STILL THE MAP.
 * ###########################################################################
 *
 * BLOCKER: RELOAD-SCRATCH ROUND-ROBIN *PHASE*, in the RELOAD pass.  The 29
 * differing encodings trace to ONE decision: at insn 192 the constant -0x2fff
 * gets r2 where the ROM gets r3, and the r2/r3 roles then stay swapped at the
 * loop-tail -1 reload and at the tail's `mov rX, r10`.  Reload sites 1-11 agree
 * with the ROM REGISTER FOR REGISTER; from site 12 the ROM is exactly ONE STEP
 * FURTHER along the round robin over {r2, r3}.  (Batch 334 called the
 * "all 29 trace to one decision" accounting refuted because of the tail; this
 * batch restores it -- the tail IS the same decision.  See above.)
 *
 * THREE MEASURED FACTS THAT NARROW THIS, and the first CONTRADICTS THE DOC'S
 * DISCRIMINATOR:
 *
 *   1. `.18.greg`'s find_reload_regs line says "Using reg 3 for reload 0" at
 *      insn 192 -- THE PASS THAT PICKS THE SPILL SET ALREADY PREFERS r3.  The
 *      r2 comes from allocate_reload_reg overriding it in reload_as_needed.  So
 *      docs/elevation.md's "read `Using reg N`" discriminator DOES NOT SEE THIS
 *      DEFECT, and the spill set here is already {2,3}, the same width as the
 *      ROM's.  This is NOT the narrow-spill-set variant -- for that, see the
 *      file-mate park 808bb2c.
 *   2. `last_spill_reg` IS RESET PER FUNCTION.  Prepending a dummy function
 *      that consumes a reload left the residue at exactly 29, so THE PHASE
 *      CANNOT BE FIXED FROM A FILE-MATE and a single-function candidate is
 *      legitimate here.
 *   3. ADDING A RELOAD EARLIER DID NOT SHIFT THE PHASE (see the reconciliation
 *      proposed above -- the probe was in a different basic block).
 *
 * Inert against this residue, all measured singly: 11 declaration-order
 * positions for `i`; 6 init-statement orders (only `best = 0;` before
 * `bestd = 0x28;` mattered and it fixed the PROLOGUE, not this); 12 tail
 * spellings; 5 `(short)` cast spellings; 4 `u16 ang` spellings; an `fp`
 * spelling; a named `u8 *` for the +0x54 byte load; --no-rerun-cse.
 *
 * THE TAIL IS NOT REACHABLE BY SPELLING.  Eight tail spellings measured in
 * batch 334 are every one BIT-IDENTICAL to the body above -- same stream, same
 * aligned-equal count, same hunk list -- which by the rule in
 * src/rom_8a000/rom_93304_c_a_c.c ("if an edit does not move the allocator's
 * inputs, the edit did not happen") means these are NOT inert levers; the edits
 * never reached flow at all:
 *     the sprite pointer named in its own local
 *     the loaded halfword named in its own local
 *     the `short *` named in its own local
 *     both of the above named, as two statements
 *     a second actor local assigned from `best`
 *     an early `return res` instead of the nested test
 *     `&&` instead of the nested ifs
 *     the body's own spelling (control)
 * A tail edit on this function should be screened by `-da`'s dumps before its
 * figure is believed either way.  BATCH 343 CORROBORATION FROM ANOTHER
 * FUNCTION: the same phenomenon was measured on Func_80c0f98 at scale -- 144
 * crossed statement-order variants, ALL EXACTLY 22, bit-identical.  So this is
 * a CLASS, not a quirk of this park.
 */
typedef unsigned char u8;
typedef unsigned short u16;

struct Sprite {
	u8 pad00[0x28];
	short *f28;
};

struct Actor {
	int f00;
	u8 pad04[2];
	u16 f06;
	int f08;
	int f0c;
	int f10;
	u8 pad14[0x50 - 0x14];
	struct Sprite *f50;
	u8 f54;
	u8 pad55[0x70 - 0x55];
};

extern struct Actor *iwram_3001e64;

extern int Func_8000948(int v);
extern int atan2(int dz, int dx);

struct Actor *Func_8093af8(struct Actor *ref, int kind)
{
	struct Actor *best;
	struct Actor *a;
	int bestd;
	int i;
	int d;
	int dx;
	int dz;
	int r;
	int ang;
	int (*fp)(int);
	struct Actor *res;

	best = 0;
	bestd = 0x28;
	a = iwram_3001e64;
	for (i = 0x3f; i >= 0; i--, a++) {
		if (a->f00 == 0)
			continue;
		if (a == ref)
			continue;
		if (a->f54 != 1)
			continue;
		d = a->f0c - ref->f0c;
		if (d >= 0) {
			if (d > 0x2fffff)
				continue;
		} else if (ref->f0c - a->f0c > 0x2fffff)
			continue;
		dx = (a->f08 - ref->f08) / 0x10000;
		dz = (a->f10 - ref->f10) / 0x10000;
		fp = Func_8000948;
		r = fp(dx * dx + dz * dz);
		if (r >= bestd)
			continue;
		ang = (u16)atan2(a->f10 - ref->f10, a->f08 - ref->f08);
		if (r > 0x17) {
			ang = (short)(ang - ref->f06);
			if (ang < -0x2fff)
				continue;
			if (ang > 0x2fff)
				continue;
		}
		best = a;
		bestd = r;
	}
	res = 0;
	if (best != 0)
		if (*best->f50->f28 == kind)
			res = best;
	return res;
}
