/* Func_8093af8 -- NON-MATCHING, 29 encodings of 124.  SIZE AND RELOCATIONS IDENTICAL,
 * instruction count identical (124 = 124).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_8a000/8093af8.c \
 *     asm/rom_8a000/rom_93304_a_c_c_c_c_a.s --func Func_8093af8
 *
 * BLOCKER: RELOAD-SCRATCH ROUND-ROBIN *PHASE*, in the RELOAD pass.  ALL 29 differing
 * encodings trace to ONE decision: at insn 192 the constant -0x2fff gets r2 where the ROM
 * gets r3, and the r2/r3 roles then stay swapped at the loop-tail -1 reload and at the
 * tail's `mov rX, r10`.  Reload sites 1-11 agree with the ROM REGISTER FOR REGISTER; from
 * site 12 the ROM is exactly ONE STEP FURTHER along the round robin over {r2, r3}.
 *
 * THREE MEASURED FACTS THAT NARROW THIS, and the first CONTRADICTS THE DOC'S DISCRIMINATOR:
 *
 *   1. `.18.greg`'s find_reload_regs line says "Using reg 3 for reload 0" at insn 192 --
 *      THE PASS THAT PICKS THE SPILL SET ALREADY PREFERS r3.  The r2 comes from
 *      allocate_reload_reg overriding it in reload_as_needed.  So docs/elevation.md's
 *      "read `Using reg N`" discriminator DOES NOT SEE THIS DEFECT, and the spill set here
 *      is already {2,3}, the same width as the ROM's.  This is NOT the narrow-spill-set
 *      variant -- for that, see the file-mate park 808bb2c.
 *   2. `last_spill_reg` IS RESET PER FUNCTION.  Prepending a dummy function that consumes
 *      a reload left the residue at exactly 29, so THE PHASE CANNOT BE FIXED FROM A
 *      FILE-MATE and a single-function candidate is legitimate here.
 *   3. ADDING A RELOAD EARLIER DID NOT SHIFT THE PHASE.  An extra `if (ref->f0c ==
 *      0x12345678) return 0;` at the top -- a real const reload, +3 instructions -- left
 *      `ldr r2,=0xffffd001` unchanged.  That CONTRADICTS the simple "add a reload
 *      somewhere else" cure and is the measured negative worth carrying.
 *
 * Inert against this residue, all measured singly: 11 declaration-order positions for `i`;
 * 6 init-statement orders (only `best = 0;` before `bestd = 0x28;` mattered and it fixed
 * the PROLOGUE, not this); 12 tail spellings; 5 `(short)` cast spellings; 4 `u16 ang`
 * spellings; an `fp` spelling; a named `u8 *` for the +0x54 byte load; --no-rerun-cse.
 *
 * ================ BATCH 334, BRIEF C: THE FIGURE IS A DISTANCE, AND THE
 * ================ PARK'S "ONE DECISION" CLAIM IS INCOMPLETE
 *
 * RE-MEASURED: the claim line at the top of this file REPRODUCES EXACTLY, and it
 * is a TRUE DISTANCE -- ref and ours are both 124 encodings, size and relocations
 * identical.  reports/park-figure-audit.md lists this park in its gap-of-one
 * shortlist as ref 116 / ours 115; that pair is the SIXTEEN-BIT-ONLY instruction
 * count, and a PAD absorbs the halfword, so the STREAM lengths are equal and this
 * park does NOT belong on a misalignment list.  The honest reading of the two
 * numbers together: the ROM has ONE MORE 16-BIT INSTRUCTION than we do and we
 * have one more pad halfword.  Anyone handed this park as "one instruction apart
 * in length" has been handed a different problem from the one in the file.
 *
 * ==================== THE MISSING INSTRUCTION, LOCATED ====================
 *
 * The claim that "ALL 29 differing encodings trace to ONE decision" -- the r2/r3
 * reload-scratch swap -- is REFUTED as an accounting of the residue, though the
 * swap itself reproduces.  tools/aligncmp.py's hunk list puts a second, separate
 * difference in the TAIL, and it is the missing instruction:
 *
 *     rom    mov r3,r10 / mov r0,#0 / cmp r3,#0 / beq END
 *            mov r2,r10 / ldr r3,[r2,#0x50]
 *     ours   mov r2,sl  / mov r0,#0 / cmp r2,#0 / beq END
 *                         ldr r3,[r2,#80]
 *
 * THE ROM COPIES `best` OUT OF r10 TWICE -- once into a low register for the null
 * test, and AGAIN, in the block after the branch, for the dereference.  We copy it
 * once and keep that low copy live across the branch.  So the ROM's low copies are
 * two short-lived values re-materialised per basic block; ours is a single pseudo
 * spanning the branch.  That is where the instruction count differs, and it is
 * independent of the r2/r3 phase.
 *
 * ================ THE TAIL IS NOT REACHABLE BY SPELLING ================
 *
 * Eight tail spellings, measured this batch.  Every one is BIT-IDENTICAL to the
 * body above -- same stream, same aligned-equal count, same hunk list -- which by
 * the rule in src/rom_8a000/rom_93304_c_a_c.c ("if an edit does not move the
 * allocator's inputs, the edit did not happen") means these are NOT inert levers;
 * the edits never reached flow at all:
 *
 *   the sprite pointer named in its own local
 *   the loaded halfword named in its own local
 *   the `short *` named in its own local
 *   both of the above named, as two statements
 *   a second actor local assigned from `best`
 *   an early `return res` instead of the nested test
 *   `&&` instead of the nested ifs
 *   the body's own spelling (control)
 *
 * This matters more than the eight rows: the previous header's inert list is also
 * mostly tail spellings ("12 tail spellings", "5 cast spellings", "4 u16 ang
 * spellings"), and the eight above suggest that WHOLE CLASS was flat for the same
 * reason -- it was never reaching the compiler's intermediate form.  A tail edit
 * on this function should be screened by `-da`'s dumps before its figure is
 * believed either way.
 *
 * >> NAMED REMAINING QUESTION, and it is narrower than the previous one: what
 *    keeps OUR low copy of `best` alive across the `beq`?  If the copy is reload's
 *    (r10 is a high register and both `cmp` and the `ldr` base need a low one),
 *    then the single copy is the anomaly and the thing to read is whether
 *    reload1.c's post-reload CSE is deleting the second re-materialisation, and
 *    whether its value table is reset at the block boundary.  If instead the copy
 *    is an allocator-assigned pseudo, the thing to read is why `best` has a low
 *    pseudo at all when the ROM reaches r10 directly at every use.  Those are
 *    different files; name which before editing C again.
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
