/* Func_dbbdc .. Func_dbc24 -- 7 one-line wrappers
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

extern void Func_dbc30();

void Func_dbbdc(int a0) { Func_dbc30(a0, 0); }
void Func_dbbe8(int a0) { Func_dbc30(a0, 2); }
void Func_dbbf4(int a0) { Func_dbc30(a0, 6); }
void Func_dbc00(int a0) { Func_dbc30(a0, 3); }
void Func_dbc0c(int a0) { Func_dbc30(a0, 5); }
void Func_dbc18(int a0) { Func_dbc30(a0, 7); }
void Func_dbc24(int a0) { Func_dbc30(a0, 4); }
