/* Func_e7320, Func_e732c -- two one-line wrappers
 *
 * Each supplies a constant second argument to Func_e7404. They exist so a
 * dispatch table can hold one address per variant.
 *
 * STATUS: MATCHING (verified by a full `make compare`).
 *
 * Func_e7404 is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */

extern void Func_e7404();

void Func_e7320(int a0) { Func_e7404(a0, 0); }
void Func_e732c(int a0) { Func_e7404(a0, 1); }
