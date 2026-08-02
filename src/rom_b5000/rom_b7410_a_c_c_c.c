/* Battle teardown: release every combatant's sprite.
 *
 * Whole-file conversion of asm/rom_b5000/rom_b7410_a_c_c_c.s -- one function,
 * so the ROM layout is preserved without splitting the translation unit.
 */
#include "gba/types.h"

struct BattleActor {
    void *sprite;
    u8 pad_04[0x24];
    s16 unk_28;   /* zero means the slot holds no live sprite */
};

extern struct BattleActor *GetBattleActor(s32 id);
extern void _DeleteActor(void *sprite);

/* Walks all fourteen battle slots and frees whatever sprite each is holding.
 *
 * Slots 0..7 are addressed by their own index; 8..13 are addressed at +0x78,
 * which is the enemy id base. The two ranges are one loop in the ROM rather
 * than two, so they are one loop here.
 *
 * The declared return type is deliberate: the ROM pops its return address into
 * r1 rather than r0, which is what gcc-2.96 does only when r0 is reserved for
 * a return value. Nothing is actually returned.
 *
 * (Our annotation for this address described it as taking a combatant id and a
 * position. It takes no arguments -- that entry was a shape guess and is
 * wrong.)
 */
s32 Func_80b7e7c(void)
{
    s32 i;

    for (i = 0; i <= 0xd; i++) {
        s32 id = i + 0x78;
        struct BattleActor *actor;

        if (i <= 7)
            id = i;

        actor = GetBattleActor(id);
        if (actor != NULL && actor->unk_28 != 0) {
            _DeleteActor(actor->sprite);
            actor->sprite = NULL;
            actor->unk_28 = 0;
        }
    }
}
