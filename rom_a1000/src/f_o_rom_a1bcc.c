/* Func_a1bcc -- LayOutGrid8
 *
 * Takes no arguments. Func_a1bdc(0x6C, 0x28, 8) -- the eight-column grid the
 * item and Psynergy lists use.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void Func_a1bdc();

void Func_a1bcc(void) { Func_a1bdc(108, 40, 8); }
