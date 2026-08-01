/* Func_ca1e4 .. Func_ca1f0 -- 2 one-line wrappers
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

extern void Func_ca1fc();

void Func_ca1e4(int a0) { Func_ca1fc(a0, 1); }
void Func_ca1f0(int a0) { Func_ca1fc(a0, 0); }
