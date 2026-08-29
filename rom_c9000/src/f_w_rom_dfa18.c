/* Func_dfa18 .. Func_dfa3c -- 4 one-line wrappers
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

extern void Func_dfa48();

void Func_dfa18(int a0) { Func_dfa48(a0, 0); }
void Func_dfa24(int a0) { Func_dfa48(a0, 1); }
void Func_dfa30(int a0) { Func_dfa48(a0, 2); }
void Func_dfa3c(int a0) { Func_dfa48(a0, 3); }
