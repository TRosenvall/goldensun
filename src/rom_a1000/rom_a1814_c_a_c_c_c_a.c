/* Func_80a3480 @ 0x080a3480  --  HideEveryFifthSprite
 *
 * Marks every fifth slot of the 32-entry sprite table at +0x48 -- the row
 * headings in the five-wide layouts.  Null slots are skipped without consuming
 * an index, so the position follows the SLOT index, not the occupied count.
 *
 * THE PREHEADER ORDER SAYS WHICH PASS BUILT THE POINTER.
 *
 * The park (blocker class 2, "register birth order") had all 28 instructions
 * and an identical loop body, and lost 5 of 30 on the order of the preheader:
 *
 *   rom    mov r2,#0xd / mov r7,r3 / mov r6,#0 / mov r8,r2 / add r7,#0x48
 *   ours   mov r7,r3 / mov r3,#0xd / add r7,#0x48 / mov r6,#0 / mov r8,r3
 *
 * `mov r8, r2` is the hoisted loop invariant, and this file's rule "a
 * loop-invariant hoisted by loop.c is ALWAYS the last insn of the preheader"
 * says it cannot be followed by a source-level statement.  In the ROM it IS
 * followed by one -- `add r7, #0x48`.  So that add is not a statement at all:
 * it is strength_reduce's giv initialiser, emitted by `emit_iv_add_mult`
 * AFTER `move_movables` has run.
 *
 * The park wrote the walk as `*slot++`, which makes the cursor a user variable
 * whose init is a preheader STATEMENT -- necessarily before the hoisted 0xd.
 * Writing the walk as `slot[i]` instead leaves only the index biv in the
 * source; strength_reduce creates the cursor itself, places its init last, and
 * flow's auto-inc pass still folds the update into `ldmia r7!, {r5}`.  Exact,
 * with no pins and no flag rule.
 *
 *   Read the preheader as a pass boundary: anything AFTER the hoisted
 *   invariant has to be created by a later pass, so stop spelling the
 *   statement and let strength reduction build it.
 *
 * `base + 0x48` written out inside the loop body instead of in the initialiser
 * is equally exact; dereferencing the global directly in the body is 4 of 30.
 */
#include "gba/types.h"

struct Sprite {
    u8 pad_00[5];
    u8 flags;
};

extern u8 *iwram_3001f2c;

void Func_80a3480(void)
{
    struct Sprite **slot = (struct Sprite **)(iwram_3001f2c + 0x48);
    s32 i;

    for (i = 0; i <= 0x1f; i++) {
        struct Sprite *sprite = slot[i];

        if (sprite != NULL && i % 5 == 0)
            sprite->flags = 0xd;
    }
}
