/* MapActor_SetPos     --  0x080923e4
 * MapActor_SetPos3D   --  0x08092454
 *
 * The whole of goldensun/asm/rom_8a000/rom_91584_c_c_a_c_c_c_c_c_a_a_c.s. NO
 * SPLIT: the second function matched from the same spelling, so the translation
 * unit converts entire. Byte-verified at 240 bytes with a clean compare.
 *
 * Place an actor, clear its motion and move targets, and re-seat it on the floor
 * if it follows terrain. The 3D form additionally takes an explicit height.
 *
 * SIGNED DIVISION BY A POWER OF TWO IS THE DISCRIMINATOR, NOT A SHIFT. The
 * `cmp rN,#0 / bge / add bias / asr` group is gcc-2.96's signed `/ 0x10000`; a
 * written `>> 16` collapses to a bare arithmetic shift and loses twelve
 * instructions. A BIAS-ADD OF (1<<n)-1 UNDER A `bge` MEANS THE SOURCE WROTE A
 * DIVIDE, NEVER A SHIFT -- one of the cheapest reads there is off a reference.
 *
 * THE and-SUBREG LEVER IS BIDIRECTIONAL AND READABLE OFF THE ASSEMBLY. The
 * recorded form says an int-width local fixes a mask landing in the wrong
 * register. Here the ROM IS the subreg form, mask in the destination, so the fix
 * is to NOT apply the lever: the direct byte-field test is right, and hoisting
 * into an int local flips exactly two registers. Mask-in-destination means a
 * direct field read; value-in-destination means an int local. Operand order
 * around the `&` is inert either way.
 *
 * STRUCT STORE ORDER SURVIVES VERBATIM, INCLUDING A WRONG-LOOKING ONE. The ROM
 * stores the higher offset BEFORE the lower one, and writing them in the natural
 * order costs 2. That is source order, not scheduling noise -- write the ROM's
 * order even when it reads backwards.
 *
 * And `a - b + c` against `a += c - b` is a four-line difference: left-to-right
 * source grouping is preserved into RTL, and reassociating moves the load.
 *
 * Only three things here are load-bearing. The declaration shape, the mask's
 * operand order and a cast at the call site are all measured inert.
 *
 * A SCREEN ARTEFACT WORTH KNOWING: tryc.py reports a pool-entry mismatch on this
 * file, and it is wrong. Its check counts our pool words across the WHOLE
 * compiler output but counts the reference's DISTINCT values, so two functions
 * in one TU that pool the same value read as a surplus. They are not shareable:
 * the macro behind `.func_end` dumps a separate pool per function, and the ROM
 * has both words. Screening each function alone makes the warning vanish, and
 * the byte compare settles it.
 *
 * The neighbour finder gave the entire top-of-file unguessed -- the callee
 * declarations with their exact prefixes and void signature, the guard shape and
 * the house include. The caller-grep confirmed arity and argument order across
 * about forty overlay sites but did NOT reveal the second function; only reading
 * the .s did.
 */
#include "actor.h"

extern Actor *GetFieldActor(s32 slot);
extern void _Actor_Stop(void);
extern int _Func_8011f54(int a, int b, int c);

void MapActor_SetPos(s32 slot, fx32 x, fx32 z)
{
    Actor *actor = GetFieldActor(slot);

    if (actor != NULL) {
        _Actor_Stop();
        actor->motion.x = 0;
        actor->motion.y = 0;
        actor->motion.z = 0;
        actor->targetY = ACTOR_NO_TARGET;
        actor->targetX = ACTOR_NO_TARGET;
        actor->pos.x = x;
        actor->pos.z = z;
        if (actor->interactFlag & 1) {
            fx32 h = _Func_8011f54(actor->layer, x / 0x10000, z / 0x10000) << 16;
            actor->pos.y = actor->pos.y - actor->floorPos + h;
            actor->floorPos = h;
        }
    }
}

void MapActor_SetPos3D(s32 slot, fx32 x, fx32 y, fx32 z)
{
    Actor *actor = GetFieldActor(slot);

    if (actor != NULL) {
        _Actor_Stop();
        actor->motion.x = 0;
        actor->motion.y = 0;
        actor->motion.z = 0;
        actor->targetY = ACTOR_NO_TARGET;
        actor->targetX = ACTOR_NO_TARGET;
        actor->pos.x = x;
        actor->pos.y = y;
        actor->pos.z = z;
        if (actor->interactFlag & 1) {
            fx32 h = _Func_8011f54(actor->layer, x / 0x10000, z / 0x10000) << 16;
            actor->pos.y = actor->pos.y - actor->floorPos + h;
            actor->floorPos = h;
        }
    }
}
