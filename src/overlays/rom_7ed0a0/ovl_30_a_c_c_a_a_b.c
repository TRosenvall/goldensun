/* OvlFunc_964_20094ac -- sets a progress flag, then shows or hides one map
 * actor and flips its collidable bit to match.
 *
 * Preserves the ROM layout when slotted after
 * asm/overlays/rom_7ed0a0/ovl_30_a_c_c_a_a_a.o in
 * goldensun/overlays/rom_7ed0a0/overlay.ld.
 *
 * TWO LEVERS, APPLIED IN ORDER, and the first is the one that mattered.
 *
 *   DELETE THE VALUE LOCAL. The park carried the read-modify-write through a
 *   named `v`. Storing straight through the pointer took it from 4 aligned to
 *   2, and made the AND arm exact on its own. This is the read-count rule
 *   again -- the local collapses a second read the ROM makes.
 *
 *   THEN THE `orr` DESTINATION RULE, ON ONE ARM ONLY. With the value local
 *   gone, the OR arm still wanted its constant in the destination register,
 *   which the recorded lever gets by naming it in a local of the FIELD'S OWN
 *   type. Applying `unsigned char m = 8;` to the else-arm and leaving the AND
 *   arm's mask a literal matches. Applying it to both, or using `int m`, does
 *   not.
 *
 * MEASURED (rom 36 lines):
 *   the park's best                                      4 aligned
 *   the address-only local dropped                       4
 *   the named mask dropped                               5
 *   the value local dropped                              2
 *   `*b |= 8` / `8 | *b` / `int m`                       2 each
 *   value local dropped + `unsigned char m` on the OR arm  MATCH
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);

void OvlFunc_964_20094ac(void)
{
	unsigned char *a;
	unsigned char *b;
	unsigned char m;

	__SetFlag(0x201);
	if (__GetFlag(0x80 << 2) != 0) {
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
