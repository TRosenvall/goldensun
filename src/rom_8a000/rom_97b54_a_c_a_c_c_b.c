/* Func_8098a84 -- 0x08098a84
 *
 * Spawns a particle actor, sets its two scale pairs, clears a sprite selector,
 * then grows it a frame at a time while rotating it, and finally stamps its
 * rotation from the caller's argument.
 *
 * DO NOT NAME THE ROTATION STEP -- and this one CONTRADICTS A PARK. Written as
 * a named `int` local assigned before the loop, gcc rematerialises
 * `mov r2, #0x80 / lsl r2, #6` INSIDE the loop; written as the bare inline
 * literal `*(unsigned short *)(p + 6) += 0x2000;` gcc pools it and loop.c
 * hoists the pool load into the preheader, which is exactly what the ROM has.
 *
 * NINE spellings of that one step were measured and every named form ties at
 * 16 differing -- `int`, `unsigned short`, three placements of the assignment,
 * an explicit read-add-write, a signed store, and a named halfword temp. Only
 * the inline literal reaches it. The shared assumption behind all nine is that
 * a loop-invariant must be NAMED so it survives the call in the loop; it is the
 * opposite -- naming it is what lets gcc rematerialise it cheaply per
 * iteration, and only an unnamed one is expensive enough to be worth hoisting.
 *
 * src/non_matching/rom_8a000/809a3c4.c parks the twin of this function at 16 of
 * 62 saying the hoisted `ldr r6, =0x2000` is unreachable -- "gcc prices
 * mov+lsl below a pool load; nothing in the C reaches that decision". It is
 * reachable, and that park should be re-attacked with the literal.
 *
 * No const.sym entry is needed: 0x2000 pools from a plain literal here, which
 * is a live counterexample to the pooled-small-value tell. This is the halfword
 * -operand exception already in const.sym's header (the addend meets an
 * `unsigned short`) -- except that hoisting it out of the loop turns it into a
 * word-sized `ldr` rather than the `ldrh` the header predicts.
 *
 * TWO MORE LEVERS:
 *  - A BITFIELD for the mask. The ROM builds a 32-bit `~0xc` with
 *    `mov r3, #0xd / neg r3, r3`; writing `q[9] &= ~0xc` narrows it to
 *    `mov r3, #0xf3`. Declaring the byte as `lo:2, sel:2, hi:4` and assigning
 *    the field puts store_bit_field in charge and restores the `neg` pair.
 *  - `do { } while (0)` between the four scale stores and the sprite-pointer
 *    load, or gcc hoists that load up among the stores. Worth nine encodings.
 *
 * ON VERIFYING THIS ONE: tryc reports it dirty at 11 differing, because gcc
 * emits `b L3 / L3: / L1:` where the ROM has `b L1 / L1:` -- the documented
 * two-labels-at-one-address false negative. The whole tail of that diff is one
 * phantom line shift. tools/objcmp.py is the only thing that shows it exact:
 * 140 bytes, 63 encodings and 4 relocations identical.
 */
struct Sub {
	unsigned char pad0[9];
	unsigned char lo : 2;
	unsigned char sel : 2;
	unsigned char hi : 4;
};

extern void _PlaySound(int id);
extern unsigned char *CreateParticleActor(int a, int b, int c, int d);
extern void _Actor_SetAnim(unsigned char *p, int n);
extern void WaitFrames(int n);

unsigned char *Func_8098a84(int a, int b, int c, int d)
{
	unsigned char *p;
	struct Sub *q;
	int v;
	int w;
	int t;

	_PlaySound(0x8a);
	p = CreateParticleActor(0xd7, a, b, c);
	if (p != 0) {
		v = 0x80 << 7;
		*(int *)(p + 0x1c) = v;
		*(int *)(p + 0x18) = v;
		w = 0xc0 << 10;
		*(int *)(p + 0x30) = w;
		*(int *)(p + 0x34) = w;
		do { } while (0);
		q = *(struct Sub **)(p + 0x50);
		q->sel = 0;
		_Actor_SetAnim(p, 3);
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
