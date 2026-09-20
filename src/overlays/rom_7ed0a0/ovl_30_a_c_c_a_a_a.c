/* Cluster OvlFunc_964_2009458..OvlFunc_964_2009458 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_c_c_a_a.s.
 *
 * Total .text for this TU = 84 bytes (= 0x54).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7ed0a0/ovl_30_a_c_b.o and asm/overlays/rom_7ed0a0/ovl_30_a_c_c_a_a_b.o in
 * goldensun/overlays/rom_7ed0a0/overlay.ld.
 *
 * Solved in batch 271 and landed in 272. Was parked at 3 of 36. No pins.
 *
 * THE LEVER: PUT THE SHARED STORE IN BOTH ARMS.
 *
 * Keeping `*q = v` after the join makes `v` a GLOBAL allocno. local-alloc then
 * hands the arm's own local (the loaded byte) r3 first and `v` takes r2 -- which
 * is the whole three-instruction residue. Writing the store inside each arm makes
 * the result block-local, the mask/result quantity wins the priority sort and
 * takes r3, the loaded byte gets r2, and jump2 CROSS-JUMPING merges the two
 * identical `strb r3, [r0]` tails back into the ROM's single shared store.
 *
 * So the duplicated store is NOT in the emitted code and costs nothing. It exists
 * only to change where the value is born.
 *
 * THE TWIN ALREADY KNEW: OvlFunc_964_20094ac, in
 * src/overlays/rom_7ed0a0/ovl_30_a_c_c_a_a_b.c right beside this file, uses
 * exactly this shape. The finding never made it back to the park.
 *
 * MEASURED: per-arm store with a plain `v = 0xf7 & t`, 3; a named `int m = 0xf7`
 * with the SHARED store, 4; `v = 0xf7; v &= t` with the shared store, 7; the store
 * literal reused as the mask variable, 13/15 at 37 lines; no `t` intermediate with
 * the per-arm store, 23. A second exact spelling (a `v = 0xf7; v &= t;`
 * accumulator with the per-arm store) is also byte-identical.
 */
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);

void OvlFunc_964_2009458(void)
{
	unsigned char *a;
	unsigned char *b;
	unsigned char m;

	__SetFlag(0x80 << 2);
	if (__GetFlag(0x201) != 0) {
		a = __MapActor_GetActor(0xe);
		a += 0x62;
		*a = 0;
		b = __MapActor_GetActor(0xe);
		b += 0x59;
		*b = *b & 0xf7;
	} else {
		a = __MapActor_GetActor(0xe);
		a += 0x62;
		*a = 1;
		b = __MapActor_GetActor(0xe);
		b += 0x59;
		m = 8;
		*b = *b | m;
	}
}
