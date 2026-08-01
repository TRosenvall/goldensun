/* Func_dd2ac .. Func_dd2b8 -- 2 one-line wrappers
 *
 * Each supplies a constant argument to a shared implementation. They exist so
 * a dispatch table can hold one address per variant.
 *
 * STATUS: ALL MATCHING (verified per function with tools/asmdiff.py, and by a
 * full `make compare`).
 *
 * The callees are declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0. Declaring them int makes agbcc emit
 * `pop {r1}; bx r1` instead and the match is lost.
 */

extern void Func_dd2c4();

void Func_dd2ac(int a0) { Func_dd2c4(a0, 0); }
void Func_dd2b8(int a0) { Func_dd2c4(a0, 1); }
