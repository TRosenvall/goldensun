/* Cluster Field_Lift..Field_Lift extracted from goldensun/asm/rom_8a000/rom_97b54_a_c_a_c_c_a.s.
 *
 * Total .text for this TU = 304 bytes (= 0x130). Never attempted before batch 277.
 * No pins, no flags. EXACT ON THE FIRST CANDIDATE (after one typo: the
 * iwram_3001f30 symbol IS the pointer, so `(int *)`, not `*(int **)`).
 *
 * IDENTICAL TWIN: Field_Carry, at src/rom_8a000/rom_97b54_c_c_c_a_b.c. tools/dupfuncs.py
 * paired them and the streams differ in exactly TWO normalised lines, both the callee
 * symbol (`bl Func_8098a84` here against `bl Func_809a3c4` there). Confirmed twice over:
 * by normalising every .L<hex> to a serial name and diffing, and by construction -- one
 * sed of the callee and the function name compiles byte-identical against the other's own
 * reference. If you edit one, edit both.
 *
 * WHY ONE CANDIDATE: the file-mates did all the work, and both were in this directory.
 * rom_97b54_a_c_a_c_c_b.c IS Func_8098a84, the callee. rom_97b54_a_c_a_c_c_c_b.c
 * (Func_8098c08) already contains this exact particle-spawn loop, solved, spelling
 * included -- `Random() + (0x80 << n)` written before the plain `= 0x80 << n`,
 * `Random() * 24`, ONE shared actor pointer, and `for (i = 0; i < N; i++)` against a ROM
 * countdown. It was transcribed, not derived. src/non_matching/rom_8a000/809802c.c
 * supplied the iwram_3001f30 idiom -- a park again, the seventh function to land off one.
 *
 * That is batch 276's pickability finding holding for a third and fourth function: an
 * elevated same-stem NEIGHBOUR, not the instruction count, is what predicts convergence.
 * 128 instructions, one candidate.
 */
extern unsigned char *iwram_3001f30;
extern void Func_8097384(void);
extern unsigned char *Func_8098a84(int a, int b, int c, int d);
extern void WaitFrames(int n);
extern void Func_8096bec(unsigned char *a, int b, int c);
extern void _Actor_WaitMovement(unsigned char *a);
extern void _PlaySound(int id);
extern unsigned char *CreateParticleActor(int id, int x, int y, int z);
extern void _Actor_SetScript(unsigned char *a, unsigned char *s);
extern unsigned int Random(void);
extern void _DeleteActor(unsigned char *a);
extern void Func_809748c(void);
extern unsigned char L9f0d4[] __asm__(".L9f0d4");

void Field_Lift(void)
{
	unsigned char *act[2];
	int v[3];
	unsigned char **q;
	unsigned char *p;
	int *g;
	int i;

	g = (int *)iwram_3001f30;
	Func_8097384();
	v[0] = g[1];
	v[1] = g[2] + (0x80 << 13);
	v[2] = g[3];
	act[0] = Func_8098a84(v[0] + (0x80 << 14), v[1], v[2], 0x80 << 8);
	act[1] = Func_8098a84(v[0] - 0x200000, v[1], v[2], 0);
	WaitFrames(0xf);
	q = act;
	for (i = 1; i >= 0; i--) {
		p = *q++;
		if (p != 0)
			Func_8096bec(p, 0xc0 << 13, *(unsigned short *)(p + 6));
	}
	_Actor_WaitMovement(act[0]);
	_PlaySound(0x86);
	for (i = 0; i < 24; i++) {
		v[0] = g[1];
		v[1] = g[2] + (0x80 << 13);
		v[2] = g[3];
		p = CreateParticleActor(0x11d, v[0], v[1], v[2]);
		if (p != 0) {
			_Actor_SetScript(p, L9f0d4);
			*(int *)(p + 0x30) = Random() + (0x80 << 10);
			*(int *)(p + 0x34) = 0x80 << 10;
			*(char *)(p + 0x55) = 0;
			Func_8096bec(p, Random() * 24 + (0x80 << 12), Random());
		}
	}
	_DeleteActor(act[0]);
	_DeleteActor(act[1]);
	Func_809748c();
}
