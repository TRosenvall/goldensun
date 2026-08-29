/* Func_f9558 .. Func_f9564 -- 2 one-line wrappers
 *
 * Each supplies a constant argument to a shared implementation, so a dispatch
 * table can hold one address per variant.
 *
 * STATUS: ALL MATCHING (verified by a full `make compare`).
 *
 * The callees are declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */

extern void Func_fa458();
extern void Func_fa490();

void Func_f9558(void) { Func_fa458(); }
void Func_f9564(void) { Func_fa490(); }
