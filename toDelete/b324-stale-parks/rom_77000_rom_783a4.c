/* ModifyHP @ 0x080783a4, ModifyPP @ 0x080783dc
 *   -- asm/rom_77000/rom_77320_c_c.s
 *
 * Blocker class 2, REGISTER BIRTH ORDER (see docs/elevation.md). Both
 * functions, identically: every instruction is right, r2 and r3 are swapped
 * from the two field loads onward.
 *
 *     rom    ldrsh r3, [r6, r1]   (hp)   ... ldrsh r2, [r6, r1]   (max)
 *     ours   ldrsh r2, [r6, r1]   (hp)   ... ldrsh r3, [r6, r1]   (max)
 *
 * The load ORDER is already right -- hp first, then max, matching the ROM.
 * Only which register each lands in differs, so this is birth order and not
 * anything about the C.
 *
 * Tried: both fields hoisted into locals before the add; the add folded into
 * the initialiser; the clamp as an if/else-if chain and as a nested ternary;
 * max read directly in the comparison instead of cached. The if/else-if and
 * ternary forms produce the same 25 instructions, the others are longer.
 */
#include "gba/types.h"

struct Unit {
    u8 pad_00[0x34];
    s16 maxHp;
    s16 maxPp;
    s16 hp;
    s16 pp;
};

extern struct Unit *GetUnit(s32 unitId);
extern void UpdateStatBarPercent(s32 unitId);

/* The damage and healing entry point -- rom_b5000 calls it with a negative
 * delta. Clamps to 0..maxHp and returns the new value. Reaching 0 is how a
 * combatant goes down.
 */
s32 ModifyHP(s32 unitId, s32 delta)
{
    struct Unit *unit = GetUnit(unitId);
    s32 hp = unit->hp;
    s32 maxHp = unit->maxHp;
    s32 clamped;

    hp += delta;
    if (hp > maxHp)
        clamped = maxHp;
    else if (hp < 0)
        clamped = 0;
    else
        clamped = hp;

    unit->hp = clamped;
    UpdateStatBarPercent(unitId);
    return unit->hp;
}

/* The PP counterpart: same clamp, same refresh. */
s32 ModifyPP(s32 unitId, s32 delta)
{
    struct Unit *unit = GetUnit(unitId);
    s32 pp = unit->pp;
    s32 maxPp = unit->maxPp;
    s32 clamped;

    pp += delta;
    if (pp > maxPp)
        clamped = maxPp;
    else if (pp < 0)
        clamped = 0;
    else
        clamped = pp;

    unit->pp = clamped;
    UpdateStatBarPercent(unitId);
    return unit->pp;
}
