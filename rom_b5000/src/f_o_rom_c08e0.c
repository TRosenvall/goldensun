/* Func_c08e0 -- FreeSceneBuffer
 *
 * Takes no arguments. Releases the scene buffer with Func_2dd8.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void Func_2dd8();

void Func_c08e0(void) { Func_2dd8(10); }
