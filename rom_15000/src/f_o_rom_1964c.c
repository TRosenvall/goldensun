/* Func_1964c -- ReleaseAllMenuBuffers
 *
 * Takes no arguments. Func_196c4 with a mask of 0x7FFFFFFF -- every slot -- so
 * this is the "free everything" form of the buffer release below.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void Func_196c4();

void Func_1964c(int a0, int a1) { Func_196c4(a0, a1, 2147483647); }
