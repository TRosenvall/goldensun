/* OvlFunc_943_20088e0 -- the whole of
 * goldensun/asm/overlays/rom_7c7b9c/ovl_30_a_c_c.s, so no split was needed and
 * the overlay's linker script is unchanged.
 *
 * An ambient prop's timer: while the byte countdown at +0x62 is non-zero it
 * ticks down; on reaching zero it rolls a die to pick one of three values for
 * the halfword at +6 and reloads the timer with 80..159 frames.
 *
 * THE PARK CALLED THIS A THREE-WAY CYCLIC REGISTER ROTATION. It was not. The
 * rotation was a symptom of a missing pseudo, and the pseudo appears only when
 * COALESCING IS BLOCKED BY A TYPE DIFFERENCE.
 *
 * A TYPE DIFFERENCE, NOT A SECOND READ, IS WHAT SPLITS THIS COPY. The ROM has
 * `ldrb r3, [p] / mov r7, r3` with the copy BEFORE the `cmp`. Every int-typed
 * spelling either lost the `mov` entirely or emitted it after the `beq` as
 * `mov r3, r5`. Declaring the compared copy `unsigned char` while the stored
 * value stays wide makes the QImode/SImode pair non-coalescable, the copy lands
 * before the compare, and the r5/r6/r7 rotation falls into the ROM's order for
 * free. That single change took 45 differing to 1.
 *
 * > This is the MIRROR of the read-count lever, not an instance of it. When the
 * > ROM has `ldrb rA / mov rB, rA` and your `mov` is on the wrong side of the
 * > `cmp`, do not add a second read -- give the compared copy a NARROWER TYPE
 * > than the stored value. A genuine second read here is byte-identical to the
 * > single read; CSE folds it away.
 *
 * TWO OTHER LEVERS, both already recorded and both load-bearing here. The store
 * must be INSIDE each arm -- a single trailing store lets gcc speculate the
 * cheap arm and inverts the guard. And `v + 0xff` needs `unsigned int v`: with
 * `int` gcc canonicalises both `v - 1` and `v + 0xff` to `sub r3, #0x1`, where
 * unsigned gives the ROM's `add r3, #0xff`. Writing the decrement as its own
 * statement before the store also works with `int`.
 *
 * MEASURED (rom 46 lines):
 *   the park's baseline, guard on an int local            46 lines, 11 differing
 *   int local, store inside each arm                      45, 45
 *   two int locals                                        45, 45
 *   `unsigned char c` copy, single trailing store         44, 44
 *   `short c` copy, single trailing store                 44, 44
 *   `unsigned char c`, stores in arms, `int v`, `v - 1`   46, 1
 *   the same with `v + 0xff` and `int v`                  46, 1
 *   the same with `unsigned int v`                        46, MATCH
 *   `v = v + 0xff; *p = v;` split, `int v`                46, MATCH
 */
extern unsigned int __Random(void);

struct A {
	unsigned char pad00[6];
	short f06;
	unsigned char pad08[0x5a];
	unsigned char f62;
};

int OvlFunc_943_20088e0(struct A *a)
{
	unsigned char *p;
	unsigned int v;
	unsigned char c;
	unsigned int r;

	p = &a->f62;
	v = *p;
	c = v;
	if (c != 0) {
		*p = v + 0xff;
	} else {
		r = (__Random() * 300) >> 16;
		if (r > 200) {
			a->f06 = 0xd0 << 8;
		} else if (r > 100) {
			a->f06 = 0xa0 << 7;
		} else {
			a->f06 = c;
		}
		*p = ((__Random() * 80) >> 16) + 0x50;
	}
	return 1;
}
