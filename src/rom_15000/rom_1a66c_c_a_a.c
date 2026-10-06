/* Func_801a910 -- asm/rom_15000/rom_1a66c_c_a_a.s   [0x0801a910]
 *
 * MATCHING.  objcmp --func: OK, 88 bytes, 43 encodings and 1 relocation
 * identical.  objcmp --whole: OK whole file, same figures.  Was a park at ten
 * differing of 43 (and at twenty-seven before that).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_1a66c_c_a_a.c asm/rom_15000/rom_1a66c_c_a_a.s \
 *     --func Func_801a910
 *   ... and the same with --whole (the .s holds this function alone).
 *
 * SPLIT: none.  asm/rom_15000/rom_1a66c_c_a_a.s holds exactly ONE
 *   `.thumb_func_start` (Func_801a910) and no data; `tools/datacheck.py` exits 0
 *   silently and `tools/split_s.py` has nothing to cut, so this is a WHOLE-FILE
 *   conversion with no object-path change.
 * PINS: 0.  `tools/shimcount.py` reports none; no `register ... __asm__`, no
 *   inline asm, and no per-file flag group (objcmp prints no "built with" line,
 *   i.e. the production flag set unadjusted).
 *
 * ======================= WHAT THE LAST TEN WERE =======================
 *
 * The park had the two arms right in every respect except arm B's SHAPE, and it
 * stated the blocker as two things that "both have to give in the same edit":
 * that writing arm B with two pointers lets `b` out of LO_REGS (worth a jump to
 * thirty-nine or forty differing), and that the returned pointer then takes r0
 * from the return-value copy preference (`set_preference`, global.c:1016),
 * costing two instructions.  Its NEXT MOVE was "break `p`'s r0 copy preference
 * WITHOUT letting `b` out of LO_REGS".
 *
 * *** BOTH HALVES OF THAT DIAGNOSIS ARE REFUTED, AND NEITHER NEEDED BREAKING.
 * THE DIMENSION NOBODY VARIED WAS HOW MANY NAMES THE TWO ARMS SHARE. ***
 *
 * Every earlier two-pointer probe REUSED arm A's locals in arm B -- the same
 * `q` for both flag walkers, the same `off`, the same `i`, the same `f`.  Arm B
 * needs a pointer where arm A needs an offset, so reusing one name forces one
 * pseudo to be both, and that single pseudo is what dragged `b` out of r4: with
 * `q` serving both arms its reference profile is the union of the two, and
 * `.17.lreg`'s class choice for register 33 follows.  Give arm B its OWN two
 * pointers and its OWN flag temporary -- `p`, `r`, `f`, declared after arm A's
 * `q`, `i`, `off` -- and the arms stop competing: `b` stays in r4 with no pin,
 * the returned pointer lands in r2 and the flag walker in r0 all by itself, and
 * `mov r0,r2` appears at arm B's exit exactly as the ROM has it.  Ten -> three.
 *
 *   >> THE NUMBER OF LOCALS IS A LEVER IN ITS OWN RIGHT, separate from their
 *   >> types, their order and their scope.  Two exclusive arms that want
 *   >> different things from "the same" variable are two variables.  Measured
 *   >> here as ten -> three on the first try, after four batches of spellings
 *   >> that all shared the names. <<
 *
 * THE LAST THREE WERE arm B's preheader order, and ONE permutation of three
 * statements closes it: *** `i = 0;` FIRST, then the returned pointer, then the
 * flag pointer. ***  The ROM's preheader is
 *     mov r2,r4 / mov r0,r4 / mov r1,#0 / add r2,#0x68 / add r0,#0x72
 * -- the counter's `mov r1,#0` sits BETWEEN the two base copies and the two
 * adds.  Each `mov rX,r4 / add rX,#imm` pair is reload's copy plus its add, so
 * the two copies are hoisted together and the zero has to be scheduled into the
 * gap; writing the zero first is what gives it the priority to land there.
 * With the pointers written first it comes out last and three encodings differ;
 * with the flag pointer written before the returned one the body is two
 * instructions short.
 *
 * MEASURED (ref 43 encodings / 42 instructions throughout unless noted):
 *   arm B's own `p`, `r`, `f`, with `i = 0;` first              ** MATCH **
 *   the same with the pointers written before `i = 0;`            3 differing
 *   the same with `p` declared after `r`                          3 differing
 *   the same without the early flag-pointer increment             9 differing
 *   arm B's locals block-scoped inside a brace after the if      11 differing
 *   both arms block-scoped                                       11 differing
 *   the flag pointer assigned before the returned pointer        40 instructions,
 *     two short -- a COUNT, not a distance
 *   (the standing park body, one pointer plus an offset)         10 differing
 *
 * ======================= WHAT THE FUNCTION IS =======================
 *
 * Two record tables inside the object at `iwram_3001e98`, searched for the
 * first entry whose halfword flag at +0xa is zero; the caller's argument picks
 * the table.  Seven records of 0x34 from +0x68 (so the last ends at +0x1d4),
 * then five records of 0x34 from +0x1d4.  The flag offsets 0x72 and 0x1de are
 * +0xa into each, and the ROM builds the two large ones as `0xef << 1` and
 * `0xea << 1`, which is why they are spelled that way.  A null return means the
 * table is full.
 *
 * THE LOOPS MUST BE WRITTEN AS A LABEL PLUS A BACKWARD `goto`, and that is the
 * park's finding, kept verbatim because it is worth more than this landing: a
 * `for`, `while` or `do` statement emits the `NOTE_INSN_LOOP_BEG` /
 * `NOTE_INSN_LOOP_END` pair that `find_and_verify_loops` (loop.c:2751-2944)
 * needs, and that transformation INVERTS the early exit and relocates the
 * return code behind the loop -- the opposite polarity from the ROM's
 * `cmp r3,#0 / bne <increment> / <return code> / <increment>`.  With no loop
 * notes loop.c sees no loop, so it cannot fire, and there is no loop-invariant
 * motion either, which is why the ROM builds `0xea << 1` inside the loop body
 * instead of hoisting it to the preheader.
 *
 * The declaration `extern char *iwram_3001e98;` is this file's own; the landed
 * sibling src/rom_15000/rom_1a66c_a_c.c declares the same object as
 * `extern unsigned char *iwram_3001e98;` and this function as returning
 * `struct Node *`.  Separate translation units, so the two spellings do not
 * meet; reconciling them into a shared header is pass-4 work.
 */
extern char *iwram_3001e98;

char *Func_801a910(int alloc)
{
	char *b;
	char *q;
	int i;
	int off;
	char *p;
	char *r;
	int f;

	b = iwram_3001e98;
	if (alloc != 0) {
		i = 0;
		q = b + (0xef << 1);
		off = 0;
	loopA:
		if (*(unsigned short *)q == 0)
			return b + off + (0xea << 1);
		i++;
		q += 0x34;
		off += 0x34;
		if (i != 5)
			goto loopA;
		return 0;
	}
	i = 0;
	p = b + 0x68;
	r = b + 0x72;
loopB:
	f = *(unsigned short *)r;
	r += 0x34;
	if (f == 0)
		return p;
	i++;
	p += 0x34;
	if (i != 7)
		goto loopB;
	return 0;
}
