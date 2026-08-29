/* Func_1c17c -- FreeScreenTiles
 *
 * r0 = handle. Releases OBJ tiles with Func_3f3c.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void Func_3f3c();

void Func_1c17c(void) { Func_3f3c(); }
