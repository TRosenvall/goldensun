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
