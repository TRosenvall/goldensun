/* Func_98ccc -- RunFreezeAbility_Wrapper
 *
 * Tail call to Func_98cd8; a second entry point for the ability table.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void Func_98cd8();

void Func_98ccc(void) { Func_98cd8(); }
