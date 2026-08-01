/* Func_d8948 .. Func_d8960 -- 3 one-line wrappers
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

extern void Func_d89ac();

void Func_d8948(int a0) { Func_d89ac(a0, 0); }
void Func_d8954(int a0) { Func_d89ac(a0, 1); }
void Func_d8960(int a0) { Func_d89ac(a0, 2); }
