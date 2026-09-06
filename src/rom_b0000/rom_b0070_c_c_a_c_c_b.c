/* Func_80b2f4c -- 0x080b2f4c, from goldensun/asm/rom_b0000/rom_b0070_c_c_a_c_c.s.
 *
 * The shop sprite's per-frame step. State 0 seeds the sprite from its home
 * point twice -- once through a wide randomised offset that is handed to the
 * placement call, once through a narrow one that becomes the sprite's own
 * position -- then arms the mover and advances the state. States 1 and 2 poll
 * the mover and, when it reports done, either rewind the state to 0 or hand
 * over to the finisher.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_b0000/rom_b0070_c_c_a_c_c_a.o and
 * asm/rom_b0000/rom_b0070_c_c_a_c_c_c.o in goldensun/stage1.ld.
 *
 * TEMPLATE: src/overlays/rom_7e3e08/ovl_30_c_c_c_c_b_b.c
 * (OvlFunc_957_200ba30) is the same three-state shape with the same callee set.
 * ITS HEADLINE LEVER DOES NOT APPLY HERE AND MUST NOT BE COPIED. That file's
 * whole finding is that a byte-wide `-1` needs an int-typed name to come out as
 * `sub` rather than `add r3, #0xff`. THIS sibling does not decrement at all: its
 * state-1 arm stores the poll's zero RESULT (`strb r0` with r0 the value just
 * compared against 0), not `*p - 1`. Writing the template's decrement costs
 * 25 encodings and four bytes. The levers were re-measured, not inherited.
 *
 * TWO LEVERS, both about ORDER, and neither about naming.
 *
 *   STATEMENT ORDER IS STORE ORDER, twice. The ROM writes the mover's two
 *   result words low-then-high (+0xc then +0x10) and the two armed constants
 *   low-then-high (+0x20 = 0x20000 then +0x24 = 0x6666). Both pairs are
 *   independent stores that gcc-2.96 emits in source order and does not
 *   schedule past each other; swapping either pair costs 4 and 5 encodings
 *   respectively, and the +0x20/+0x24 swap notably does NOT change the length
 *   -- the length tell is silent on it, so the diff TEXT is the only evidence.
 *   Note the template writes ITS pair the other way round, which is exactly why
 *   this had to be read off this ROM rather than carried over.
 *
 *   THE EPILOGUE POPS INTO r0, SO THE FUNCTION IS void. `pop {r0} / bx r0`
 *   is positive evidence of a void return (the read-count rule's companion,
 *   already on file in rom_b0070_c_c_a_c_b.c). Declaring this `int` leaves the
 *   allocation untouched and changes only `pop {r1} / bx r1` -- 2 encodings.
 *
 * NO PIN, NO BARRIER, NO SCAFFOLDING. r6 (the entity), r7 (the state byte) and
 * r8 (the state pointer) all reach callee-saved registers on statement
 * structure alone, because all three are live across the two Random /
 * vec3_translate call pairs. Nothing needed a call-clobbered pin and nothing
 * needed a hoist.
 *
 * FIVE ALTERNATIVES ALSO MATCH BYTE-FOR-BYTE, recorded because the negative is
 * the useful part -- each one LOOKS like it is being dictated by the assembly
 * and is not:
 *
 *   - `*p = 0;` in place of `*p = r;`, and `*(char *)(e + 0x42) = 0;` in place
 *     of `= k`. Inside those branches gcc knows the value is zero and reuses
 *     the register either way. This is the standing "a value that is provably
 *     constant inside its branch is NOT evidence" rule; the ROM's `strb r0`
 *     and `strb r7` are NOT statements about the source.
 *   - deleting the `p` local and writing `e + 0x40` out at all three uses.
 *   - `signed char *p` with a bare `*p` instead of `char *p` plus a cast.
 *   - `0xa0 << 14` / `0x80 << 11` / `0x80 << 10` for the three constants
 *     instead of 0x280000 / 0x40000 / 0x20000. gcc's thumb mov+lsl split takes
 *     the smallest legal shift, so the ROM's shift amounts are gcc's own
 *     arithmetic and say nothing about the spelling.
 *
 * MEASURED (rom 80 lines, 176 bytes, 79 encodings, 8 relocations):
 *   as written                                          80 lines, MATCH
 *   `*p = 0` / `*(e+0x42) = 0` literals                  80, MATCH
 *   `p` local deleted, expression written out            80, MATCH
 *   `signed char *p`, no cast                            80, MATCH
 *   `0xa0 << 14` etc. instead of hex constants           80, MATCH
 *   +0x24 stored before +0x20                            80, 5 encodings
 *   +0x10 stored before +0xc                             80, 4 encodings
 *   declared `int` instead of `void`                     80, 2 encodings
 *   the template's `int n = *p; *p = n - 1;`             82, 25 encodings
 *   the two seed words hoisted into named locals         88, 85 encodings
 *
 * The last five are the discriminating perturbations: this screen is not
 * trivially green. The seed-hoist row is the sharpest -- naming the two words
 * loaded from +0x14/+0x18 and reusing them for the second vec3_translate call
 * destroys the whole allocation from the first instruction, because the ROM
 * RE-READS both fields after the placement call rather than carrying them.
 *
 * Flag group: the DEFAULT one. No Makefile rule is needed -- the stock
 * `asm/%.o: src/%.c` rule with GCC296_CFLAGS produces this.
 *
 * Verified with tools/objcmp.py against both the original multi-function .s and
 * the isolated split piece: 176 bytes, 79 encodings and 8 relocations
 * identical.
 */
extern int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern void _Func_809ba5c(char *e, int a, int b);
extern int _Func_809ba34(char *e);
extern void _Func_809bb34(char *e);

void Func_80b2f4c(char *e)
{
	char *p;
	int v[3];
	int k;

	p = e + 0x40;
	k = *(signed char *)p;
	if (k == 0) {
		v[0] = *(int *)(e + 0x14);
		v[2] = *(int *)(e + 0x18);
		vec3_translate(0x280000, Random(), v);
		_Func_809ba5c(e, v[0], v[2]);
		v[0] = *(int *)(e + 0x14);
		v[2] = *(int *)(e + 0x18);
		vec3_translate(0x40000, Random(), v);
		*(int *)(e + 0xc) = v[0];
		*(int *)(e + 0x10) = v[2];
		*(int *)(e + 0x20) = 0x20000;
		*(int *)(e + 0x24) = 0x6666;
		*(char *)(e + 0x42) = k;
		*p = *p + 1;
	} else if (k == 1) {
		int r = _Func_809ba34(e);
		if (r == 0) {
			*p = r;
		}
	} else if (k == 2) {
		if (_Func_809ba34(e) == 0) {
			_Func_809bb34(e);
		}
	}
}
