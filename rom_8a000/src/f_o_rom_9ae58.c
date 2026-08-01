/* Func_9ae58 -- RunFieldAbility_Wrapper
 *
 * Tail call to Func_9ae64; a separate entry point so the ability table can
 * reference it by its own address.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void Func_9ae64();

void Func_9ae58(void) { Func_9ae64(); }
