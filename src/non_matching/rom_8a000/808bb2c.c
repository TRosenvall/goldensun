/* Func_808bb2c -- NON-MATCHING, 102 encodings of 130.  SIZE IDENTICAL, instruction count
 * identical (130 = 130).  Same 8 relocations, offsets shifted by 2 bytes.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_8a000/808bb2c.c \
 *     asm/rom_8a000/rom_8ba38_a_a_a_a_c.s --func Func_808bb2c
 *
 * NOTE THE REFERENCE PATH: this function was in rom_8ba38_a_a_a_a.s until batch 285 split
 * that file to land its neighbour Func_808ba38.  It now lives in the `_c` part.
 *
 * BLOCKER: RELOAD SCRATCH-REGISTER SET TOO NARROW -- the `allocate_reload_reg` class at
 * docs/elevation.md:19305, and here the DOCUMENTED CURE IS THE RIGHT DIAGNOSIS (unlike its
 * file-mate Func_8093af8, where it is not -- see that park).
 *
 * The ROM uses **r1** as a scratch in five places where this candidate uses r2/r3:
 * `ldr r1,=ewram_2000434 / ldr r3,[r1]`, `mov r1,#0xf0`, `ldr r1,=iwram_3001e70`,
 * `mov r1,r10`.  THE ROM KEEPS THE SYMBOL ADDRESS IN A REGISTER DISTINCT FROM THE LOADED
 * VALUE, so it needs THREE scratches where this needs two -- and that is what puts r1 in
 * its spill set.  So the next attempt should force a third simultaneously-live scratch
 * rather than permute the two it has.
 *
 * ALSO MEASURED HERE, and it is a lever used BACKWARD for the first time:
 * `reload_cse_move2add` FIRES ON THE ROM'S SIDE TOO AND SOMETIMES MUST BE DEFEATED.  The
 * entry derived `-0x1f` as `mov r3,#0 / sub r3,#0x1f` -- ONE INSTRUCTION SHORTER than the
 * ROM's `mov r2,#0x1f / neg r2,r2` -- because `n = 0` had landed in the same hard
 * register.  Moving `n = 0;` after the first slot read separates them.  Batch 284 used
 * move2add forward, to SHORTEN toward the ROM; this is the first case of needing it
 * lengthened.
 */
#include "dma.h"

struct Sprite {
	u8 pad00[9];
	u8 b9;
	u8 pad0a[0x15 - 0xa];
	u8 b15;
};

struct Actor {
	u8 pad00[0xc];
	int f0c;
	u8 pad10[0x50 - 0x10];
	struct Sprite *f50;
};

extern unsigned char ewram_2001124[];
extern int ewram_2000434;
extern unsigned char *iwram_3001ebc;
extern int **iwram_3001e70;

extern struct Actor *GetFieldActor(int slot);
extern void _Actor_SetAnim(struct Actor *a, int anim);
extern void _Actor_SetSpriteFlags(struct Actor *a, int flags);
extern void _Actor_Stop(struct Actor *a);

void Func_808bb2c(void)
{
	u8 *src;
	u8 *sl;
	u8 *q1;
	u8 *q2;
	u8 *q3;
	struct Actor *a;
	struct Sprite *s;
	int n;
	int slot;
	int m;
	int t1;
	int t2;
	int *w;
	int *x;
	int v;
	int k;

	src = ewram_2001124;
	sl = ewram_2001124 - 0x20;
	q1 = ewram_2001124 + 0xe00;
	q2 = ewram_2001124 + 0xe20;
	q3 = ewram_2001124 + 0xe40;
	n = 0;
	slot = *sl;
	sl++;
	while (slot != 0xff) {
		a = GetFieldActor(slot);
		if (a != 0) {
			s = a->f50;
			DMA3_SET(src, a, 0x8400001c);
			k = *q1;
			if (k != 0)
				_Actor_SetAnim(a, k);
			_Actor_SetSpriteFlags(a, *q2);
			m = (*q3 & 3) << 2;
			t1 = ~0xc;
			t1 &= s->b9;
			t1 |= m;
			s->b9 = t1;
			t2 = ~0xc;
			t2 &= s->b15;
			t2 |= m;
			s->b15 = t2;
			a->f50 = s;
			if (slot == ewram_2000434) {
				w = *(int **)(iwram_3001ebc + (0xf0 << 1));
				x = *iwram_3001e70;
				v = a->f0c;
				w[5] = v;
				w[3] = v;
				x[1] = v;
				_Actor_Stop(a);
			}
		}
		src += 0x70;
		q1++;
		q2++;
		n++;
		q3++;
		if (n > 0x1f)
			break;
		slot = *sl;
		sl++;
	}
}
