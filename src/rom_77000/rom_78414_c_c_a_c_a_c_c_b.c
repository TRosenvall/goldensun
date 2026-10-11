/* Func_8078870 (0x08078870) -- MATCHING.
 *   OK Func_8078870 -- 84 bytes, 40 encodings and 1 relocations identical
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_77000/rom_78414_c_c_a_c_a_c_c_b.c \
 *     asm/rom_77000/rom_78414_c_c_a_c_a_c_c_b.s --whole
 *
 * SPLIT REQUIRED, text-only; tools/datacheck.py prints nothing.
 *   tools/split_s.py asm/rom_77000/rom_78414_c_c_a_c_a_c_c.s Func_8078870
 *     -> _b.s Func_8078870 (46 lines), _c.s Func_80788c4 (79 lines).
 *   No _a.s: the target is the FIRST function in the file.  The piece-mate
 *   Func_80788c4 stays parked at fourteen and its park repoints to _c.s.
 * Pins: 0.  Devices: none.  No flag group.  Production flags.
 *
 * GetItemProperty.  Walks fifteen halfword inventory records starting at
 * a + 0xd8, and for the first record whose 0x200 bit is set and whose
 * GetItemInfo byte 2 equals the caller's selector, returns the record's low
 * nine bits; zero if none matches.
 *
 * ===================================================================
 * THIS PARK'S LANGUAGE-LEVEL BOUND IS REFUTED, BY THE CONSTRUCT THE PARK
 * ITSELF NAMED AS AN OPEN DIMENSION AND DID NOT TRY.
 * ===================================================================
 *
 * The standing claim, built over five batches and roughly 140 measured
 * spellings, was: the residue is three defects; two of them (the 0x200 chain
 * and the 0x1ff chain hoisting after the parameter copy and after the pointer
 * init) are cured by holding each mask in a named local, and the third -- cse1
 * commoning the first halfword read with the second, so one `ldrh` is emitted
 * where the ROM emits two -- is caused by that same naming.  Hence:
 *
 *   "To be narrow the mask must be constant-foldable; to fix the prologue order
 *    it must survive as a pseudo.  No C spelling of a 0x200 mask is both, so D1
 *    and D3 cannot both be cured.  This is a language-level bound, not a search
 *    result."
 *
 * Every clause of the analysis behind that is correct and reproduces.  The
 * quantifier at the end is what fails.  The park's own batch-332 note had
 * already found the hole and written it down as an open dimension:
 * explow.c:896-913 sends only the SCALAR type codes through PROMOTE_MODE at
 * :898-901, and RECORD, UNION and ARRAY fall out of the default arm at :912
 * UNPROMOTED.  So a one-member aggregate holds a sixteen-bit value in a pseudo
 * of its own width with no extension insns for move_movables to hoist -- which
 * is exactly the shape the bound says the language does not contain.  The note
 * ended "whether that reaches a MASK is a separate question and was not tried
 * here ... stated as an open dimension, not as a result."
 *
 * IT REACHES A MASK.  `struct HWord { unsigned short v; }` with `m.v = 0x200;`
 * is simultaneously
 *   - a COMPONENT_REF of sixteen-bit unsigned type, so build_binary_op's
 *     narrowing block (c-typeck.c:2354-2355, cases at :2385-2412) matches on
 *     its FIRST case -- both operands narrowed from the same precision with the
 *     same signedness -- and the `and` stays in HImode, so the first read stays
 *     a plain mem:HI, is not character-for-character the second read's
 *     expression, and all THREE `ldrh` survive; and
 *   - not an INTEGER_CST in the front end, so it is not folded away and it
 *     survives into RTL as a pseudo assigned before the pointer init, which is
 *     what the prologue's LUID order needs.
 *
 * That is both halves at once, and it took the figure from five to three.
 *
 * THE LAST TWO WERE `i = 0` AND THEY ARE STATEMENT ORDER AFTER ALL.  With the
 * masks in aggregates the residue was a single adjacent transposition: the
 * counter's zero against the first high-register copy.  Assigning the counter
 * FIRST, ahead of both masks, closes it.  Measured: counter first is exact;
 * counter between the masks is two; counter after both masks is three; all
 * twenty-four orderings were swept, and crossed with an empty-asm barrier in
 * each of five positions (one hundred and twenty rows) -- nothing below the
 * ordering alone, so the barrier that closed three functions in this family is
 * confirmed inert here, exactly as the park's batch-330 rows said.
 *
 * OWNER RULING 4 IS NOT REOPENED AND IS NOT NEEDED.  All three reads of
 * `*(unsigned short *)p` are plain; there is no volatile qualifier anywhere in
 * this body.  The ruling recorded the volatile 2 as "a figure about the
 * blocker" and kept the device-free 5; the device-free figure is now zero.
 *
 * WHAT SURVIVES FROM THE PARK, unchanged: the D1/D2 chain-order analysis and
 * its LUID reading of rank_for_schedule's final rung; the cse-hash argument
 * that no type-based alias difference can separate the reads (canon_hash's
 * `case MEM`, cse.c:2252-2267, does not hash the alias set at all); the
 * PROMOTE_PROTOTYPES / PROMOTE_FUNCTION_ARGS reason an `unsigned short`
 * parameter cannot work (arm.h:2363, :611); and the PROMOTE_MODE reason a
 * DECLARED halfword local cannot (arm.h:597-606).  The aggregate is the one
 * escape from the last of those, and it is the whole of the news here.
 */
extern unsigned char *GetItemInfo(unsigned int id);

struct HWord { unsigned short v; };

int Func_8078870(unsigned char *a, int kind)
{
	int i;
	unsigned char *p;
	unsigned char *info;
	struct HWord m;
	struct HWord n;

	i = 0;
	m.v = 0x200;
	n.v = 0x1ff;
	p = a + 0xd8;
	do {
		if (*(unsigned short *)p & m.v) {
			info = GetItemInfo(*(unsigned short *)p);
			if (info[2] == kind)
				return *(unsigned short *)p & n.v;
		}
		i++;
		p += 2;
	} while (i <= 0xe);
	return 0;
}
