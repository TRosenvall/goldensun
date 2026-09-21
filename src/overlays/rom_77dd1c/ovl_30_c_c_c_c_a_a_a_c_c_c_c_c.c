/* Cluster OvlFunc_882_200bce4..OvlFunc_882_200bce4 extracted from
 * goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_c_c.s.
 *
 * Total .text for this TU = 308 bytes (= 0x134). Never attempted before batch 277.
 * No pins, no flags. 118 instructions; four candidates, 115 -> 21 -> 20 -> 16 -> 0.
 *
 * LEVER 1 -- NAME THE DOUBLY-USED LOAD, AND LEAVE THE OTHER LOAD INLINE BETWEEN ITS TWO
 * USES. The ROM's shape is `ldr f8 / str f8 / ldr f10 / str f38 / str f10`, and it is
 * reached by naming exactly ONE of the two loads:
 *
 *     t = a->f8;
 *     b->f8 = t;
 *     b->f10 = a->f10;     <- must sit BETWEEN the two uses of t
 *     b->f38 = t;
 *
 * Unnamed, strict aliasing forces a reload of `a->f8` after the store to `b->f8`, two
 * instructions too many. Naming BOTH loads reorders the block wrong (21 differing). Naming
 * only `t` but putting the f38 store before the f10 store is 20. So the lever is not
 * "name the reused value" on its own -- it is the naming TOGETHER WITH what separates the
 * two uses.
 *
 * LEVER 2 -- the three-named-locals constant lever, applied to FOUR call sites. See the
 * amendment recorded in docs/elevation.md this batch: the precondition is that the
 * assignments sit in a block a branch DOMINATES, and constants in block 0 must be left
 * BARE. Here all four sites are behind the `a->f64 != 0` guard, so all four take locals.
 *
 * NOT A LEVER, and worth knowing: the switch is a plain `switch` with grouped `case`s and
 * NO `default`. gcc's own cross-jumping produces the ROM's shared `.L3de0` tail while
 * leaving `ldr r3, [r5, #0x18]` duplicated per arm -- nothing had to be written to get
 * that. The `else` arm is literally `s->f18 = 0;`, and gcc reusing the known-zero `and`
 * result register is what makes it `str r2` rather than a fresh `mov`.
 */
struct Sub {
	unsigned char pad00[0x18];
	int f18;
	unsigned char pad1c[0x23 - 0x1c];
	unsigned char f23;
};

struct Actor {
	unsigned char pad00[8];
	int f8;
	unsigned char pad0c[4];
	int f10;
	unsigned char pad14[4];
	int f18;
	int f1c;
	unsigned char pad20[0x38 - 0x20];
	int f38;
	unsigned char pad3c[0x50 - 0x3c];
	struct Sub *f50;
	unsigned char pad54[0x64 - 0x54];
	short f64;
	short f66;
};

extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8012330(int a, int b, int c);
extern unsigned int iwram_3001e40;

void OvlFunc_882_200bce4(void)
{
	struct Actor *a;
	struct Actor *b;
	struct Sub *s;
	int t;
	int x1;
	int y1;
	int x2;
	int y2;
	int x3;
	int y3;
	int x4;
	int y4;
	int z1;
	int z2;
	int z3;

	x1 = 0xc0 << 10;
	y1 = 0xc0 << 10;
	z1 = 0x80 << 9;
	x2 = 0x80 << 11;
	y2 = 0x80 << 11;
	z2 = 0x80 << 9;
	x3 = 0x80 << 10;
	y3 = 0x80 << 10;
	z3 = 0x80 << 9;
	x4 = -1;
	y4 = -1;
	a = (struct Actor *)__MapActor_GetActor(0x13);
	b = (struct Actor *)__MapActor_GetActor(0x1b);
	s = b->f50;
	if (a->f64 != 0) {
		if (a->f64 == 60)
			__Func_8012330(x1, y1, z1);
		if (a->f64 == 40)
			__Func_8012330(x2, y2, z2);
		if (a->f64 == 30)
			__Func_8012330(x3, y3, z3);
		if (a->f64 == 20)
			__Func_8012330(x4, y4, 0xe666);
		a->f64--;
	}
	t = a->f8;
	b->f8 = t;
	b->f10 = a->f10;
	b->f38 = t;
	s->f23 = 10;
	if (iwram_3001e40 & 1) {
		switch (a->f66) {
		case 1:
		case 5:
			b->f18 += 0xa3d;
			b->f1c += 0xa3d;
			break;
		case 4:
			b->f18 += 0x51e;
			b->f1c += 0x51e;
			break;
		case 2:
		case 3:
		case 6:
		case 7:
		case 8:
		case 9:
			b->f18 -= 0x7ae;
			b->f1c -= 0x7ae;
			break;
		}
		s->f18 = b->f18;
	} else {
		s->f18 = 0;
	}
}
