/* Func_c9c60 .. Func_c9c9c -- 6 one-line wrappers
 *
 * Each supplies a constant argument to a shared implementation, so a dispatch
 * table can hold one address per variant.
 *
 * STATUS: ALL MATCHING (verified by a full `make compare`).
 *
 * The callees are declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */

extern void Func_c9ca8();

void Func_c9c60(int a0) { Func_c9ca8(a0, 0); }
void Func_c9c6c(int a0) { Func_c9ca8(a0, 1); }
void Func_c9c78(int a0) { Func_c9ca8(a0, 2); }
void Func_c9c84(int a0) { Func_c9ca8(a0, 4); }
void Func_c9c90(int a0) { Func_c9ca8(a0, 3); }
void Func_c9c9c(int a0) { Func_c9ca8(a0, 1); }
