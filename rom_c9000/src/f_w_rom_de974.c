/* Func_de974 .. Func_de9b0 -- 6 one-line wrappers
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

extern void Func_dea70();

void Func_de974(int a0) { Func_dea70(a0, 0); }
void Func_de980(int a0) { Func_dea70(a0, 1); }
void Func_de98c(int a0) { Func_dea70(a0, 2); }
void Func_de998(int a0) { Func_dea70(a0, 3); }
void Func_de9a4(int a0) { Func_dea70(a0, 11); }
void Func_de9b0(int a0) { Func_dea70(a0, 4); }
