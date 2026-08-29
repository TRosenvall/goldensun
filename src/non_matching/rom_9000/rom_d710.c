/* ActorCmd_Loop @ 0x0800d710 -- asm/rom_9000/rom_d654_a_c_a_a_a_c.s
 *
 * Source asm: goldensun/asm/rom_9000/rom_d654_a_c_a_a_a_c.s
 *
 * Blocker class 2 with an extra wrinkle. 36 of 37 instructions, diverging at
 * the counter update:
 *
 *     rom    mov r0, r5 / add r0, #0x5d / ldrb r2, [r0] / add r2, #1 / strb r2, [r0]
 *            then reuses r2 for the comparison
 *     ours   the same, but RE-READS the byte for the comparison
 *
 * The re-read comes from writing the compare against the field rather than
 * against the incremented value. Caching it removes the re-read but then
 * inverts the branch polarity, because the ROM has the jump path as the
 * fall-through and gcc puts the `if` body there (see docs/elevation.md).
 * Expressing it as a short-circuit `||` so the jump becomes the `if` body
 * gets to 36 instructions but shifts the divergence rather than closing it.
 *
 * Both halves are individually solvable; getting them at the same time is
 * what is open.
 */
#include "actor.h"

extern u16 Actor_FindScriptMarker(Actor *actor, void *label);

/* Two operands: an iteration count and a target label.
 *
 * A count of 0xFFFF loops forever and always jumps. Otherwise the counter at
 * +0x5D is bumped and compared against the count; still lower and control
 * jumps back to the label, otherwise the counter resets and the cursor steps
 * past all three words. Always returns 1.
 */
s32 ActorCmd_Loop(Actor *actor)
{
    s32 *operand = (s32 *)((u8 *)actor->script + (s16)actor->scriptPos * 4 + 4);
    u32 count = *operand++;
    void *label = (void *)*operand;

    if (count != 0xffff) {
        u8 *counter = &actor->scriptLoop;

        *counter = *counter + 1;
        if (*counter >= (s16)count) {
            *counter = 0;
            actor->scriptPos += 3;
            return 1;
        }
    }
    actor->scriptPos = Actor_FindScriptMarker(actor, label);
    return 1;
}
