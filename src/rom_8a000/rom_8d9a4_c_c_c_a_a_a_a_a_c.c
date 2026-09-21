/* Cluster Func_8090488..Func_8090488 extracted from goldensun/asm/rom_8a000/rom_8d9a4_c_c_c_a_a_a_a_a.s.
 *
 * Total .text for this TU = 252 bytes (= 0xfc). Never attempted before batch 276.
 * No pins, no flags. EXACT ON THE FIRST CANDIDATE.
 *
 * 101 INSTRUCTIONS -- THE FIRST FUNCTION LANDED PAST THE 100 BOUNDARY, AND IT COST ONE
 * CANDIDATE. tools/pickable.py rejects anything over 120 as having "too many independent
 * residues to converge", and batches 272-275 stayed under 100 on that reading. This file
 * and src/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a_c_a.c (also 101, two candidates) are
 * the counter-examples, and the reason is not that the boundary is wrong by 20
 * instructions. It is that BOTH had an elevated near-twin in the same stem.
 *
 * This one is Func_80903bc (src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_a_a_b.c) with a THREE-way
 * tail where that has a two-way. Every lever transferred unchanged and none had to be
 * rediscovered: `StopTask` declared `int` (the return-type/argument-fill lever), the
 * `divsi3_RAM` call made through a function-pointer local, the second global derived from
 * the first by a negative offset off the same base, and the typed `struct G` halfword
 * fields. Nothing here was read out of a dump.
 *
 * So the predictor that actually held across all four of batch 276's 101-107 targets was
 * NEIGHBOUR, NOT SIZE: the two with twins went in 1 and 2 candidates, the two without
 * stalled at 26 and 35 of ~110 after 12 and 22. See reports/batch-276.md for the full
 * measurement, including the two residues there that are mutually exclusive by pass order.
 */
struct G {
	unsigned char pad[0x100];
	unsigned short a;
	unsigned short b;
};

extern unsigned char iwram_3001ecc[];
extern int iwram_3001e40;
extern int StopTask(void *task);
extern void SetIntrHandler(int a, int b, void (*f)(void));
extern int divsi3_RAM();

void Func_8090488(void);

void Func_8090488(void)
{
	int (*fp)(int, int);
	signed char *t;
	struct G *g;
	int n;
	unsigned int v;

	t = *(signed char **)iwram_3001ecc;
	g = *(struct G **)(iwram_3001ecc - 0x5c);
	if (t[0x53c] != 0) {
		if (t[0x53d] >= t[0x53c]) {
			t[0x53c] = 0;
			StopTask(Func_8090488);
			SetIntrHandler(1, 0, 0);
			return;
		}
		n = (t[0x53b] - t[0x53a]) * ++t[0x53d];
		fp = divsi3_RAM;
		n = fp(n, t[0x53c]);
		*(unsigned short *)(t + 0x52a) = t[0x53a] + n;
	}
	v = *(unsigned short *)(t + 0x52a);
	if (v > 0x4f) {
		g->a = 0xc8;
		g->b = 0xfa;
	} else if (v != 0 && (iwram_3001e40 & 1)) {
		g->a = v + 0x50;
		g->b = 0x50 - v;
	} else {
		g->a = 0;
		g->b = 0x9f;
	}
}
