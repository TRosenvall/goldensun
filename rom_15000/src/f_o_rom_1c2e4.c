/* Func_1c2e4 -- RestoreAfterMenu
 *
 * Takes no arguments. Forwards to Func_1f5d4.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void Func_1f5d4();

void Func_1c2e4(void) { Func_1f5d4(); }
