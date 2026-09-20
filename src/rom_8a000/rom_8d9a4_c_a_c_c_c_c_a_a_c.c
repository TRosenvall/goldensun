/* Cluster Func_808ee0c..Func_808ee0c extracted from goldensun/asm/rom_8a000/rom_8d9a4_c_a_c_c_c_c_a_a.s.
 *
 * Total .text for this TU = 216 bytes (= 0xd8). Never attempted before batch 275.
 * No pins, no flags, no split -- its .s held the function alone.
 *
 * Walks up to ten warp records looking for one within a tile of the player, and on a hit
 * snaps the player to it and launches them along the warp's bearing.
 *
 * THREE LEVERS, 95 differing to 26 to exact.
 *
 * 1. A FOUR-TERM `&&` RANGE CHAIN MUST BE PARENTHESISED INTO PAIRS. Written flat,
 *    `fold_truthop` folds only the FIRST pair; the second is left-associated against the
 *    accumulated test, never becomes adjacent siblings, and `fold_range_test` never fires on
 *    it -- so the second delta gets two signed compares against pooled 0xfff00001/0xfffff
 *    instead of the ROM's biased `add / cmp / bhi`. `(A && B) && (C && D)` closes it.
 *
 *    Worth knowing because the flat form is the natural way to write it and the failure is
 *    silent: the first half of the test is right and the second half is not. Two `if (...)
 *    continue;` statements measure identically at 26, i.e. they do not help either.
 *
 * 2. THE INVARIANT LOADS BELONG INSIDE THE LOOP. `ax = a->f08` before the loop puts them
 *    ahead of the `p[4] != 0` guard; written inside, loop-invariant motion drops them into
 *    the PREHEADER, which is after the guard -- and that is where the ROM has them. The same
 *    shape as the recorded "a giv init lands at loop_start, a source statement lands before
 *    it", now for a plain invariant.
 *
 * 3. INTERLEAVE THE TWO DELTA CHAINS (`ox`, `dx`, then `oz`, `dz`) rather than computing both
 *    offsets first. It decides which of r0/r1 holds `ox`.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;

struct Actor {
	unsigned char pad00[8];
	int f08;
	unsigned char pad0c[4];
	int f10;
	unsigned char pad14[0x38 - 0x14];
	int f38;
	int f3c;
	int f40;
};

extern struct Actor *GetFieldActor(int id);
extern int atan2(int a, int b);
extern void vec3_translate(int d, unsigned short ang, void *v);

void Func_808ee0c(void)
{
	unsigned char *g;
	unsigned char *p;
	struct Actor *a;
	int ox;
	int oz;
	int dx;
	int dz;
	int i;
	unsigned short ang;

	g = gState;
	a = GetFieldActor(*(int *)(g + (0xfa << 1)));
	p = iwram_3001ebc + (0x8e << 1);
	for (i = 0; i < 10 && p[4] != 0; i++, p += 8) {
		ox = p[6] << 20;
		dx = a->f08 - ox - (0x80 << 12);
		oz = p[7] << 20;
		dz = a->f10 - oz - (0x80 << 12);
		if ((dx >= -0xfffff && dx <= 0xfffff) && (dz >= -0xfffff && dz <= 0xfffff)) {
			a->f08 = ox + (0x80 << 12);
			a->f10 = oz + (0x80 << 12);
			ang = atan2(dz, dx);
			vec3_translate(0xa0 << 13, ang, &a->f08);
			a->f38 = 0x80 << 24;
			a->f3c = 0x80 << 24;
			a->f40 = 0x80 << 24;
			return;
		}
	}
}
