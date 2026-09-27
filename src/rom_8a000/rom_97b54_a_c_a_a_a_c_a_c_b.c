/* Func_8098070  --  0x08098070, split out of asm/rom_8a000/rom_97b54_a_c_a_a_a_c_a_c.s;
 * Field_Move (parked) stays in _a.s. Matched from scratch; returns the particle
 * pointer (`pop {r1}`), and the two `Random() - Random()` values are two separate
 * single-assignment locals, which avoids a spill.
 */
extern void _PlaySound(int id);
extern unsigned char *CreateParticleActor(int id, int x, int y, int z);
extern void _Actor_SetScript(unsigned char *a, unsigned char *s);
extern void _Actor_SetAnim(unsigned char *a, int anim);
extern unsigned int Random(void);
extern void Func_8096bec(unsigned char *a, int b, int c);
extern void Func_8097b70(unsigned char *a);
extern unsigned char L9f0d4[] __asm__(".L9f0d4");

unsigned char *Func_8098070(unsigned char *a)
{
	unsigned char *p;
	unsigned char *q;
	int i;
	unsigned int ang;
	int dist;
	unsigned int r, u;
	int z;

	ang = (*(unsigned short *)(a + 6) + (0x80 << 6)) & 0xc000;
	p = CreateParticleActor(0xd7, *(int *)(a + 8), *(int *)(a + 0xc) + (0x80 << 13), *(int *)(a + 0x10));
	if (p == 0)
		return 0;
	*(int *)(p + 0x1c) = 0x80 << 7;
	*(int *)(p + 0x18) = 0x80 << 7;
	*(void **)(p + 0x6c) = Func_8097b70;
	*(int *)(p + 0x30) = 0x80 << 10;
	*(int *)(p + 0x34) = 0x80 << 10;
	z = 0;
	*(char *)(p + 0x55) = z;
	_Actor_SetAnim(p, 3);
	Func_8096bec(p, 0x80 << 13, ang);
	for (i = 7; i >= 0; i--) {
		q = CreateParticleActor(0x11d, *(int *)(a + 8), *(int *)(a + 0xc) + (0x80 << 13), *(int *)(a + 0x10));
		if (q != 0) {
			_Actor_SetScript(q, L9f0d4);
			*(int *)(q + 0x30) = Random() + (0x80 << 9);
			*(int *)(q + 0x34) = 0x80 << 9;
			*(char *)(q + 0x55) = 2;
			*(int *)(q + 0x48) = 0x51e;
			r = Random();
			r -= Random();
			*(int *)(q + 0x28) = r;
			dist = Random() * 24 + (0x80 << 12);
			u = Random();
			u -= Random();
			u >>= 3;
			u += *(unsigned short *)(a + 6);
			Func_8096bec(q, dist, u);
		}
	}
	_PlaySound(0x8a);
	return p;
}
