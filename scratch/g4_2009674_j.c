extern int OvlFunc_898_2009638(int *a, int *b);
extern int __atan2(int y, int x);
extern void __Actor_SetAnim(char *a, int anim);

int OvlFunc_898_2009674(char *a, char *b, int lim, int force)
{
	int *bp;
	int *ap;
	int ret;
	int ang;
	int dir;
	int base;
	int plus;
	int minus;

	ret = 0;
	ap = (int *)(a + 8);
	bp = (int *)(b + 8);
	if (OvlFunc_898_2009638(bp, ap) < lim || force != 0) {
		ang = (unsigned short)__atan2(*(int *)(b + 0x10) - *(int *)(a + 0x10),
					      *bp - *ap);
		minus = (ang - (0x80 << 5)) & (0xf0 << 8);
		plus = (ang + (0x80 << 5)) & (0xf0 << 8);
		base = ang & (0xf0 << 8);
		dir = *(unsigned short *)(a + 6) & (0xf0 << 8);
		if (base == dir || plus == dir || minus == dir || force != 0) {
			a[0x5b] = 1;
			__Actor_SetAnim(a, 1);
			ret = 1;
		}
	} else {
		a[0x5b] = ret;
		__Actor_SetAnim(a, 2);
	}
	return ret;
}
