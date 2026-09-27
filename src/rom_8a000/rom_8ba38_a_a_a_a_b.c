#include "dma.h"



struct Sprite {
	u8 pad00[9];
	u8 b9;
	u8 pad0a[0x24 - 0xa];
	u8 b24;
	u8 pad25[1];
	u8 b26;
};

struct Actor {
	u8 pad00[0x50];
	struct Sprite *f50;
	u8 f54;
};

extern unsigned char ewram_2001124[];
extern int iwram_3001ebc;

extern struct Actor *GetFieldActor(int slot);

void Func_808ba38(void)
{
	u8 *dst;
	u8 *p;
	u8 *q1;
	u8 *q2;
	u8 *q3;
	struct Actor *a;
	struct Sprite *s;
	int i;
	int j;
	int n;
	int limit;
	int v1;
	int v2;
	int v3;

	dst = ewram_2001124;
	q1 = ewram_2001124 + 0xe00;
	q2 = ewram_2001124 + 0xe20;
	q3 = ewram_2001124 + 0xe40;
	n = 0;
	p = dst - 0x20;
	limit = 0x42;
	if (*(short *)(iwram_3001ebc + (0xcf << 1)) == 3)
		limit = 8;
	for (i = 0; i < limit; i++) {
		a = GetFieldActor(i);
		if (a != 0) {
			*p = i;
			p++;
			DMA3_SET(a, dst, 0x8400001c);
			if (a->f54 == 1) {
				s = a->f50;
				v1 = s->b24;
				v2 = s->b26;
				v3 = ((unsigned int)s->b9 << 28) >> 30;
			} else {
				v1 = 0;
				v2 = 0;
				v3 = 0;
			}
			*q1 = v1;
			q1++;
			*q2 = v2;
			q2++;
			*q3 = v3;
			n++;
			q3++;
			dst += 0x70;
			if ((unsigned int)n > 0x1f)
				break;
		}
	}
	for (i = n; i < 0x20; i++) {
		*p = 0xff;
		p++;
	}
}
