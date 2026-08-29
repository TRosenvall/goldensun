/* Cluster OvlFunc_907_20080dc..OvlFunc_907_20080dc extracted from goldensun/asm/overlays/rom_79b154/ovl_30_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * Returns a table pointer in one area and NULL everywhere else.
 *
 * UNPARKED, AND IT HAD TWO WRONG SYMBOLS. The park read "logic faithful, does
 * NOT byte-match (endgame permuter seed)" and blamed a "scalar .L pool" class.
 * Both operands of the two-instruction diff were wrong:
 *
 *     rom    ldr r3, =0x20            ours  ldr r3, =SpecialExitTag
 *     rom    ldr r0, =gOvl_020093fc   ours  ldr r0, =.L13fc
 *
 * The comparison is against an AREA ID -- `gState[0xe0]` is the same halfword
 * GetEntrances reads -- so it is `_AREA_20`, already defined in area.sym, not a
 * separately named `SpecialExitTag`. And the returned table is a real global,
 * `gOvl_020093fc`, declared `.global` in ovl_30_c_c.s; the park had it as a
 * local `.L13fc` bound with an asm-label, which is the right technique for the
 * wrong symbol.
 *
 * Sixth park found carrying a semantic error rather than a codegen one, and the
 * third this session. All three were caught the same way: the diff was one or
 * two instructions and the differing operand was a VALUE or a NAME, not a
 * register. That check is cheap and should come before any reasoning about
 * allocation.
 */
extern short gState[];
extern unsigned char gOvl_020093fc[];
extern int _AREA_20;

unsigned char *OvlFunc_907_20080dc(void)
{
	if (gState[0xe0] == (int)&_AREA_20)
		return gOvl_020093fc;
	return (unsigned char *)0;
}
