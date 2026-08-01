/* Func_a1804 -- ClearMenuEntries
 *
 * Takes no arguments. _Func_1ed40(0, r1, 0) -- populates menu 0, which is
 * empty, and so wipes whatever entries were showing.
 *
 * STATUS: MATCHING.
 *
 * The callee is declared void because the ROM epilogue is `pop {r0}; bx r0`,
 * which would destroy a return value in r0.
 */
extern void _Func_1ed40();

/* NOTE the unused first parameter.  The ROM leaves r1 untouched and passes it
 * straight through, so the value it forwards is the caller's SECOND argument.
 * Declaring only one parameter puts that value in r0 and agbcc emits an extra
 * `adds r1, r0, #0` to shuffle it into place. */
void Func_a1804(int unused, int menuId) { _Func_1ed40(0, menuId, 0); }
