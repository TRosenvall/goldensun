/* Func_1c924 -- RunMenuScreenDefault
 *
 * Takes no arguments. Func_1c49c with the default parameters.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void Func_1c49c();

void Func_1c924(void) { Func_1c49c(); }
