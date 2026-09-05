/* Func_809a3c4 -- 0x0809a3c4
 *
 * Twin of Func_8098a84 (src/rom_8a000/rom_97b54_a_c_a_c_c_b.c): spawn a
 * particle actor, set its two scale pairs, clear a sprite selector, grow it a
 * frame at a time while rotating, then stamp its rotation. Differs only in the
 * anim index and a plain byte store where the twin clears a bitfield.
 *
 * THIS FILE WAS PARKED, AND THE PARK'S CONCLUSION WAS WRONG. It sat at 16 of 62
 * asserting that the ROM's hoisted `ldr r6, =0x2000` was unreachable -- "gcc
 * prices `mov`+`lsl` (two instructions, no pool entry) below a pool load, and
 * nothing in the C reaches that decision" -- after measuring the step constant
 * hoisted to the entry block, assigned above the guard, and assigned inside it,
 * all tying at 16.
 *
 * All three of those spellings NAMED the constant. That is the whole error:
 *
 *     int step; ... step = 0x2000; ... *(u16 *)(p + 6) += step;   16 of 62
 *     *(u16 *)(p + 6) += 0x2000;                                  EXACT
 *
 * A NAMED loop-invariant is cheap for gcc to rematerialise per iteration, so it
 * does; an UNNAMED one becomes a pool reference, and loop.c hoists pool loads
 * out of the loop. The park's own reasoning about instruction pricing was
 * right and led to the wrong conclusion, because the choice it describes is not
 * made until after the constant has already been turned into a pool entry --
 * and whether that happens is decided by naming.
 *
 * The twin measured this properly: NINE spellings, every named form tying at
 * 16, only the bare literal reaching it. The shared assumption behind all nine
 * is that a loop-invariant must be named to survive the call inside the loop.
 * It is exactly backwards.
 *
 * ON VERIFYING: tryc reports this DIRTY at 11 differing, because gcc emits an
 * extra label at the same address as another and the whole tail of the diff is
 * one phantom line shift. tools/objcmp.py shows it exact -- 136 bytes, 61
 * encodings and 4 relocations identical. A park resting on a tryc score alone
 * is a park that has not been checked.
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
			do {
				t += 0x80 << 4;
				*(int *)(p + 0x1c) = t;
				*(int *)(p + 0x18) = t;
				*(unsigned short *)(p + 6) += 0x2000;
				WaitFrames(1);
				t = *(int *)(p + 0x18);
			} while (t <= 0xffff);
		}
		*(unsigned short *)(p + 6) = d;
	}
	return p;
}
