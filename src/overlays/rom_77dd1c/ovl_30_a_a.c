/* OvlFunc_882_2008030 -- the whole of
 * goldensun/asm/overlays/rom_77dd1c/ovl_30_a_a.s, so no split was needed and
 * the overlay's linker script is unchanged.
 *
 * A wandering prop's per-frame tick: when its countdown at +0x64 reaches zero,
 * pick a fresh facing and reload the countdown with Random() % 0x14 + 0x14,
 * then decrement it. Always returns 1.
 *
 * THE PARK THIS REPLACES READ THE BLOCKER BACKWARDS. It named the `mov r0, r3`
 * after the `ldrsh` as a copy two source variables could not reproduce. It is
 * the CSEd-second-read signature: the ROM re-reads the counter for the
 * decrement, and gcc folds that second read into the copy at the load site.
 * Carrying the value in a local gives ONE read and a body one instruction
 * short -- exactly the tell batch 178 recorded. `a->f64--` through the struct
 * emits the copy and the rest of the stream with it.
 *
 * The sibling in the same .s, src/overlays/rom_77dd1c/ovl_30_a_a_b.c
 * (OvlFunc_882_2008064), was the confirmation: same countdown field, same
 * Random() reload, matched with the same spelling.
 *
 * THE FIELD MUST BE SIGNED. `short f64` gives the ROM's `ldrsh`; `unsigned
 * short` kills it and costs fourteen lines. Naming the decrement's intermediate
 * as a `short` is worse still -- it forces a redundant lsl/asr/lsr
 * sign-extension triple and swaps the r5/r6 assignment.
 *
 * MEASURED (rom 23 lines):
 *   struct with signed `short f64`, `a->f64--`        23 lines, MATCH
 *   the same written `a->f64 = a->f64 - 1;`           23, MATCH (byte-identical)
 *   `void *` param, `short *p`, `*p = *p - 1`         23, MATCH (byte-identical)
 *   the park's named local carried across the join    22, 19
 *   as matched but `unsigned short f64`               23, 14
 *   a named `short` intermediate for the decrement    25, 23
 */
struct A {
	unsigned char pad00[6];
	unsigned short f06;
	unsigned char pad08[0x5c];
	short f64;
};

extern unsigned int __Random(void);

int OvlFunc_882_2008030(struct A *a)
{
	if (a->f64 == 0) {
		a->f06 = __Random();
		a->f64 = __Random() % 0x14 + 0x14;
	}
	a->f64--;
	return 1;
}
