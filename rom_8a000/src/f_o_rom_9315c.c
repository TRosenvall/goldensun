/* Func_9315c -- ShowMessageAtXY_Wrapper
 *
 * Tail call to Func_93168 with the arguments untouched; a separate entry point
 * so the export table can expose it under its own address.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void Func_93168();

void Func_9315c(void) { Func_93168(); }
