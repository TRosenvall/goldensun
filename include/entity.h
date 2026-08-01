#ifndef GUARD_ENTITY_H
#define GUARD_ENTITY_H

#include "types.h"

/* The overworld entity.
 *
 * rom_9000 owns these: Func_c150 creates one, Func_c2d8 points its script VM at
 * a script, and the frame loop walks the table calling each entity's update
 * hook.  rom_8a000's cutscene layer drives them by slot, and the map overlays
 * spawn props as entities too.  See docs/ -- the overworld writeup covers the
 * lifecycle.
 *
 * ONLY THE NAMED FIELDS ARE ESTABLISHED.  Everything else is `unk_XX` padding
 * on purpose: this struct is meant to grow as functions are converted, and a
 * guessed field name is worse than an honest hole.  Two specific cautions:
 *
 *   +0x30 / +0x34  read two different ways in the annotations -- as an angular
 *                  velocity pair (rom_78ef88's prop physics) and as max speed /
 *                  acceleration (rom_8a000's Func_92064).  Both may be true for
 *                  different entity kinds, or one reading may be wrong.  Left
 *                  unnamed until something settles it.
 *   +0x38 / +0x40  park at 0x80000000, which the engine uses as a "no target"
 *                  sentinel, but their meaning is not pinned down.
 */

typedef struct Entity Entity;

/* Installed at +0x6C and called once per frame as hook(entity).  Both call
 * sites -- rom_92b8.s and rom_ca6c.s -- ignore the return value, so the
 * effective signature is void, even though some hooks do return one. */
typedef void (*EntityHook)(Entity *entity);

struct Entity
{
    /* 0x00 */ void *script;        /* script VM program, or NULL     */
    /* 0x04 */ u16 scriptCursor;    /* byte offset into it            */
    /* 0x06 */ u16 unk_06;
    /* 0x08 */ s32 x;               /* world position, 16.16          */
    /* 0x0C */ s32 y;
    /* 0x10 */ s32 z;
    /* 0x14 */ u8 unk_14[0x04];
    /* 0x18 */ s32 rotX;            /* orientation, 16.16             */
    /* 0x1C */ s32 rotY;
    /* 0x20 */ u8 unk_20[0x24];     /* includes the +0x30/+0x34 pair  */
    /* 0x44 */ s32 velX;            /* added to the position each frame
                                       by the prop physics hook       */
    /* 0x48 */ s32 velY;
    /* 0x4C */ s32 velZ;
    /* 0x50 */ void *actor;         /* the sprite/actor, rom_9000     */
    /* 0x54 */ u8 drawKind;         /* 0 skip, 1 the single actor at
                                       +0x50, 2 the four-actor array;
                                       Func_c880 dispatches on it      */
    /* 0x55 */ u8 unk_55[0x17];
    /* 0x6C */ EntityHook updateHook;
};

#endif /* GUARD_ENTITY_H */
