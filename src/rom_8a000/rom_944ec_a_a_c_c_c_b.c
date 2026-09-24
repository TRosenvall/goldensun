extern unsigned char gState[];

extern int _GetFlag(int id);
extern unsigned char *Func_808d394(int id);
extern unsigned char *GetFieldActor(int slot);
extern void _Actor_SetPos(unsigned char *a, int x, int y, int z);
extern void _Actor_SetAnim(unsigned char *a, int n);

void Func_8095680(void)
{
	unsigned char *g;
	unsigned char *p;
	unsigned char *q;
	unsigned char *rec;
	unsigned char *a;
	unsigned char *b;
	int hi;
	int low;
	int i;

	g = gState;
	p = g + (0x8d << 2);
	hi = *(short *)p & (0xf0 << 8);
	low = *(unsigned short *)p & 0xfff;
	if (_GetFlag(0x109) == 0)
		return;
	if (hi != 0)
		return;
	hi = low & (0x80 << 4);
	low &= 0x7ff;
	if ((unsigned int)(low - 0x12c) > 0x50)
		return;
	if (*(short *)(g + 0x236) <= 0)
		return;
	i = 8;
	q = g + 0x1f4;
	for (; i <= 0x41; i++) {
		rec = Func_808d394(i);
		if (rec == 0)
			continue;
		if (*(short *)(rec + 2) - 0x30 != low - 0x12c)
			continue;
		a = GetFieldActor(i);
		if (a == 0)
			continue;
		if (hi == 0) {
			*(int *)(a + 0x14) = 0;
			*(unsigned char *)(a + 0x55) = 3;
			_Actor_SetPos(a, *(int *)(rec + 8), *(int *)(rec + 0xc),
				      *(int *)(rec + 0x10));
		} else {
			b = GetFieldActor(*(int *)q);
			_Actor_SetPos(a, *(int *)(b + 8), *(int *)(b + 0xc),
				      *(int *)(b + 0x10) - (0x20 << 16));
		}
		_Actor_SetAnim(a, 1);
	}
}
