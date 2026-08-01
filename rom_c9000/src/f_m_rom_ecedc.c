/* Func_ecedc .. Func_ecee8 -- 2 one-line wrappers
 *
 * Each supplies a constant argument to a shared implementation, so a dispatch
 * table can hold one address per variant.
 *
 * STATUS: ALL MATCHING (verified by a full `make compare`).
 *
 * The callees are declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */

extern void Func_ecef4();

void Func_ecedc(int a0) { Func_ecef4(a0, 1); }
void Func_ecee8(int a0) { Func_ecef4(a0, 2); }
