/* Func_800c548 @ 0x0800c548, Func_800c570 @ 0x0800c570
 *   -- asm/rom_9000/rom_c004_c_a_c_a.s
 *
 * Blocker classes 1 and 2 at once (see docs/elevation.md).
 *
 *   1. the ROM masks in 32-bit width, gcc narrows to the byte:
 *
 *          rom    mov r3, #0xd / neg r3, r3     (~0xc as 0xfffffff3)
 *          ours   mov r3, #0xf3                 (~0xc as a byte)
 *
 *      gcc narrows through an (s32) cast on the load and through a named s32
 *      temporary, because it can see the result reaches a strb.
 *
 *   2. the ROM keeps the actor pointer in r0, reusing the actor register;
 *      gcc puts it in r1 and has to spill the parameter into r4 first.
 *
 * The two travel together: freeing r0 is what lets the parameter stay in r1,
 * so fixing the mask width alone will probably not be enough.
 */
#include "actor.h"

struct DrawActor {
    u8 pad_00[0x05];
    u8 oamFlags;    /* bits 0-1 affine mode, bits 2-3 OAM priority */
    u8 pad_06[0x17];
    u8 scaleFlags;  /* bit 1 selects caller-supplied scale over depth-derived */
};

/* Replaces the OAM priority field, leaving the affine mode alone. Requires an
 * exact draw kind of 1, not just the low nibble.
 */
void Func_800c548(Actor *actor, s32 priority)
{
    if (actor != NULL && actor->drawKind == 1) {
        struct DrawActor *sprite = (struct DrawActor *)actor->sprite;

        sprite->oamFlags = (sprite->oamFlags & ~0xc) | ((priority & 3) << 2);
    }
}

/* Replaces the flag Func_b388 tests to decide whether to compute the
 * perspective scale from the depth or take the caller's value.
 */
void Func_800c570(Actor *actor, s32 useCallerScale)
{
    if (actor != NULL && actor->drawKind == 1) {
        struct DrawActor *sprite = (struct DrawActor *)actor->sprite;

        sprite->scaleFlags = (sprite->scaleFlags & ~2) | ((useCallerScale & 1) << 1);
    }
}
