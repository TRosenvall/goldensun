/* Cluster Func_8098c08..Func_8098c08 extracted from goldensun/asm/rom_8a000/rom_97b54_a_c_a_c_c.s.
 *
 * Total .text for this TU = 196 bytes (= 0xc4). Never attempted before batch 274.
 * No pins, no flags.
 *
 * TWO LEVERS.
 *
 * 1. DISTINCT CALL RESULTS WANT DISTINCT VARIABLES. Reusing one pointer for the
 *    pre-loop and the in-loop CreateParticleActor forced a global allocno
 *    (`mov r6, r0 / cmp r6, #0`) where the ROM tests r0 directly. Splitting into two
 *    names went 70 differing to 39. That is the recorded "two results of the same call
 *    need two pointer variables" rule, and note it is the OPPOSITE of what
 *    Func_809c314 needed in batch 273, where two uses of one register wanted ONE
 *    variable -- read which register the ROM spends before choosing.
 *
 * 2. AN HImode LITERAL STORE POOLS, AND THE `int` LOCAL THAT FIXES IT LANDS THE `mov`
 *    TOO EARLY. `*(short *)(p + 0x5e) = 0x14` gives `ldr r3, =0x14`; `h = 0x14; ... = h`
 *    gives `mov r3, #0x14` but scheduled BEFORE the address `add`. Walking the address in
 *    its own statement -- `q = p + 0x55; *q = 0; q += 9; *(short *)q = h;` -- puts the
 *    add first and closes it. 2 differing to exact.
 *
 *    That is a third condition on the HImode-store rule, after batch 273's loop-invariant
 *    counter-case: the int local is right, but its placement relative to the address
 *    computation is a separate question.
 *
 * src/non_matching/rom_8a000/8096140.c -- a PARK -- supplied every callee prototype.
 */
extern void _PlaySound(int id);
extern unsigned char *CreateParticleActor(int id, int x, int y, int z);
extern void _Actor_SetScript(unsigned char *a, unsigned char *s);
extern void Func_8096bec(unsigned char *a, int b, int c);
extern unsigned int Random(void);
extern unsigned char Data_9f0b0[];
extern unsigned char L9f0d4[] __asm__(".L9f0d4");

int Func_8098c08(unsigned char *e)
{
	int v[3];
	unsigned char *p;
	unsigned char *c;
	unsigned char *q;
	int h;
	int i;

	_PlaySound(0x86);
	v[0] = *(int *)(e + 8);
	v[1] = *(int *)(e + 0xc);
	v[2] = *(int *)(e + 0x10);
	p = CreateParticleActor(0x11b, v[0], v[1] - 0x200000, v[2]);
	if (p != 0) {
		q = p + 0x55;
		*(char *)q = 0;
		q += 9;
		h = 0x14;
		*(short *)q = h;
		_Actor_SetScript(p, Data_9f0b0);
	}
	for (i = 0; i < 12; i++) {
		c = CreateParticleActor(0x11d, v[0], v[1], v[2]);
		if (c != 0) {
			_Actor_SetScript(c, L9f0d4);
			*(int *)(c + 0x30) = Random() + (0x80 << 9);
			*(int *)(c + 0x34) = 0x80 << 9;
			*(char *)(c + 0x55) = 0;
			Func_8096bec(c, Random() * 24 + (0x80 << 12), Random());
		}
	}
	return 0;
}
