/* Func_80799b0 -- 0x080799b0, asm/rom_77000/rom_79460_c_c_c_a_c_c.s
 *
 * NON-MATCHING: 108 encodings of 142 differ (objcmp).
 *
 * objcmp --func Func_80799b0, verbatim:
 *   XX ENCODINGS differ in 108 place(s) (ref 142, ours 142)
 *      first at index 14: ref dc75  ours dc74
 *   XX RELOCATIONS differ
 * No SIZE line: 296 bytes both sides, 142 encodings both sides.  The RELOCATIONS line
 * is a two-byte position shift on ONE entry, not a different set -- all four are the
 * same symbols in the same order (Func_80797fc, GetFlag, Func_80797ec, .L84b1c) and
 * three of the four offsets are identical.  So 108 is a true distance, but the figure
 * that ranks variants is tryc --align: 55 instructions in disagreeing regions of 153.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/80799b0.c \
 *     asm/rom_77000/rom_79460_c_c_c_a_c_c.s --func Func_80799b0
 *
 * THE .s HOLDS ONE FUNCTION AND NO DATA SECTION (grep -ci func_start = 1, datacheck
 * reports nothing), so this converts the file WHOLE when it closes.  It reads
 * `.L84b1c`, which is already .global in asm/rom_77000/rom_79460_c_c_c_c_c_c.s and is
 * unaffected by any split of that file.
 *
 * THREE THINGS THIS ROM TELLS YOU, all three now in the source:
 *
 * 1. *** Func_80797ec TAKES TWO ARGUMENTS. ***  The ROM computes r1 -- `mov r1,r5`,
 *    then `mov r1,r4` if buf[ix2] > 9 -- and never reads it again except that `bl
 *    Func_80797ec` follows.  Declared with one parameter, `sel` is dead and gcc
 *    deletes the whole selection, which is exactly the 8 instructions that were
 *    missing:
 *      Func_80797ec(ix)         134 of 142, 280 bytes -- 8 short, size wrong
 *      Func_80797ec(ix, sel)    142 of 142, 296 bytes -- size and count EXACT
 *    This is the lever that made the object the right length.
 *
 * 2. Func_80797fc IS CALLED WITH AN UNINITIALISED MIDDLE ARGUMENT.  The ROM sets r0
 *    and r2 and leaves r1 untouched across `bl Func_80797fc`.  Passing an
 *    uninitialised `int x` reproduces it and costs nothing; the first 14 encodings
 *    match, so the entry sequence including this call is byte-identical.
 *
 * 3. `if (best != -1) return best;` IS DEAD CODE THAT THE ROM EMITS.  `best` is -1
 *    from the first instruction and never written before the test, yet the ROM spends
 *    `mov r0,#1 / neg r0,r0 / cmp r8,r0 / bne` on it.  cse loses the constant across
 *    three conditional branches and two calls, so the test survives while `return
 *    best` folds to `return -1`.  Writing it out is required; leaving it out is four
 *    instructions short.
 *
 * COUNTER SPLIT, single drops from the 142-of-142 baseline (objcmp / tryc --align):
 *      one `i` for both max loops and the table walk     107 / 65
 *      separate counter for the SECOND max loop          105 / 62
 *      separate counter for the TABLE WALK               108 / 55   <- in the file
 *      both of the above                                 108 / 55
 * The two disagree in direction: --align prefers the table-walk split and objcmp's
 * positional count prefers the max-loop split.  Taking --align, per batch 292.
 * `j` declared before `i` is inert.
 *
 * SHAPES CONFIRMED CORRECT (they are byte-identical regions):
 *   - the first max pass walks `ldmia r2!, {r3}` -- that is gcc's SImode post-increment
 *     load, from `buf[i]` with `i` the only index; the second pass cannot use it
 *     because of the `i != ix` skip, so it is `ldr r3,[r2] / add r2,#4`.
 *   - the requirement compare is `buf[j] < req[j] * 10`, and the ROM's
 *     `lsl r2,r3,#2 / add r2,r3 / lsl r2,#1` is `* 10` written as `* 10`.
 *   - the inner loop is rotated with its j == 0 test peeled and exits with j == 4;
 *     `for (j = 0; j <= 3; j++) ... break;` then `if (j == 4)` gives exactly that.
 *   - the table entry is 0x54 bytes: an `int` id then four requirement bytes.  The
 *     walk is `for (n = 0xca; n >= 0; n--)`, strength-reduced by loop.c into the ROM's
 *     two `sub #0x54` pointers with the base in r10 copied to r14.
 *
 * SHIMS: none.
 *
 * BLOCKER: register assignment plus one block placement, both downstream of the same
 * rotation.
 *   a. The ROM keeps the shared max-loop/table index in r0 and `j` in r4; we have them
 *      swapped, and `ix`/`ix2` follow (ROM r5/r4, ours r0/r5).  The first differing
 *      encoding, index 14, is a `bge` whose offset moved by one instruction -- i.e.
 *      everything after it is the same instructions in rotated registers.  gcc uses
 *      r14 as a general register here, which is the REG_ALLOC_ORDER {3,2,1,0,12,14,...}
 *      tail, so the pressure is genuinely at the limit and there is no slack to steer.
 *   b. The loop's success block (`best = n; break;`, the ROM's
 *      `.L79a42: mov r8,r0 / b .L79aac`) is emitted by the ROM BETWEEN the `sel`
 *      selection and the `bl Func_80797ec`, which is why the ROM pays a `b .L79a46`
 *      that we do not.  We emit that block immediately after the entry guards.  Five
 *      loop spellings did not move it.
 */
struct Entry {
	int id;
	unsigned char req[4];
	unsigned char pad08[0x54 - 8];
};

extern void Func_80797fc(int id, int x, int *out);
extern int GetFlag(int id);
extern int Func_80797ec(int i, int sel);
extern struct Entry L84b1c[] __asm__(".L84b1c");

int Func_80799b0(int id)
{
	int buf[4];
	int best;
	int mx;
	int ix;
	int mx2;
	int ix2;
	int sel;
	int v;
	int i;
	int n;
	int j;
	int x;

	best = -1;
	if (id > 7)
		return 0;
	Func_80797fc(id, x, buf);
	if (GetFlag(0x20) != 0) {
		if (id == 0)
			return 0xc8;
		if (id == 1)
			return 0xc9;
	}
	if (id == 5)
		return 0xca;
	if (best != -1)
		return best;
	mx = best;
	ix = best;
	for (i = 0; i <= 3; i++) {
		if (mx < buf[i]) {
			mx = buf[i];
			ix = i;
		}
	}
	mx2 = -1;
	ix2 = -1;
	for (i = 0; i <= 3; i++) {
		if (i != ix) {
			if (mx2 < buf[i]) {
				mx2 = buf[i];
				ix2 = i;
			}
		}
	}
	sel = ix;
	if (buf[ix2] > 9)
		sel = ix2;
	v = Func_80797ec(ix, sel);
	for (n = 0xca; n >= 0; n--) {
		if (L84b1c[n].id == v) {
			for (j = 0; j <= 3; j++) {
				if (buf[j] < L84b1c[n].req[j] * 10)
					break;
			}
			if (j == 4) {
				best = n;
				break;
			}
		}
	}
	if (best == -1)
		best = 0;
	return best;
}
