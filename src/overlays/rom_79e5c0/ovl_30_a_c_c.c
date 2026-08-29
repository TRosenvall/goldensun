/* Cluster OvlFunc_911_20081ac..OvlFunc_911_20081ac extracted from goldensun/asm/overlays/rom_79e5c0/ovl_30_a_c_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * Returns a table pointer in one area and NULL everywhere else.
 *
 * UNPARKED, AND THE PARK WAS WRONG ABOUT WHAT WAS BLOCKING IT. It was filed as
 * "logic faithful, does NOT byte-match (endgame permuter seed)" with the
 * residual described as a "scalar .L pool" class. The logic was not faithful:
 * the C compared against `_AREA_38` where the ROM compares against 0x26.
 *
 *     rom    ldr r3, =0x26
 *     ours   ldr r3, =0x38
 *
 * One symbol, wrong value. `_AREA_26` was already defined in area.sym.
 * Changing the name is the whole fix and it matches.
 *
 * This is the fourth park found carrying a SEMANTIC error under a plausible
 * blocker diagnosis, and the first where the error is a wrong constant rather
 * than wrong control flow. tools/audit_parks.py catches the control-flow kind
 * by looking for displaced labels; it cannot see this one, because a wrong
 * constant produces a perfectly well-formed single-instruction diff that looks
 * exactly like codegen noise.
 *
 * The lesson for reading a one-instruction diff: check whether the two sides
 * differ in an OPERAND VALUE before assuming they differ in register allocation.
 * A value difference is a bug in the C.
 */
extern short gState[];
extern unsigned char L3010[] __asm__(".L3010");
extern int _AREA_26;

unsigned char *OvlFunc_911_20081ac(void)
{
	int v;
	v = gState[0xe0];
	if (v == (int)&_AREA_26)
		return L3010;
	return (unsigned char *)0;
}
