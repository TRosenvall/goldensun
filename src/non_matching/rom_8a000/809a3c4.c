/* Func_809a3c4 (0x0809a3c4) -- NON-MATCHING.
 * Blocker class: LOOP-INVARIANT CONSTANT NOT HOISTED (see docs/elevation.md).
 *
 * 62 lines against the ROM's 62, 16 differing, and the sixteen are one
 * instruction's worth of damage: the ROM hoists the loop's step constant into a
 * callee-saved register before the loop, we rebuild it inside the loop, and
 * every line after that point is shifted by one.
 *
 *     rom    ldr r6, =0x2000        <- once, above .L2
 *            .L2: ... add r3, r6
 *
 *     ours   .L2: ... mov r2, #0x80 / lsl r2, #0x6 / add r3, r2
 *
 * gcc-2.96 declines to hoist here because it prices `mov`+`lsl` (two cheap
 * instructions, no pool entry) below a pool load, and 0x2000 is exactly the
 * imm8<<shift shape that build covers. The ROM's compiler priced it the other
 * way. Nothing in the C reaches that decision.
 *
 * THE ONE REAL FIX, worth keeping. Before it, the body was 61 lines against 62:
 * gcc reused the register still holding 0xc0<<10 -- whose low byte is zero --
 * to serve `*(char *)(p + 0x5a) = 0`, where the ROM materialises `mov r3, #0x0`
 * on its own. Assigning the zero through a named local first restores the
 * separate materialisation and takes the length to exact. That is the
 * naming-a-stored-value lever applied to a value whose bit pattern happens to
 * be a SUBSET of a live register's, which the recorded note does not cover --
 * it is written for exact duplicates.
 *
 * MEASURED:
 *   plain `*(char *)(p + 0x5a) = 0;`                 61 lines vs 62, 33 differing
 *   the zero named in a local first (below)          62 lines vs 62, 16
 *   step hoisted to the entry block                  62 vs 62, 16
 *   step assigned above the guard instead of inside  62 vs 62, 16
 *
 * The last two are the dominating-block lever, tried in both placements. Both
 * inert: gcc folds the constant back into the loop body regardless of where the
 * C assigns it, because the fold happens after the source structure is gone.
 *
 * WHAT IS RIGHT: the 0x8a sound cue, the particle actor spawn and its null
 * guard, both 0x18/0x1c scale writes and both 0x30/0x34 writes, the 0x5a clear,
 * the anim set, the 0x80<<4 per-frame growth with the 0xffff loop bound, the
 * per-frame rotation step, and the final rotation store. Only the placement of
 * the step constant is wrong.
 */
extern void _PlaySound(int id);
extern unsigned char *CreateParticleActor(int a, int b, int c, int d);
extern void _Actor_SetAnim(unsigned char *p, int n);
extern void WaitFrames(int n);

unsigned char *Func_809a3c4(int a, int b, int c, int d)
{
	unsigned char *p;
	int v;
	int w;
	int t;
	int step;
	int z;

	_PlaySound(0x8a);
	p = CreateParticleActor(0xd7, a, b, c);
	if (p != 0) {
		v = 0x80 << 7;
		*(int *)(p + 0x1c) = v;
		*(int *)(p + 0x18) = v;
		w = 0xc0 << 10;
		*(int *)(p + 0x30) = w;
		*(int *)(p + 0x34) = w;
		z = 0;
		*(char *)(p + 0x5a) = z;
		_Actor_SetAnim(p, 1);
		t = *(int *)(p + 0x18);
		if (t < (0x80 << 9)) {
			step = 0x2000;
			do {
				t += 0x80 << 4;
				*(int *)(p + 0x1c) = t;
				*(int *)(p + 0x18) = t;
				*(unsigned short *)(p + 6) += step;
				WaitFrames(1);
				t = *(int *)(p + 0x18);
			} while (t <= 0xffff);
		}
		*(unsigned short *)(p + 6) = d;
	}
	return p;
}
