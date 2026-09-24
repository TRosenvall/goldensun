typedef unsigned char u8;

struct Actor {
	u8 pad00[0xc];
	int f0c;
	u8 pad10[0x55 - 0x10];
	u8 f55;
	u8 pad56[0x68 - 0x56];
	void *f68;
	void *f6c;
};

extern u8 L9f0bc[] __asm__(".L9f0bc");
extern u8 *iwram_3001f30;

extern void Func_8097384(void);
extern void _Actor_SetScript(u8 *a, u8 *script);
extern u8 *Func_8098a84(int x, int y, int z, int w);
extern void WaitFrames(int n);
extern void Func_8096bec(u8 *a, int n, int f);
extern void _Actor_WaitMovement(u8 *a);
extern void _PlaySound(int id);
extern int Func_8096b88(u8 *a);
extern void Func_80981b0(u8 *a);
extern void Func_809748c(void);

void Field_Lift_Target(void)
{
	u8 *arr[2];
	int v[3];
	u8 *b;
	struct Actor *t;
	u8 *e;
	u8 *o;
	u8 **p;
	int z;
	int i;

	b = iwram_3001f30;
	t = *(struct Actor **)(b + 0x14);
	e = *(u8 **)(b + 0x10);
	if (t == 0)
		return;
	Func_8097384();
	*(void **)(e + 0x68) = t;
	_Actor_SetScript(e, L9f0bc);
	v[0] = *(int *)(b + 4);
	v[1] = *(int *)(b + 8) + (0x80 << 13);
	v[2] = *(int *)(b + 0xc);
	arr[0] = Func_8098a84(v[0] + (0x80 << 14), v[1], v[2], 0x80 << 8);
	arr[1] = Func_8098a84(v[0] - (0x80 << 14), v[1], v[2], 0);
	WaitFrames(0xf);
	p = arr;
	for (i = 1; i >= 0; i--) {
		o = *p++;
		if (o != 0)
			Func_8096bec(o, 0xe0 << 12, *(unsigned short *)(o + 6));
	}
	_Actor_WaitMovement(arr[0]);
	t->f6c = Func_8096b88;
	_PlaySound(0x82);
	t->f55 = 4;
	z = t->f0c;
	if (arr[0] != 0 && arr[1] != 0) {
		while (t->f0c <= z + (0x80 << 14)) {
			*(int *)(arr[0] + 0xc) += 0x80 << 7;
			*(int *)(arr[1] + 0xc) += 0x80 << 7;
			t->f0c += 0x80 << 7;
			WaitFrames(1);
		}
	}
	Func_80981b0(arr[0]);
	Func_80981b0(arr[1]);
	Func_809748c();
}
