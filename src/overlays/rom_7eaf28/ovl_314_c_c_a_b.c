/* OvlFunc_960_2008ce4 -- a palette pulse: fold the frame counter into a
 * triangle wave, scale it, splat it into all three channels and write the
 * result to one palette entry.
 *
 * Preserves the ROM layout when slotted between
 * asm/overlays/rom_7eaf28/ovl_314_c_c_a_a.o and ovl_314_c_c_a_c.o in
 * goldensun/overlays/rom_7eaf28/overlay.ld.
 *
 * THE PARK'S RESIDUE WAS `asr r3, #0x10` WHERE THE ROM HAS `lsr`, and its note
 * that declaring the local unsigned "is not enough" was correct for a reason
 * worth writing down. arm.h's PROMOTE_MODE reads
 *
 *     else if (MODE == HImode)  UNSIGNEDP = TARGET_MMU_TRAPS != 0;
 *
 * and TARGET_MMU_TRAPS is 0 in this configuration, so EVERY HImode local
 * promotes to SImode SIGN-extended regardless of how it is declared. No type on
 * the halfword local can ever produce the logical shift.
 *
 * THE FIX IS ON THE READ SIDE. Assigning the halfword into an `unsigned int`
 * before the store forces a zero-extending read and gives the ROM's `lsr`.
 * `(t << 16) >> 16` at the store works identically.
 *
 * MEASURED (rom 27 lines):
 *   the park's best                                      1 aligned
 *   `(unsigned)` casts on the or-operands                1
 *   the or written compound                              1
 *   result into an `unsigned int` and stored from it     2 (the pair vanishes)
 *   `& 0xffff` at the store or on the temp               5, 3, 2
 *   -malignment-traps / -mshort-load-bytes               14 each
 *   `u = n;` then store `u`                              MATCH
 *   `(t << 16) >> 16` at the store                       MATCH
 */
extern unsigned int iwram_3001e40;

void OvlFunc_960_2008ce4(void)
{
	unsigned short v, n;
	unsigned int u;

	v = iwram_3001e40 & 0x3f;
	if (v > 0x1f)
		v = 0x40 - v;
	n = (v >> 1) + 7;
	n = n | ((n << 10) | (n << 5));
	u = n;
	*(volatile unsigned short *)0x500019e = u;
}
