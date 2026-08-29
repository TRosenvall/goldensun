/* Cluster Actor_IsNotMoving..Actor_IsNotMoving extracted from goldensun/asm/rom_9000/rom_ca6c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_9000/rom_ca6c_a_c_a.o and asm/rom_9000/rom_ca6c_a_c_c.o in
 * goldensun/stage1.ld.
 *
 * True when the actor has no move in progress. Which axes count depends on the
 * flag at 0x55: with it clear all three targets must be idle, with it set only
 * X and Z are checked.
 *
 * TWO LEVERS, AND THE FIRST ONE IS THE INTERESTING ONE.
 *
 * 1. THE SHARED COMPARE TAKES A DIFFERENT OPERAND ON EACH PATH. The ROM joins
 *    the two arms at a single `cmp r2, r3`, and r3 does not hold the same thing
 *    coming in:
 *
 *        arm A (flag clear)  r3 = a->targetX   -- already proved == NO_TARGET
 *        arm B (flag set)    r3 = NO_TARGET    -- materialised in the arm
 *
 *    Writing the join as `if (t != ACTOR_NO_TARGET)` makes gcc materialise the
 *    constant ONCE after the join, which is one instruction short and puts the
 *    `mov #0x80 / lsl #0x18` on the wrong side of the label. Carrying BOTH
 *    sides of the comparison in their own variables -- `lhs` and `rhs`, with
 *    `rhs = a->targetX` in one arm and the constant in the other -- reproduces
 *    it. gcc-2.96 knows targetX equals NO_TARGET there and reuses the register
 *    rather than rebuilding the constant, exactly as the ROM does.
 *
 *    That is worth remembering as a shape: when a ROM's join block compares
 *    against a register whose contents differ per predecessor, the source named
 *    the operand rather than repeating the constant.
 *
 * 2. BRANCH POLARITY, the rule already in docs/elevation.md. Written as two
 *    early returns the arms come out inverted -- `beq` to the success block
 *    with `mov r0, #0` falling through. The ROM falls through into SUCCESS:
 *
 *        cmp r2, r3 / bne .L1 / ldr r3, [r0, #0x40] / mov r0, #1 / cmp r3, r2
 *        beq .L3 / .L1: mov r0, #0
 *
 *    so the success case is the `if` body and the `return 0` is the tail:
 *
 *        if (lhs == rhs) { if (a->targetZ == lhs) return 1; }
 *        return 0;
 *
 * Note the final compare is against `lhs`, not against the constant -- same
 * point as (1), and it is why `rhs` can be dropped from that test.
 */
#include "gba/types.h"
#include "actor.h"

s32 Actor_IsNotMoving(Actor *a)
{
    fx32 lhs;
    fx32 rhs;

    if (a->interactFlag == 0) {
        if (a->targetX != ACTOR_NO_TARGET)
            return 0;
        lhs = a->targetY;
        rhs = a->targetX;
    } else {
        lhs = a->targetX;
        rhs = ACTOR_NO_TARGET;
    }
    if (lhs == rhs) {
        if (a->targetZ == lhs)
            return 1;
    }
    return 0;
}
