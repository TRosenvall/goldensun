/* OvlFunc_939_20092a4 -- from
 * goldensun/asm/overlays/rom_7c460c/ovl_314_c_a_c_c_c_c.s.
 *
 * A per-frame ambient effect, gated to one frame in sixteen: pick a random
 * angle out of 52 buckets, bias it, and push the actor along that heading with
 * the cosine quartered and the sine halved.
 *
 * Preserves the original ROM layout when slotted before
 * asm/overlays/rom_7c460c/ovl_314_c_a_c_c_c_c_c.o in
 * goldensun/overlays/rom_7c460c/overlay.ld.
 *
 * I WROTE THE PARK THIS REPLACES AND ITS DIAGNOSIS WAS WRONG. It read the
 * ROM's `lsl r3, #0x6 / mov r5, r3 / add r5, #0xe6` as a shift-in-place
 * followed by a copy, filed it under the recorded failure mode of the
 * named-intermediate lever -- "the lever needs the two values simultaneously
 * live, which a shift's input and output are not" -- and recorded the split
 * spelling as inert at 38 differing.
 *
 * THE COPY IS NOT ABOUT THE SHIFT. It is forced by the ADD: Thumb has no
 * three-operand add for an immediate above 7, so `+ 0xe6` must be destructive,
 * and the shift result and the biased value therefore have to be two separate
 * pseudos. Writing
 *
 *     ang = X << 6;
 *     ang += 0xe6;
 *
 * is ONE pseudo -- the `+=` reassigns the same variable -- and gcc folds the
 * copy away. Writing it as one expression
 *
 *     ang = (X << 6) + 0xe6;
 *
 * gives gcc two values to hold and it emits the ROM's three instructions. That
 * is the whole difference, and it took the function from 2 aligned to a match.
 *
 * > WHERE THE ROM HAS `<op> rX, #n / mov rY, rX / add rY, #K` WITH K > 7,
 * > WRITE IT AS ONE EXPRESSION, NOT TWO STATEMENTS. The `+=` form collapses to
 * > a single pseudo. This generalises past shifts to any value feeding a large
 * > add, and it is the mirror of the recorded "derivation as its own
 * > statement" lever -- that one wants the boundary, this one must not have it.
 *
 * The precedent was already in the tree: src/rom_9000/rom_1219c_b.c writes
 * `off = ((layer & 3) << 2) + 0x28;` and emits exactly `lsl / mov / add`.
 *
 * MEASURED (rom 55 lines):
 *   the park's two-statement form                     2 aligned of 55
 *   the single-expression form                        MATCH
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned int iwram_3001e40;
extern unsigned int __Random(void);
extern int __cos(int a);
extern int __sin(int a);
extern void OvlFunc_common0_10c(int a, int b, int c, int d, int e, int f, int g, int h);

void OvlFunc_939_20092a4(void)
{
	unsigned char *a;
	int v[3];
	int z;
	int ang;
	int c;
	int s;

	a = __MapActor_GetActor(0);
	z = iwram_3001e40 & 0xf;
	if (z != 0)
		return;
	ang = (((__Random() * 0x34) >> 16) << 6) + 0xe6;
	c = __cos(ang);
	v[0] = c / 4;
	v[1] = z;
	s = __sin(ang);
	v[2] = s / 2;
	OvlFunc_common0_10c(*(int *)(a + 8), *(int *)(a + 0xc), *(int *)(a + 0x10),
			    v[0], v[1], v[2], z, z);
}
