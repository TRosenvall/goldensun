extern unsigned char iwram_3001e70[];

extern int L5b38 __asm__(".L5b38");
extern int L5b50[] __asm__(".L5b50");
extern int L5b58 __asm__(".L5b58");
extern int L5b60[] __asm__(".L5b60");

extern int __cos(int);
extern int __sin(int);
extern int __Random(void);

void OvlFunc_943_200b1a8(void)
{
	unsigned char *st;
	unsigned char *q;
	int *p;
	int c;
	int sn;
	int t1;
	int msk;
	int t2;
	int n1;
	int n2;

	st = *(unsigned char **)iwram_3001e70;
	p = *(int **)st;
	c = __cos(L5b58);
	sn = __sin(L5b38);
	t1 = *p;
	t1 += c >> 1;
	*p++ = t1;
	*p = *p + sn;
	n1 = __Random();
	L5b58 = L5b58 + ((unsigned int)((n1 * 3) << 7) >> 16);
	n2 = __Random();
	t2 = L5b38 + ((unsigned int)(n2 << 9) >> 16);
	msk = 0xffff;
	L5b58 = (unsigned short)L5b58;
	L5b38 = t2 & msk;
	q = st + (0x82 << 1);
	*(int *)(q + 8) = L5b50[0];
	L5b50[0] = L5b50[0] - L5b60[0];
	if (L5b50[0] < 0)
		L5b50[0] = L5b50[0] + (0x80 << 14);
	if (L5b50[0] > (0x80 << 14))
		L5b50[0] = L5b50[0] - 0x200000;
	*(int *)(q + 0xc) = L5b50[1];
	L5b50[1] = L5b50[1] - L5b60[1];
	if (L5b50[1] < 0)
		L5b50[1] = L5b50[1] + (0x80 << 14);
}
