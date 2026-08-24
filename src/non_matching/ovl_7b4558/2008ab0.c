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
 *
 * RE-ATTEMPTED, batch 41, from tools/rank_parks.py. Still 2 of 27.
 *
 * THIS IS A TWIN PAIR AND THE DIFF IS IDENTICAL in both members --
 * src/non_matching/ovl_7b4558/2008ab0.c (OvlFunc_927_2008ab0) and
 * src/non_matching/rom_8a000/rom_9a44c.c (Func_809a44c), one an overlay copy of
 * the other. Solving either solves both, which is the reason to spend on it.
 *
 *     rom    ... last add-assign ... / ldr r1,[r0,#0x50] / add r0,#0x64
 *     ours   ... ldr r1,[r0,#0x50] hoisted TWO INSTRUCTIONS EARLIER, into the
 *            middle of the last add-assign group ...
 *
 * gcc schedules the sprite load above a store, which means it proved the two
 * cannot alias. Everything tried was aimed at that:
 *
 *   the goalFacing read as an explicit pointer walk rather than a field   2
 *   the sprite read as *(struct Spr **)((unsigned char *)a + 0x50)        2
 *   the same with the address in its own local first                      2
 *   -fno-schedule-insns, -fno-gcse, -fno-strength-reduce, --no-rerun-cse  2
 *   -fno-schedule-insns2                                                  8 (worse)
 *   -O1                                                                   8 (worse)
 *
 * Note that the two flags which DO move it move it the wrong way, which says
 * the post-reload scheduler is not what places this load -- it is placed
 * earlier and the scheduler is what puts it back. A next attempt should look at
 * why gcc believes the load cannot alias the stores, rather than at scheduling.
 * The compiler source is in the build image; see docs/elevation.md.
 *
 * CORRECTED, batch 41. Last round's note said gcc "proved the two cannot alias"
 * and suggested reading the alias code. That was the wrong diagnosis, and the
 * full listing shows why -- look at WHERE the load lands, not just that it moved:
 *
 *     rom    ldr r3,[r0,#0x1c] / add r3,r2 / str r3,[r0,#0x1c] / ldr r1,[r0,#0x50]
 *     ours   ldr r3,[r0,#0x1c] / ldr r1,[r0,#0x50] / add r3,r2 / str r3,[r0,#0x1c]
 *
 * gcc drops the sprite load into the slot between a load and its use. That is
 * the POST-RELOAD SCHEDULER filling a load-use stall on ARM7TDMI, not an
 * aliasing decision -- and the ROM leaves the slot empty.
 *
 * Which also explains the flag result that looked backwards: -fno-schedule-insns2
 * DOES fix this instruction, and goes to 8 because turning the scheduler off
 * moves seven others. So the function needs the scheduler ON everywhere except
 * this one slot, which no flag expresses.
 *
 * Tried, on the theory that giving the scheduler a cheaper instruction for the
 * slot would leave the load alone:
 *
 *   the goalFacing pointer walk hoisted above the sprite load      7 (worse)
 *   the same with the halfword read in its own local               5 (worse)
 *   the add-assign split into a read, an add and a store           2 (no change)
 *
 * Every arrangement that gives the scheduler something else to move makes it
 * move that instead, and it is always wrong. What would settle this is the
 * scheduler's cost model -- haifa-sched.c and the arm machine description are
 * both in the build image -- specifically whether the ROM's build had
 * -fno-schedule-insns2 for this translation unit only. That is checkable
 * against the other functions in the same .s: if they need the scheduler ON,
 * the flag is not the answer and this is a genuine compiler difference.
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
