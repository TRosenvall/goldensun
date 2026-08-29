/* Func_d9194 .. Func_d91d0 -- 6 one-line wrappers
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

extern void Func_d91dc();

void Func_d9194(int a0) { Func_d91dc(a0, 0); }
void Func_d91a0(int a0) { Func_d91dc(a0, 1); }
void Func_d91ac(int a0) { Func_d91dc(a0, 2); }
void Func_d91b8(int a0) { Func_d91dc(a0, 3); }
void Func_d91c4(int a0) { Func_d91dc(a0, 5); }
void Func_d91d0(int a0) { Func_d91dc(a0, 4); }
