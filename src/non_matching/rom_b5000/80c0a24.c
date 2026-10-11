/* Func_80c0a24 -- NON-MATCHING, 34 differing encodings of 209 stream encodings,
 * ref 209 against ours 209, instructions 192 against 192, sizes equal at 448
 * bytes -- SO THIS IS NOW A TRUE DISTANCE and the count means what it says.
 * The relocation LIST still matches symbol for symbol and in order, displaced
 * four bytes by the literal pool.  Batch 335 brief B closed the length gap; the
 * superseded claim was forty-seven of two hundred and nine with ours two
 * encodings LONGER, and the body below is the one that closed it.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80c0a24.c \
 *     asm/rom_b5000/rom_bffb8_a_c_c.s --func Func_80c0a24
 *
 * Its file-mate AnimTransitionIn is parked next door at 125/129; the .s does NOT convert
 * whole until both land.
 *
 * WHAT IS ALREADY RIGHT, and it is most of the function: the 5-parameter frame; the commoned
 * iwram_3001f00 triple; `#include "math.h"` with four `fx32_multiply` sharing one
 * `ldr r4,=Func_8000888` (the `.call_via` MACRO, not gcc's veneer); `fp = Func_80008ac`
 * twice for `bl _call_via_r2`; gcc's own mid-function pool AT THE ROM'S POSITION; and ALL
 * REGISTER ALLOCATION INCLUDING `mov r14, r0`.
 *
 * THE FILL LOOPS MUST BE GUARD + `do/while`, which is what puts the invariant between the
 * guard and the body.  That is the same loop-shape family as batch 285's goto/break rule and
 * batch 284's "while vs if-guard is a question about the ROM".
 *
 * RESIDUE IS THREE REGISTER-FORM SPOTS, NONE STRUCTURAL:
 *
 *   1. The in-place `lsr` of the `bias` truncation.  GETTING THE PAIR AT ALL REQUIRES
 *      `lo = (unsigned short)bias;` AS ITS OWN STATEMENT -- nine one-expression spellings all
 *      fold, and the RTL dump shows `fold` narrowing the `|` to HImode and combine then
 *      DELETING the widening pair (190 against 194).
 *   2. `add r3,r9 / mov r7,r3` where this has `mov r0,r9 / add r7,r0,r3`.
 *   3. One extra `mov r0,r9` at the closing `*(int *)p ^= 1`.
 *
 * ===== BATCH 335 BRIEF B: THE EXTRA INSTRUCTION WAS RESIDUE 3, AND NAMING THE
 * ===== XOR'S OPERAND REMOVES IT.  PLUS A STRUCTURAL READ OF THE POOL ORDER.
 *
 * DIRECTION, stated because the brief asks for it: OURS WAS LONGER.  So the
 * search was never "what statement is missing" but "what lets gcc reuse a value
 * we rematerialise", and the answer was in residue 3, the place the park had
 * already named and then mis-diagnosed as inert.
 *
 * THE DEFECT.  The closing read-modify-write reaches its base through r9, a high
 * register, so each memory access needs a low copy.  We emitted TWO:
 *
 *     ours   mov r2,r9 / ldr r3,[r2] / mov r2,#1 / eor r3,r3,r2
 *            / mov r0,r9 / str r3,[r0]
 *     ROM    mov r1,r9 / ldr r3,[r1] / mov r2,#1 / eor r3,r2 / str r3,[r1]
 *
 * We put the base's low copy in r2 AND THEN CLOBBERED IT WITH THE CONSTANT, so
 * the store needed a second copy.  The ROM's base copy lands in r1 and survives
 * the constant in r2.  The park had listed `int *pi`, `*pi = *pi ^ 1` and
 * `*pi ^= 1` as inert and concluded the spot was a register-form residue; all
 * three are indeed inert, and so are a `struct P *` with a real member, a named
 * loaded value, and both operand orders of the xor (nine spellings measured this
 * round).  THE DIMENSION NOBODY VARIED WAS THE CONSTANT.
 *
 * THE FIX, and it is one line: give the xor's `1` its own named local.  With
 * `one = 1;` as a statement and `*(int *)p ^= one;`, the constant becomes a
 * pseudo that local-alloc places, instead of a reload-time constant competing
 * for the register the base reload just took -- and the base copy then survives
 * both accesses.  Only the reload rotation is left there (`mov r0,r9` where the
 * ROM has `mov r1,r9`), per docs/elevation.md's "Reload registers rotate".
 *
 * Measured, by tools/objcmp.py on the installed path: the previous body reads
 * two encodings long with ours two hundred and eleven; this one reads
 * 34 of 209 at equal length.  tools/aligncmp.py: aligned-equal one hundred and
 * eighty-four of two hundred and nine against the old body's one hundred and
 * eighty-two, so the body is better as well as shorter -- not the batch-334 trap
 * of a matching count with the instruction in the wrong place.
 *
 * MEASURED INERT, all tying this body at 34 and at 192 instructions: an explicit
 * halfword cast on the OR's constant; `lo` declared as a halfword carrier rather
 * than an int holding a truncation.  MEASURED WORSE: a halfword constant carrier
 * local, either before or after the truncation (thirty-eight); `w` declared as a
 * halfword (one hundred and eighty-eight instructions -- combine deletes the
 * widening pairs, the park's recorded fold).  MEASURED INERT AND WORTH RECORDING
 * AS A WHOLE DIMENSION: all four spellings of residue 2's pointer arithmetic
 * (operand order, the product accumulated into its own named local, and a
 * halfword-pointer walk) are BIT-IDENTICAL to each other.  gcc canonicalises
 * that expression completely, so residue 2 is not reachable by spelling it.
 *
 * ---------------------------------------------------------------------------
 * A STRUCTURAL READ THE PARK DID NOT HAVE: THE ROM'S TWO FILL CONSTANTS ARE
 * LOADED IN HImode, AND THE LITERAL POOL'S ORDER PROVES IT.
 *
 * Twelve of the remaining differing encodings are pool-relative `ldr` offsets
 * plus two misplaced pool words, and they have ONE cause.  The mid-function pool
 * holds the same ten words on both sides, but in a different order:
 *
 *   ROM    0x3f8e, 0x478a, then the eight symbol and constant words
 *   ours   0x3f8e, the eight words, then 0x478a LAST
 *
 * gcc emits minipool fixes in ascending `addr + range` order (arm.c:4820), and
 * `pool_range` is 64 for HImode against 1020 for SImode (arm.md:4353).  Our
 * 0x3f8e sorts to the front because `*q++ = 0x3f8e` is a halfword STORE of a
 * constant -- `*thumb_movhi_insn`, range 64 -- while our 0x478a comes out of an
 * SImode `|` and sorts last among the range-1020 entries.  **For the ROM's
 * 0x478a to sit second it must also be a range-64 entry, i.e. the original's
 * constant reaches the pool through a HImode move.**  This is the same kind of
 * structural argument the `_MSG_182` ruling rests on: the byte layout forces the
 * mode, and the mode is invisible in the instruction text.
 *
 * AND THE OBVIOUS WAY TO GET IT IS CLOSED.  A halfword carrier local holding the
 * constant does not survive to the pool: the constant is folded back to SImode
 * before the minipool pass sees it, which is exactly the warning the bank-mate
 * Func_80c0130's park records ("the constructor decides the mode at the
 * declaration and survival at the use -- if the aggregate reads inert, check
 * `.13.combine` for a const-fold").  Three carrier spellings measured, all with
 * the pool order UNCHANGED and the figure worse.  The open question for the next
 * reader is therefore sharp: what reaches `*thumb_movhi_insn` with 0x478a while
 * leaving the SImode widening pair that the ROM also has?  The ROM holds both at
 * once -- `ldr r0,[sp] / ldr r3,=const / lsl r2,r0,#16 / lsr r2,#16 / orr r2,r3`
 * -- and every halfword spelling that gets the mode loses the pair.
 *
 * NEXT: the pool order is the biggest single block (about twelve encodings) and
 * it now has a mechanism and a line.  The tail's `mov r0,r9` against `mov r1,r9`
 * is a reload rotation, not a spelling.  Residue 2 is canonicalised and should
 * not be re-swept.
 */
/* Func_80c0a24 (0x080c0a24) -- NOT MATCHING, and CLOSE.  Second of two in
 * asm/rom_b5000/rom_bffb8_a_c_c.s.  195 instructions against the ROM's 194;
 * objcmp: ref 209 encodings / 448 bytes, ours 211 / 452.  The relocation LIST
 * matches symbol-for-symbol and in order (two _call_via_r2, one _GetFlag, then
 * iwram_3001f00 / iwram_3001ad0 / Func_80008ac / Func_8000888), displaced 4
 * bytes.  datacheck reports NO DATA.  No shim, no new .sym entry.
 *
 * WHAT IS RIGHT: the five-parameter frame (the fifth at sp+0x30) with its three
 * spilled register parameters at sp+4/8/0xc and the one local at sp+0; the
 * commoned `ldr r3,=iwram_3001f00` with `sub #0x88` and `sub #0x80` off it
 * (the b9b30 idiom, and note this function commons where AnimTransitionIn does
 * NOT); `#include "math.h"` and FOUR `fx32_multiply` calls sharing one
 * `ldr r4,=Func_8000888` -- the .call_via macro form, which is math.h's inline
 * asm and not gcc's veneer; `fp = Func_80008ac; fp(a, b)` twice for the
 * `bl _call_via_r2` form; gcc's own mid-function literal pool between the
 * second and third fill loops, at the ROM's position; and ALL the register
 * allocation including `mov r14, r0` -- gcc puts lr to work here by itself.
 *
 * THE FOUR FILL LOOPS MUST BE GUARD + do/while, NOT `while`.  `if (i < rows)
 * { w = ...; do { *q++ = w; i++; } while (i < rows); }` is what puts the
 * invariant between the guard and the body, which is where the ROM computes it.
 * A plain `while` hoists it ABOVE the guard, and then gcse commons the two
 * loops' `bias` truncations into one (the ROM recomputes it per loop, because
 * with the guard in the way it is not available on all paths).
 *
 * NAMED BLOCKER, and it is small: THE 16-BIT TRUNCATION OF `bias` AND TWO
 * REGISTER-FORM CHOICES.  Residue, in three places:
 *   1. `lsl r2,r3,#16 / lsr r2,#16` -- the ROM does the second shift IN PLACE
 *      on r2; gcc emits `lsl r3,r0,#16 / lsr r2,r3,#16`.  Same count, different
 *      registers.  Getting the pair AT ALL needs the truncation in its own
 *      statement (`lo = (unsigned short)bias; w = lo | 0x478a;`): written as one
 *      expression, `fold` narrows the whole `|` to HImode -- the RTL dump shows
 *      `(set (reg:HI 141) (const_int 0x478a))` -- and combine then deletes the
 *      widening pair, 190 instructions against 194.  Nine one-expression
 *      spellings were measured and ALL fold: both cast positions, `& 0xffff`,
 *      `<<16 >>16`, constant-on-the-left, `int`/`unsigned`/`short`/
 *      `unsigned short` carriers, `bias` as `volatile`, as `int bias[1]`, and
 *      written through a pointer.  volatile is far worse (201 instructions).
 *   2. `q = (char *)p + t * 0x140` gives `mov r0,r9 / add r7,r0,r3` where the
 *      ROM accumulates into the product register, `add r3,r9 / mov r7,r3`.
 *      Both operand orders and a `struct P *` typing are inert.
 *   3. The closing `*(int *)p ^= 1` costs one extra `mov r0,r9` because gcc
 *      puts the base in r2 and then needs r2 for the constant 1; the ROM keeps
 *      the base in r1.  A named `int *pi`, `*pi = *pi ^ 1`, `*pi ^= 1` and a
 *      `struct P *` with a real `flip` member are all inert.
 * All three are register-form residues on an otherwise instruction-identical
 * function; none is structural.
 */
#include "math.h"

struct M {
	short a;
	short b;
	short c;
	short d;
	int e;
	int f;
};

extern unsigned char iwram_3001f00[];
extern short iwram_3001ad0[];
extern int _GetFlag(int flag);
extern int Func_80008ac(int a, int b);

void Func_80c0a24(int a0, int a1, int a2, int a3, int a4)
{
	int *g;
	unsigned char *p;
	unsigned char *view;
	struct M *m;
	unsigned short *q;
	int bias;
	int val;
	int x;
	int t;
	int t0;
	int d;
	int s;
	int A;
	int B;
	unsigned int rows;
	unsigned int i;
	int w;
	int lo;
	int (*fp)(int, int);
	int one;

	g = *(int **)iwram_3001f00;
	p = *(unsigned char **)((char *)iwram_3001f00 - 0x88);
	view = *(unsigned char **)((char *)iwram_3001f00 - 0x80);
	val = 0x80 << 4;
	bias = 0;
	if (a4 >= (0x80 << 9)) {
		bias = 0x80 << 6;
		val = (0xd0 << 7) + -*(short *)(view + 0x36) * 3;
	}
	if (p == 0)
		return;
	x = g[2];
	if ((x == 1 || g[3] == 1) && g[4] == 0)
		iwram_3001ad0[2] = val >> 8;
	if (x != 2)
		return;
	t = *(int *)p ^ 1;
	q = (unsigned short *)(p + t * 0x140);
	fp = Func_80008ac;
	t0 = fp(a4, 0x80 << 9);
	m = (struct M *)(p + 0x10);
	s = t0 >> 8;
	m->a = s;
	m->b = 0;
	m->c = 0;
	m->d = s;
	d = a4 - 0x10000;
	q += 0x10;
	A = fx32_multiply(t0, fx32_multiply(a0, d));
	B = fx32_multiply(t0, fx32_multiply(a1, d));
	m->e = ((A + 0x7fff) >> 8) + a2 + val;
	B = ((B + 0x7fff) >> 8) + a3 - 0x1000;
	m->f = B;
	fp = Func_80008ac;
	rows = (fp((short)s, (0x80 << 7) - B) >> 16) + 1;
	i = 0;
	if (_GetFlag(0x16b) == 0) {
		for (i = 0; i <= 0xf; i++)
			*q++ = 0x3f8e;
	}
	if (rows > 0x88)
		rows = 0x88;
	if (i < rows) {
		lo = (unsigned short)bias;
		w = lo | 0x478a;
		do {
			*q++ = w;
			i++;
		} while (i < rows);
	}
	if (i <= 0x87) {
		lo = (unsigned short)bias;
		w = lo | 0x478e;
		do {
			*q++ = w;
			i++;
		} while (i <= 0x87);
	}
	if (i <= 0x9f) {
		do {
			*q++ = 0x3f8e;
			i++;
		} while (i <= 0x9f);
	}
	one = 1;
	*(int *)p ^= one;
}
