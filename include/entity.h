#ifndef GUARD_ENTITY_H
#define GUARD_ENTITY_H

#include "types.h"

/* The overworld entity -- 0x70 bytes.
 *
 * rom_9000 owns these: Func_c150 creates one, Func_c2d8 points its script VM at
 * a script, and Func_c880 walks the 0x40-slot table at [iwram_1e64]+0x1B90 once
 * per frame to draw them.  rom_8a000's cutscene layer drives them by slot, and
 * the map overlays spawn props as entities too.
 *
 * The 0x70 stride is confirmed by that draw loop, which steps backwards through
 * the table so higher slots draw first.
 *
 * ONLY THE NAMED FIELDS ARE ESTABLISHED.  Everything else is `unk_XX` on
 * purpose: this struct grows as functions are converted, and a guessed field
 * name is worse than an honest hole.  Three cautions:
 *
 *   +0x30 / +0x34  read two ways in the annotations -- as max speed and
 *                  acceleration (Func_92064, and Func_d14c's braking
 *                  calculation), and as a scale pair passed to the draw call.
 *                  Both may be true for different entity kinds, or one reading
 *                  may be wrong.  Named for the movement reading because far
 *                  more call sites use it that way; do not trust it blind.
 *   +0x38 / +0x40  park at 0x80000000, the engine's "no target" sentinel.
 *                  Func_d14c writes a move target across +0x38..+0x40, so they
 *                  are named for that -- but the draw loop also reads a
 *                  priority byte at +0x42, which overlaps targetZ.  One of the
 *                  two readings is wrong and it is not yet clear which.
 *   +0x0A / +0x12  the integer halves of x and z.  NOT separate fields -- they
 *                  alias the high halfwords of the 16.16 values at +0x08 and
 *                  +0x10, and the ROM reads them both ways.
 */

typedef struct Entity Entity;

/* Installed at +0x6C and called once per frame as hook(entity).  Both call
 * sites -- rom_92b8.s and rom_ca6c.s -- ignore the return value, so the
 * effective signature is void, even though some hooks do return one. */
typedef void (*EntityHook)(Entity *entity);

/* Values for Entity.drawKind.  Func_c880 dispatches on the LOW NIBBLE. */
#define DRAW_KIND_NONE      0   /* not drawn                                 */
#define DRAW_KIND_SINGLE    1   /* one actor at +0x50, via Func_b388         */
#define DRAW_KIND_ARRAY     2   /* up to four actors from the array at +0x50  */

/* Entity.targetX/Y/Z hold this when there is no move in progress. */
#define ENTITY_NO_TARGET    0x80000000

struct Entity
{
    /* 0x00 */ void *script;        /* script VM program.  Zero means the slot
                                       is inactive and the draw loop skips it */
    /* 0x04 */ u16 scriptCursor;    /* byte offset into it                    */
    /* 0x06 */ u16 facing;          /* full-circle angle.  The TOP FOUR BITS
                                       are the 16-way direction the overlays
                                       index their step tables with           */
    /* 0x08 */ s32 x;               /* world position, 16.16                  */
    /* 0x0C */ s32 y;
    /* 0x10 */ s32 z;
    /* 0x14 */ u8 unk_14[0x04];
    /* 0x18 */ s32 rotX;            /* orientation, 16.16                     */
    /* 0x1C */ s32 rotY;
    /* 0x20 */ u8 unk_20[0x02];
    /* 0x22 */ u8 tileType;         /* picks the collision layer Func_120dc
                                       samples; above 2 falls back to
                                       ewram_10000                            */
    /* 0x23 */ u8 displayFlags;     /* bit 1 shifts the sprite by -0x140.0000 */
    /* 0x24 */ s32 velXCur;         /* cleared together when a move is
                                       cancelled mid-flight                   */
    /* 0x28 */ u8 unk_28[0x04];
    /* 0x2C */ s32 velZCur;
    /* 0x30 */ s32 maxSpeed;        /* see the caution above                  */
    /* 0x34 */ s32 accel;
    /* 0x38 */ s32 targetX;         /* ENTITY_NO_TARGET when idle             */
    /* 0x3C */ s32 targetY;
    /* 0x40 */ s32 targetZ;         /* overlaps the +0x42 priority byte       */
    /* 0x44 */ s32 velX;            /* added to the position each frame by
                                       the prop physics hook                  */
    /* 0x48 */ s32 velY;
    /* 0x4C */ s32 velZ;
    /* 0x50 */ void *actor;         /* one actor, or an array of up to four,
                                       depending on drawKind                  */
    /* 0x54 */ u8 drawKind;         /* DRAW_KIND_*, low nibble only           */
    /* 0x55 */ u8 interactFlag;     /* per-entity marker; maps clear it across
                                       every slot on entry                    */
    /* 0x56 */ u8 arrivalAxis;      /* which axis the arrival test watches:
                                       0x10 = x (default), 0x12 = z           */
    /* 0x57 */ u8 unk_57;
    /* 0x58 */ u8 exactStop;        /* non-zero suppresses Func_d14c's braking
                                       allowance                              */
    /* 0x59 */ u8 interactFlags;    /* bit 0 makes the entity block a push;
                                       overlays OR in 0x10 / 0x14 to make an
                                       NPC talkable                           */
    /* 0x5A */ u8 unk_5a[0x08];
    /* 0x62 */ u8 tickFast;         /* fast counter; rolls into tickSlow      */
    /* 0x63 */ u8 unk_63;
    /* 0x64 */ u16 goalFacing;      /* Func_92adc's turn target               */
    /* 0x66 */ u16 tickSlow;        /* bumped when tickFast passes 0x50       */
    /* 0x68 */ u8 unk_68[0x04];
    /* 0x6C */ EntityHook updateHook;
};

#endif /* GUARD_ENTITY_H */
