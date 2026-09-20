/* Cluster Func_8092624..Func_8092624 extracted from goldensun/asm/rom_8a000/rom_925e0_a_a_a_c.s.
 *
 * Total .text for this TU = 228 bytes (= 0xe4). Never attempted before batch 274.
 * No pins, no flags. The reference keeps its literal pool INSIDE the function and this
 * reproduces it -- 98 encodings and 14 relocations identical, so the in-function pool
 * words match too.
 *
 * ================ synth_mult CAPS AT THREE OPERATIONS ================
 *
 * This function's nine-operation shift/add chain CANNOT come from `r * 0x1999`. Measured
 * directly against this build: `t*3` synthesises in 2 ops, `t*12` in 3, `t*63` in 2,
 * `t*504` in 3, `t*1023` in 2 -- but `t*13` becomes `mov #13 / mul`, and `t*819`,
 * `t*6552` and `t*6553` all become a POOL LOAD plus `mul`.
 *
 * So a long shift/add chain in the ROM is always COMPOSITE IN THE SOURCE, and every
 * factor has to be under the cap. `u = t*12 + t; -(u*504 + t)` is the shortest of four
 * byte-identical spellings -- and note gcc does NOT fold `t*12 + t` into `t*13`, which is
 * what makes the split expressible at all.
 *
 * COROLLARY: a `mul` whose destination is the CONSTANT's register (`ldr r3, =K /
 * mul r3, r0`) is the ordinary above-the-cap constant multiply. Do not reach for the
 * destructive-mulsi3 lever for it -- that one is for a mask meeting a multiply.
 *
 * TWO MORE:
 *   - THE `int`-LOCAL MASK LEVER HAS AN ORDERING TWIN. A NAMED mask statement is
 *     scheduled before the load it masks. Where the ROM loads first, the destructive form
 *     `m = -0xd; m &= b;` makes the mask pseudo the result pseudo and fixes it -- that
 *     alone took this function from 13 differing to instruction-exact.
 *   - `(x & 1) == 1` IS FOLDED TO `!= 0`. Naming the bit first --
 *     `bit = Random() & 1; if (bit == 1)` -- preserves the ROM's `cmp r0, #1 / bne`.
 */
extern unsigned char *_CreateActor(int kind, int x, int y, int z);
extern unsigned int Random(void);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void _Actor_SetScript(unsigned char *a, unsigned char *s);
extern void Func_80929d8(unsigned char *a);
extern void Func_80925e0(void);
extern unsigned char L9fbec[] __asm__(".L9fbec");
extern unsigned char L9fc04[] __asm__(".L9fc04");

void Func_8092624(unsigned char *e, int f)
{
	unsigned char *n;
	unsigned char *s;
	unsigned char *t;
	int zero;
	int r;
	int w;
	int u;
	int bit;
	int m;
	int q;
	int b;

	n = _CreateActor(0xde, *(int *)(e + 8), *(int *)(e + 0xc),
	                 *(int *)(e + 0x10));
	if (n != 0) {
		s = *(unsigned char **)(n + 0x50);
		bit = Random() & 1;
		if (bit == 1) {
			_Actor_SetAnim(n, 2);
			_Actor_SetScript(n, L9fbec);
		} else {
			_Actor_SetAnim(n, 1);
			_Actor_SetScript(n, L9fc04);
		}
		if (f != 0)
			Func_80929d8(n);
		zero = 0;
		n[0x55] = zero;
		r = Random() % 10 + 5;
		w = r * 12 + r;
		*(int *)(n + 0x34) = -(w * 504 + r);
		u = (Random() % 15 - 7) * 2;
		*(int *)(n + 0x30) = u * 0x1999;
		*(short *)(n + 0x64) = zero;
		*(void **)(n + 0x6c) = Func_80925e0;
		s[0x26] = 0;
		t = *(unsigned char **)(e + 0x50);
		q = t[9] & 0xc;
		b = s[9];
		m = -0xd;
		m &= b;
		s[9] = m | q;
	}
}
