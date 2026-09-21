/* Cluster OvlFunc_950_20083dc..OvlFunc_950_20083dc extracted from
 * goldensun/asm/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_c_a_c_a_a.s.
 *
 * Total .text for this TU = 292 bytes (= 0x124). Never attempted before batch 277.
 * No pins, no flags. 120 instructions; four candidates, 73 -> 16 -> 8 -> 3 -> 0.
 *
 * EVERYTHING CAME FROM TWO EXEMPLARS FOUND BY GREPPING, not by sweeping spellings:
 *
 *   src/overlays/rom_7987ac/ovl_30_c_c_c_a.c gave the opening verbatim --
 *   `*(int *)(p + (0xe0 << 1)) = 0x209;` and `*(short *)(g + (0xe1 << 1))`, both of which
 *   gcc derives from the 0x1c0 already in a register (`add r2, #0x49`) -- plus the
 *   stack-argument-pair lever (`s0`/`s1` named in the ROM's store order).
 *
 *   src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c_c.c gave `mask = ~0xc;` as a named `int`, which
 *   is what produces the `mov r5, #0xd / neg r5, r5` pair.
 *
 * AND ONE MEASURED NEGATIVE AGAINST THE FIRST OF THOSE EXEMPLARS, which is the reason this
 * header exists. That file's lever 3 says the held zero must be a named `int zero` written
 * immediately before the first store. HERE THAT IS WRONG -- bare `0` literals are exact
 * and every named form costs 3:
 *
 *     bare `0` literals                              0
 *     `zero = 0;` after the GetActor call            3
 *     `zero` declared first among the locals         3
 *     `zero` assigned BEFORE the GetActor call       6
 *     two distinct actor pointers, named `zero`      3
 *
 * gcc holds the bare 0 in r8 across both calls on its own. So "the held zero is named" is
 * CONDITIONAL, not general, and the discriminator between the two cases is not yet
 * identified. Read that exemplar's lever 3 with this note beside it.
 */
struct Spr {
	unsigned char pad00[9];
	unsigned char f9;
};

struct Actor {
	unsigned char pad00[0x23];
	unsigned char f23;
	unsigned char pad24[0x50 - 0x24];
	struct Spr *f50;
};

extern char *iwram_3001ebc;
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapTransitionIn(void);
extern void OvlFunc_950_200813c(void);
extern void OvlFunc_950_2008328(void);

int OvlFunc_950_20083dc(void)
{
	char *p;
	unsigned char *g;
	struct Actor *a;
	int s0;
	int s1;
	int mask;
	int f1;
	int f2;
	int f3;
	int c1;
	int c2;
	int px;
	int py;
	int q;

	px = 0x8c << 18;
	py = 0xaa << 18;
	q = 0x80 << 8;
	f1 = 0x8bc;
	f2 = 0x8bc;
	f3 = 0x8bc;
	c1 = 0xc0 << 2;
	c2 = 0xc0 << 2;
	p = iwram_3001ebc;
	*(int *)(p + (0xe0 << 1)) = 0x209;
	if (__GetFlag(0x95 << 4)) {
		s0 = 0x33;
		s1 = 0x2d;
		__Func_8010704(0x33, 0x2f, 3, 1, s0, s1);
		a = (struct Actor *)__MapActor_GetActor(0x1f);
		a->f23 = 0;
		mask = ~0xc;
		a->f50->f9 = (mask & a->f50->f9) | 8;
		a = (struct Actor *)__MapActor_GetActor(0x20);
		a->f23 = 0;
		a->f50->f9 = (mask & a->f50->f9) | 8;
		if (__GetFlag(f1)) {
			__MapActor_SetPos(0x19, px, py);
			__Func_8092adc(0x19, q, 0);
		}
		g = gState;
		if (*(short *)(g + (0xe1 << 1)) == 0x13 && !__GetFlag(f2)) {
			__SetFlag(f3);
			__MapTransitionIn();
			OvlFunc_950_200813c();
		}
		if (*(short *)(g + (0xe1 << 1)) == 0x10 && !__GetFlag(c1)) {
			__SetFlag(c2);
			__MapTransitionIn();
			OvlFunc_950_2008328();
		}
		if (__GetFlag(0x8ab)) {
			__MapActor_SetPos(0x23, 0, 0);
			__MapActor_SetPos(0x24, 0, 0);
		}
	}
	return 0;
}
