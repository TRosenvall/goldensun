/* Func_db6c8 .. Func_db6d4 -- 2 one-line wrappers
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

extern void Func_db6e0();

void Func_db6c8(int a0) { Func_db6e0(a0, 1); }
void Func_db6d4(int a0) { Func_db6e0(a0, 0); }
