#ifndef GUARD_SAVE_H
#define GUARD_SAVE_H

#include "types.h"

/* Party and save state.
 *
 * `ewram_240` is the block ITSELF, not a pointer to it -- unlike iwram_1ebc and
 * iwram_1e70.  Accesses look like `ldr r3, =ewram_240` followed straight by an
 * offset, with no intervening load.  Worth knowing, because mixing the two
 * conventions up produces code that assembles and then reads garbage.
 */

extern u8 ewram_240[];

#define SAVE_COINS           0x010   /* clamped at 0 on spend and 0x30D40 on
                                        gain, so it cannot go negative or past
                                        999,999                              */
#define SAVE_UNK_1B4         0x1B4

/* --- where the player is ------------------------------------------------ */
/* Slots 1 and 3 of every overlay branch on these two, and area 0x10-style maps
 * subdivide again on the entrance.  Getting them backwards is easy: the AREA
 * comes first. */
#define SAVE_AREA_ID         0x1C0   /* halfword: which map                   */
#define SAVE_ENTRANCE_ID     0x1C2   /* halfword: which door into it          */
#define SAVE_PENDING_AREA    0x1C4   /* staged destination for the next
                                        transition                            */
#define SAVE_PENDING_ENTRANCE 0x1C6

/* --- the party ---------------------------------------------------------- */
#define SAVE_LEADER_SLOT     0x1F4   /* the entity slot the player controls    */
#define SAVE_ROSTER          0x1F8   /* roster bytes; Func_796c4 widens them
                                        into halfwords for callers            */

/* --- route memory ------------------------------------------------------- */
/* rom_7ec968 records which of two ways the player came through and reads it
 * back on the way out, so the exit destination depends on the entrance. */
#define SAVE_ROUTE_AREA      0x240
#define SAVE_ROUTE_MODE      0x242

/* --- save bits ---------------------------------------------------------- */
/* Not a field but the dominant idiom in the overlays.  Func_79338 tests,
 * Func_79358 sets, Func_79374 clears; the index is (idx & 0xFFF) >> 3 for the
 * byte and idx & 7 for the bit.
 *
 * Three usage patterns worth recognising, all seen in the overlays:
 *   - a plain one-shot marker: set once, tested forever after,
 *   - a MATCHED PAIR recording which of two things happened, cleared together
 *     to reset the choice (rom_7ec968's 0x8FB / 0x8FC),
 *   - a per-object bit that is CLEARED as well as set, so a wrong move undoes
 *     cleanly, with a third bit tracking the resulting state so a transition
 *     fires once per change rather than every frame (rom_7a6ae4's puzzle).
 */

#endif /* GUARD_SAVE_H */
