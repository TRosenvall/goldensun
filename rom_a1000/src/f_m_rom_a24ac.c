/* Func_a24ac .. Func_a24c4 -- 3 one-line wrappers
 *
 * Each supplies a constant argument to a shared implementation, so a dispatch
 * table can hold one address per variant.
 *
 * STATUS: ALL MATCHING (verified by a full `make compare`).
 *
 * The callees are declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */

extern void _Func_1e71c();

void Func_a24ac(void) { _Func_1e71c(15); }
void Func_a24b8(void) { _Func_1e71c(2); }
void Func_a24c4(void) { _Func_1e71c(4); }
