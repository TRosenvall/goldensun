/* Func_fa318 -- SoundVBlank
 *
 * Takes no arguments. Func_f95f0 -- the VBlank half of the mixer. Exported, and
 * the only rom_f9000 export whose name does not appear in a `bl` anywhere; it
 * reaches the frame loop through the export veneer.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void Func_f95f0();

void Func_fa318(void) { Func_f95f0(); }
