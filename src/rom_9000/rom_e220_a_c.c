/* Actor attribute opcode: collision radius.
 *
 * Whole-file conversion of asm/rom_9000/rom_e220_a_c.s -- one function, so the
 * ROM layout is preserved without splitting the translation unit.
 */
#include "actor.h"

/* One of the attribute opcodes the script VM dispatches on. op selects what to
 * do with the operand:
 *
 *     0  set the radius
 *     1  add to it
 *     _  test it, leaving the result in the +0x57 flag byte
 *
 * The test is the interesting one: the stored radius is read UNSIGNED and the
 * operand is narrowed to a SIGNED halfword before the comparison, so a
 * negative operand can never equal a stored radius. That asymmetry is in the
 * ROM, not a decompilation artifact -- it is what the `ldrh` / `asr` pair
 * means.
 */
void ActorAttrOp_width(Actor *actor, s32 op, s32 value)
{
    if (op == 0)
        actor->width = value;
    else if (op == 1)
        actor->width += value;
    else
        actor->scriptVar = (actor->width == (s16)value);
}
