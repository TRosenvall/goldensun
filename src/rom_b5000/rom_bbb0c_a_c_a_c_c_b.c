/* Func_80be0b4  --  0x080be0b4, split out of asm/rom_b5000/rom_bbb0c_a_c_a_c_c.s;
 * Func_80be18c (parked) and Func_80be378 stay in _c.s. Matched from scratch with
 * the Djinni/List/Rec structs from Func_80bfba4's park.
 */
typedef unsigned char u8;
typedef signed char s8;

struct Djinni {
	u8 f0;
	u8 f1;
	u8 f2;
	s8 f3;
};

struct List {
	struct Djinni e[0x40];
	int count;
};

struct Rec {
	u8 pad0[8];
	struct List l;
};

extern int Func_80b6c08(int kind, unsigned short *buf);
extern struct Rec *_Func_8077330(int side);

int Func_80be0b4(unsigned int side, u8 *out)
{
	unsigned short buf[8];
	struct List *d;
	int count;
	int n;
	int i;
	int j;

	count = 0;
	n = Func_80b6c08(side <= 7 ? 1 : 2, buf);
	d = &_Func_8077330(side > 7)->l;
	if (out != 0) {
		for (i = 3; i >= 0; i--)
			out[i] = 0;
	}
	for (i = 0; i != d->count; i++) {
		if (d->e[i].f3 == -1) {
			for (j = 0; j < n && buf[j] != d->e[i].f2; j++)
				;
			if (j != n) {
				if (out != 0)
					out[d->e[i].f0]++;
				count++;
			}
		}
	}
	return count;
}
