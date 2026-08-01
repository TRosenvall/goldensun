/* Func_fa484 -- ResumeCurrentPlayer
 *
 * r0 = player. A thin wrapper on Func_fa264.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void Func_fa264();

void Func_fa484(void) { Func_fa264(); }
