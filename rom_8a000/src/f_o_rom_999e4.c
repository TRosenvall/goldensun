/* Func_999e4 -- RunBreakAbility_Wrapper
 *
 * Tail call to Func_999f0; a second entry point for the ability table.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void Func_999f0();

void Func_999e4(void) { Func_999f0(); }
