extern int OvlFunc_898_2009638(int *a, int *b);
extern int __atan2(int y, int x);
extern void __Actor_SetAnim(void *a, int anim);

int OvlFunc_898_2009674(int *a, int *b, int lim, int force)
{
	int *ap;
	int *bp;
	int ret;
	int ang;
	int dir;
	int base;
	int plus;
	int minus;

	bp = b + 2;
	ap = a + 2;
	ret = 0;
	if (OvlFunc_898_2009638(bp, ap) < lim || force != 0) {
		ang = (unsigned short)__atan2(b[4] - a[4],
					      *bp - *ap);
		minus = (ang - (0x80 << 5)) & (0xf0 << 8);
		plus = (ang + (0x80 << 5)) & (0xf0 << 8);
		base = ang & (0xf0 << 8);
		dir = *(unsigned short *)((char *)a + 6) & (0xf0 << 8);
		if (base == dir || plus == dir || minus == dir || force != 0) {
			((char *)a)[0x5b] = 1;
			__Actor_SetAnim(a, 1);
			ret = 1;
		}
	} else {
		((char *)a)[0x5b] = ret;
		__Actor_SetAnim(a, 2);
	}
	return ret;
}
