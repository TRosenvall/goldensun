/* Func_cf88c .. Func_cf8d4 -- 7 one-line wrappers
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

extern void Func_cf8e0();

void Func_cf88c(int a0) { Func_cf8e0(a0, 4); }
void Func_cf898(int a0) { Func_cf8e0(a0, 5); }
void Func_cf8a4(int a0) { Func_cf8e0(a0, 0); }
void Func_cf8b0(int a0) { Func_cf8e0(a0, 1); }
void Func_cf8bc(int a0) { Func_cf8e0(a0, 6); }
void Func_cf8c8(int a0) { Func_cf8e0(a0, 2); }
void Func_cf8d4(int a0) { Func_cf8e0(a0, 3); }
