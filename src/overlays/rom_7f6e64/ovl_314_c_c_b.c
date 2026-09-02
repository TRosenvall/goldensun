/* OvlFunc_969_200d9f0 -- from goldensun/asm/overlays/rom_7f6e64/ovl_314_c_c.s.
 *
 * A per-frame effect step: bail unless the enable byte at +0x63 is set, offset
 * the actor's y from its base by a quarter of the counter at +0x62, run the
 * shared update, then advance that counter while it is inside 1..0x1f.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7f6e64/ovl_314_c_c_a.o and ovl_314_c_c_c.o in
 * goldensun/overlays/rom_7f6e64/overlay.ld.
 *
 * THIS PARK'S CONCLUSION WAS WRONG AND IT SAID SO IN ITS OWN CITATION. It read
 * the ROM's `ldrb r3, [r5] / mov r2, r3 / cmp r2, #0` as an elided copy, wrote
 * "the copy is a symptom of register pressure in the original, not of how the
 * source is spelled", and pointed at src/non_matching/rom_b5000/80bf54c.c as
 * the same shape. Batch 179 MATCHED that function. The copy is the
 * CSEd-second-read signature: the source reads the counter once for the tests
 * and again for the increment, and gcc turns the second read into the copy.
 *
 * Reading everything through the pointer instead of carrying a local matched on
 * the FIRST screen -- 27 lines against 27, where the park's best was 26 against
 * 27 with nine differing. The park had also recorded that swapping the two
 * loads in source order was byte-identical, which is true and beside the point:
 * the register rotation it was chasing was a consequence of the missing read,
 * not an independent problem.
 *
 * That is the fourth park in three batches whose "not reachable from source"
 * conclusion the read-count rule has overturned.
 */
extern void OvlFunc_969_200d688(void *a);

void OvlFunc_969_200d9f0(unsigned char *actor)
{
	unsigned char *k;
	unsigned char *p;

	k = actor;
	k += 0x63;
	if (*k == 0)
		return;
	p = actor;
	p += 0x62;
	*(int *)(actor + 0xc) = *(int *)(actor + 0x4c) + ((*p >> 2) << 16);
	OvlFunc_969_200d688(actor);
	if (*p == 0)
		return;
	if (*p > 0x1f)
		return;
	*p = *p + 1;
}
