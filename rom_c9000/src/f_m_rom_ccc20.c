/* Func_ccc20 .. Func_ccc2c -- 2 one-line wrappers
 *
 * Each supplies a constant argument to a shared implementation, so a dispatch
 * table can hold one address per variant.
 *
 * STATUS: ALL MATCHING (verified by a full `make compare`).
 *
 * The callees are declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */

extern void Func_ccc38();

void Func_ccc20(int a0) { Func_ccc38(a0, 0); }
void Func_ccc2c(int a0) { Func_ccc38(a0, 1); }
