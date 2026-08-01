/* Func_e291c .. Func_e2940 -- 4 one-line wrappers
 *
 * Each supplies a constant argument to a shared implementation, so a dispatch
 * table can hold one address per variant.
 *
 * STATUS: ALL MATCHING (verified by a full `make compare`).
 *
 * The callees are declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */

extern void Func_e2974();

void Func_e291c(int a0) { Func_e2974(a0, 0); }
void Func_e2928(int a0) { Func_e2974(a0, 0); }
void Func_e2934(int a0) { Func_e2974(a0, 1); }
void Func_e2940(int a0) { Func_e2974(a0, 2); }
