/* Func_d5274 .. Func_d52bc -- 7 one-line wrappers
 *
 * Each supplies a constant argument to a shared implementation, so a dispatch
 * table can hold one address per variant.
 *
 * STATUS: ALL MATCHING (verified by a full `make compare`).
 *
 * The callees are declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */

extern void Func_d52c8();

void Func_d5274(int a0) { Func_d52c8(a0, 1); }
void Func_d5280(int a0) { Func_d52c8(a0, 2); }
void Func_d528c(int a0) { Func_d52c8(a0, 3); }
void Func_d5298(int a0) { Func_d52c8(a0, 4); }
void Func_d52a4(int a0) { Func_d52c8(a0, 5); }
void Func_d52b0(int a0) { Func_d52c8(a0, 7); }
void Func_d52bc(int a0) { Func_d52c8(a0, 6); }
