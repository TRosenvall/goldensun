/* Func_9a8b8 -- RunImpactAbility_Wrapper
 *
 * Tail call to Func_9a8c4; a second entry point for the ability table.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void Func_9a8c4();

void Func_9a8b8(void) { Func_9a8c4(); }
