/* Func_d8988 .. Func_d89a0 -- 3 one-line wrappers
 *
 * Each supplies a constant argument to a shared implementation, so a dispatch
 * table can hold one address per variant.
 *
 * STATUS: ALL MATCHING (verified by a full `make compare`).
 *
 * The callees are declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */

extern void Func_d89ac();

void Func_d8988(int a0) { Func_d89ac(a0, 4); }
void Func_d8994(int a0) { Func_d89ac(a0, 5); }
void Func_d89a0(int a0) { Func_d89ac(a0, 6); }
