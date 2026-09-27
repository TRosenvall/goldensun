/* Func_80981b0  --  0x080981b0, was asm/rom_8a000/rom_97b54_a_c_a_a_a_c_c_a.s
 * (this function alone), so it converts whole. Matched on the FIRST candidate,
 * written from Field_Lift's particle loop: `for (i = 0; i < 31; i++)` for the
 * rise and `i < 8` for the particles, `r = Random(); ... = r - Random();`, and
 * `Random() * 24 + (0x80 << 12)`.
 */
extern void _PlaySound(int id);
extern void WaitFrames(int n);
extern unsigned char *CreateParticleActor(int id, int x, int y, int z);
extern void _Actor_SetScript(unsigned char *a, unsigned char *s);
extern unsigned int Random(void);
extern void Func_8096bec(unsigned char *a, int b, int c);
extern void _DeleteActor(unsigned char *a);
extern unsigned char L9f0d4[] __asm__(".L9f0d4");

void Func_80981b0(unsigned char *a)
{
	unsigned char *p;
	int i;
	int r;

	_PlaySound(0x9a);
	for (i = 0; i < 31; i++) {
		*(int *)(a + 0xc) += 0x80 << 9;
		*(unsigned short *)(a + 6) += 0x80 << 6;
		*(int *)(a + 0x18) += -0x800;
		*(int *)(a + 0x1c) += -0x800;
		WaitFrames(1);
	}
	for (i = 0; i < 8; i++) {
		p = CreateParticleActor(0x11d, *(int *)(a + 8), *(int *)(a + 0xc), *(int *)(a + 0x10));
		if (p != 0) {
			_Actor_SetScript(p, L9f0d4);
			*(int *)(p + 0x30) = Random() + (0x80 << 9);
			*(int *)(p + 0x34) = 0x80 << 9;
			*(char *)(p + 0x55) = 2;
			*(int *)(p + 0x48) = 0xa3d;
			r = Random();
			*(int *)(p + 0x28) = r - Random();
			Func_8096bec(p, Random() * 24 + (0x80 << 12), Random());
		}
	}
	_PlaySound(0x83);
	_DeleteActor(a);
}
