/* Func_dea24 .. Func_dea30 -- 2 one-line wrappers
 *
 * Each supplies a constant argument to a shared implementation, so a dispatch
 * table can hold one address per variant.
 *
 * STATUS: ALL MATCHING (verified by a full `make compare`).
 *
 * The callees are declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */

extern void Func_dea70();

void Func_dea24(int a0) { Func_dea70(a0, 7); }
void Func_dea30(int a0) { Func_dea70(a0, 10); }
