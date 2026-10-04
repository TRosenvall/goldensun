/* Func_80bf574 (0x080bf574) -- NON-MATCHING.
 *
 * NON-MATCHING, 3 of 24 encodings  (MEASURED, batch 323 brief B).
 * Was parked at 15 of 24.  PIN-FREE, and the memory profile MATCHES the
 * reference (ldr=1 ldrb=1 strb=2 -- no MEM flag from tools/crossfire.py).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80bf574.c \
 *     asm/rom_b5000/rom_bbb0c_a_c_c_a_c_a.s --func Func_80bf574
 *
 * --whole reports the SAME 3 (the .s holds this function alone, so there is no
 * section-tail or sibling effect hiding in the --func figure).
 * tools/datacheck.py on the reference is silent: no data section.
 *
 * FIRST, TWO CORRECTIONS TO THE OLD PARK.
 *
 * 1. Its "24 lines against the ROM's 25 -- ONE SHORT -- with 16 differing" was
 *    reporting objcmp's 15, and 15 was MISALIGNMENT, not distance.  The ROM is
 *    23 instructions plus one pool word; the park's body was 22 instructions
 *    plus an alignment half-word plus the pool word.  Both come to 24
 *    encodings, so the COUNT looked equal while the instruction streams were
 *    one apart, and ten of the fifteen "differences" were identical text at a
 *    shifted index.  Only ONE cause was ever present.
 *
 * 2. Its verdict -- "gcc will not produce the pair, because it is right not to
 *    ... There is no source spelling for 'please compute this redundantly'" --
 *    is REFUTED.  There is one, and it is the ordinary one: name the
 *    decremented byte.  `unsigned char v = *p + 0xff; *p = v;` makes the
 *    comparison need a real 8-bit value, so combine can no longer collapse the
 *    zero-extension into the lsl-only form it used for `(x << 24) != 0`, and
 *    the ROM's `lsl`+`lsr` pair appears in place.  The redundancy is not
 *    redundant once the value is also stored.  15 -> 3 on the first candidate.
 *
 * WHAT THE RESIDUE OF 3 IS: A ROTATION OF THREE INSTRUCTIONS, NOT A DISTANCE.
 *
 *     rom    adds r3, #0xff / strb r3, [r1] / lsls r3, #24 / lsrs r3, #24 / cmp
 *     ours   adds r3, #0xff / lsls r3, #24  / lsrs r3, #24 / strb r3, [r1] / cmp
 *
 * Everything else -- all 21 other encodings, the pool word and both
 * relocations -- is exact.  The ROM stores the RAW sum and then zero-extends in
 * place; we zero-extend first and store the extended value.
 *
 * WHY, FROM `.23.sched2`.  sched2 has NO freedom here.  Block 1's dependence
 * table is a single chain -- 33(add) -> 36(lsl) -> 37(lsr) -> 43(store) ->
 * 45(cmp+branch) -- because `v` is a QImode pseudo and the store reads the QI
 * subreg of the SImode register the extension writes.  The ROM needs the store
 * to be a LEAF off the add, parallel to the extension.  So this is an RTL-shape
 * question, not a scheduling one, and no `-fsched` lever can reach it.
 *
 * THE OTHER HORN, AND WHY IT IS A WRONG PROGRAM RATHER THAN A BETTER BODY.
 *
 * Making the store independent of the extension IS possible -- store an `int`
 * and truncate in a separate statement:
 *
 *     t = *p + 0xff;  *p = t;  t = (unsigned char)t;  if (t != 0) ...
 *
 * and that reproduces the ROM's `add / strb / lsl / lsr / cmp / bne` EXACTLY,
 * indices 9-14 and 17-22 all green.  It reads 9, and it is NOT an improvement:
 * crossfire flags it COUNT MEM.  It is 23 instructions against 24 and it has
 * LOST the pool word -- with `u` displaced to r1 the offset 0x146 stays live in
 * r0, and reload_cse_move2add (reload1.c:8840) derives 0x147 from it as
 * `adds r0, #1` instead of loading the ROM's `.word 0x147`.  One fewer `ldr`
 * than the reference is a wrong program, so the 9 is a figure ABOUT this
 * blocker and not a candidate.  Six crossed edits on that body were inert or
 * worse (best 9, next 10).
 *
 * So the two horns are mutually exclusive as measured: the spelling that gets
 * the ORDER right loses the allocation, and the spelling that gets the
 * ALLOCATION right cannot separate the store from the extension.  3 is the
 * better and memory-faithful one.
 *
 * MEASURED, ALL WORSE (tools/crossfire.py, depth 2, base = this body):
 *   final store through `p = u + 0x147`                             8
 *   `*p = *p + 0xff; v = *p;` (re-read after the store)        19, MEM
 *       -- gcc emits a fresh `ldrb` reload, not a truncation; this is the
 *          park's own observation reproduced, and the MEM flag now says why it
 *          can never be right: the reference has one `ldrb`, this has two.
 *   `*p = v = *p + 0xff;` (chained assignment)                      4
 *   `int t` + separate truncation, six crossings            9-18, mostly MEM
 * EXACTLY INERT (candidate prerequisites, all 3): declaration order with `v`
 *   first, `if (v)` for `if (v != 0)`, `if (!*p)` for `if (*p == 0)`,
 *   `*p - 1` for `*p + 0xff`, and writing the companion offset as
 *   `(0xa3 << 1) + 1` instead of `0x147`.
 *
 * STILL RIGHT, inherited from the old park and re-confirmed: the `mov`+`lsl`
 * construction of the even 0xa3<<1 offset, the pooled 0x147 companion offset,
 * the CSEd double read at the top (`ldrb r2 / mov r3, r2 / cmp r3, #0` -- the
 * batch-178 lever, and naming the byte AFTER the store is what collapses it),
 * the `add r3, #0xff` decrement rather than `(*p)--`, the inverted `bne` guard
 * that distinguishes this sibling from its eleven mates, and the shared
 * `mov r0, #0` exit.
 */
extern unsigned char *_GetUnit(void);

int Func_80bf574(void)
{
	unsigned char *u;
	unsigned char *p;
	unsigned char v;

	u = _GetUnit();
	p = u + (0xa3 << 1);
	if (*p == 0)
		goto fail;
	v = *p + 0xff;
	*p = v;
	if (v != 0)
		goto fail;
	*(u + 0x147) = v;
	return 1;
fail:
	return 0;
}
