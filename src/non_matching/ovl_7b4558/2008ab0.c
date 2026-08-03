/* OvlFunc_927_2008ab0  [ovl_7b4558]  --  0x02008ab0
 *
 * Source asm: goldensun/asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c.s
 *
 * HEAD OF A 6-MEMBER FAMILY, one copy per overlay plus one in
 * overlays/common/common0.s.
 *
 * A per-frame integration step, and the reading is not in doubt: position
 * gains velocity on all three axes, the 0x18/0x1C pair gains speed and accel,
 * and the sprite's own angle at +0x1E gains goalFacing. Twenty-seven
 * instructions against twenty-seven, and twenty-six of them are identical.
 *
 * Blocker: ONE HOIST. gcc loads the sprite pointer one instruction earlier
 * than the ROM does:
 *
 *     rom    add r3,r2 / str r3,[r0,#0x1c] / ldr r1,[r0,#0x50]
 *     ours   ldr r1,[r0,#0x50] / add r3,r2 / str r3,[r0,#0x1c]
 *
 * The load is independent of the rotY update, so gcc is free to move it and
 * does. Everything before and after matches exactly.
 *
 * TRIED, all still 27-vs-27 diverging at instruction 18 unless noted:
 *
 *   1. the pointer arithmetic written inline in the final statement
 *   2. the sprite address taken into a named local first (28 instructions --
 *      the extra name costs an instruction, the same way naming the two stack
 *      values does in the stack-arg-pair class)
 *   3. the sprite reached through a typed struct pointer
 *   4. goalFacing read into a local before the add
 *   5. every update written as `x = x + y` rather than `x += y`
 *   6. the rotX/rotY pair wrapped in a nested block
 *
 * So the named-intermediate lever does not help here, and neither does
 * statement grouping. What would is something that makes the sprite load
 * DEPEND on the rotY store -- and nothing in the source does, because in the
 * ROM it does not either. This may be a scheduling difference with no C
 * expression, which is the class docs/elevation.md calls "Scheduling" and
 * records as unsolved.
 *
 * Worth retrying if a scheduling lever is ever found: this is six functions.
 */
#include "actor.h"

void OvlFunc_927_2008ab0(Actor *a)
{
    struct Spr { unsigned char pad_00[0x1e]; unsigned short ang; } *s;

    a->pos.x += a->velX;
    a->pos.y += a->velY;
    a->pos.z += a->velZ;
    a->rotX += a->speed;
    a->rotY += a->accel;
    s = (struct Spr *)a->sprite;
    s->ang += a->goalFacing;
}
