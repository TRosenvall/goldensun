/* Func_80c0a24 -- NON-MATCHING, 47 encodings of 209, TWO INSTRUCTIONS LONG: ref 209
 * encodings / 448 bytes, ours 211 / 452.  195 instructions against 194, and THE RELOCATION
 * LIST MATCHES SYMBOL FOR SYMBOL AND IN ORDER (displaced 4 bytes).  AT 47 OF 209 WITH THE
 * RELOCATIONS IN ORDER THIS IS ONE OF THE CLOSER PARKS IN ITS BANK.
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
	*(int *)p ^= 1;
}
