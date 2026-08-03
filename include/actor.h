#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "gba/types.h"

/* The overworld actor -- 0x80 bytes in GS1.
 *
 * rom_9000 owns these: it creates one, points its script VM at a script, and
 * walks the 0x40-slot table once per frame to draw them.  rom_8a000's cutscene
 * layer drives them by slot, and the map overlays spawn props as actors too.
 *
 *
 * WHY THIS LAYOUT CHANGED
 *
 * The previous version of this struct was SIXTEEN BYTES SHORT from `sprite`
 * onward: `sprite` sat at 0x40 when it belongs at 0x50, so every field after
 * it landed 0x10 below the offset its own name claimed -- `__unk5A` was at
 * 0x4A, `__unk5C` at 0x4C, `update` at 0x5C.  The names were right; the
 * offsets were not.
 *
 * Three of this tree's own matched files bypass this header and declare local
 * structs rather than use it:
 *
 *     src/overlays/rom_7f2f14/ovl_30_a_c_c_b.c
 *     src/overlays/rom_7c5efc/ovl_30_c_a_c_c_a_c_b.c
 *     src/overlays/rom_7ced6c/ovl_30_c_c_a_c_c_b.c
 *
 * All three place `update` at 0x6C, and byte-match the ROM doing so.  They
 * also fix unk55 at 0x55, unk59 at 0x59 and unk63 at 0x63.  The missing 0x10
 * bytes sit between 0x3C and 0x4C.
 *
 * Nothing was broken by the old layout, because nothing used it past 0x40.
 * It would have broken the first thing that did.
 *
 *
 * HOW MUCH OF THIS IS ESTABLISHED
 *
 * Fields carrying a real name are ones some function's behaviour pins down,
 * and the load width is part of that -- a field read with `ldrsh` is signed
 * here even where a neighbouring one is not.  Fields named unk_XX are honest
 * holes: the offset is right, the meaning is not known.
 *
 * Five offsets have two competing readings.  Both are recorded rather than
 * one being picked, because picking wrong is worse than saying so:
 *
 *   0x18/0x1C   a scale pair, or a rotation pair.  Func_809a44c adds 0x30 and
 *               0x34 into them once per frame, which fits either reading as a
 *               rate of change.  Named rot* for the movement reading; the
 *               draw path reads them as scale.
 *   0x22        collision layer, or tile type.
 *   0x30/0x34   movement tuning (max speed, acceleration), or the pair that
 *               feeds 0x18/0x1C above.  These cannot both be true.
 *   0x38        gravity, or the first word of a move target.  0x38..0x40 park
 *               at 0x80000000, which is the engine's no-target sentinel and
 *               is hard to explain as gravity.
 *   0x54        a visible flag, or a draw-kind selector.  Draw-kind wins: the
 *               draw loop dispatches on the low nibble and 2 is a valid value,
 *               which a boolean cannot be.
 */

typedef struct Actor Actor;

/* Installed at 0x6C and called once per frame as hook(actor).  Both call
 * sites ignore the return value, so the effective signature is void even
 * though some hooks do return one.
 *
 * (This was previously declared `actorfun_t *update` -- a pointer to a
 * function pointer.  The matched overlay files assign a function address
 * straight into it, so the extra indirection was wrong.)
 */
typedef void (*actorfun_t)(Actor *actor);

/* Values for Actor.drawKind.  The draw loop dispatches on the LOW NIBBLE. */
#define DRAW_KIND_NONE      0   /* not drawn                                 */
#define DRAW_KIND_SINGLE    1   /* one sprite at 0x50                        */
#define DRAW_KIND_ARRAY     2   /* up to four sprites from the array at 0x50 */

/* Actor.targetX/Y/Z hold this when there is no move in progress. */
#define ACTOR_NO_TARGET     0x80000000

struct Actor {
    /* 0x00 */ void *script;        /* script VM program.  Zero means the slot
                                       is inactive and the draw loop skips it */
    /* 0x04 */ u16 scriptPos;       /* byte offset into it.  READ SIGNED by
                                       the opcode handlers (`ldrsh`), even
                                       though it is only ever a forward offset
                                       in practice                           */
    /* 0x06 */ u16 facing;          /* full-circle angle.  The TOP FOUR BITS
                                       are the 16-way direction the overlays
                                       index their step tables with          */
    /* 0x08 */ vec3_t pos;          /* world position, 16.16.  The integer
                                       halves at 0x0A and 0x12 are the high
                                       halfwords of x and z, NOT separate
                                       fields -- the ROM reads them both ways */
    /* 0x14 */ fx32 floorPos;
    /* 0x18 */ fx32 rotX;           /* see the caution above -- also read as
                                       a scale pair                          */
    /* 0x1C */ fx32 rotY;
    /* 0x20 */ u16 width;           /* collision radius.  Read UNSIGNED and
                                       compared against a SIGNED operand, so
                                       a negative operand never matches      */
    /* 0x22 */ u8 layer;            /* picks the collision layer sampled for
                                       this actor; above 2 falls back        */
    /* 0x23 */ u8 flags;            /* bit 1 shifts the sprite by -0x140.0000 */
    /* 0x24 */ vec3_t motion;       /* per-frame motion; 0x24 and 0x2C are
                                       cleared together when a move is
                                       cancelled mid-flight                  */
    /* 0x30 */ fx32 speed;          /* see the caution above                  */
    /* 0x34 */ fx32 accel;
    /* 0x38 */ fx32 targetX;        /* ACTOR_NO_TARGET when idle              */
    /* 0x3C */ fx32 targetY;
    /* 0x40 */ fx32 targetZ;
    /* 0x44 */ fx32 velX;           /* added into pos each frame by the prop
                                       physics hook                          */
    /* 0x48 */ fx32 velY;
    /* 0x4C */ fx32 velZ;
    /* 0x50 */ void *sprite;        /* one sprite, or an array of up to four,
                                       depending on drawKind                 */
    /* 0x54 */ u8 drawKind;         /* DRAW_KIND_*, low nibble only           */
    /* 0x55 */ u8 interactFlag;     /* per-actor marker; maps clear it across
                                       every slot on entry                   */
    /* 0x56 */ u8 arrivalAxis;      /* which axis the arrival test watches:
                                       0x10 = x (default), 0x12 = z          */
    /* 0x57 */ u8 scriptVar;        /* the script VM's condition byte -- what
                                       the conditional-jump opcodes test     */
    /* 0x58 */ u8 exactStop;        /* non-zero suppresses the braking
                                       allowance                             */
    /* 0x59 */ u8 interactFlags;    /* bit 0 makes the actor block a push;
                                       overlays OR in 0x10 / 0x14 to make an
                                       NPC talkable                          */
    /* 0x5A */ u8 walkFlags;        /* bit 0 makes the actor turn to face its
                                       heading                               */
    /* 0x5B */ bool8 stop;
    /* 0x5C */ u8 unk_5C;
    /* 0x5D */ u8 scriptLoop;       /* iteration counter for the loop opcode  */
    /* 0x5E */ u16 waitTimer;
    /* 0x60 */ u8 unk_60[0x02];
    /* 0x62 */ u8 tickFast;         /* fast counter; rolls into tickSlow      */
    /* 0x63 */ u8 unk_63;
    /* 0x64 */ u16 goalFacing;      /* the gradual-turn target                */
    /* 0x66 */ u16 tickSlow;        /* bumped when tickFast passes 0x50       */
    /* 0x68 */ u32 unk_68;         /* word-written; Camera_SetTarget
                                      parks its script argument here     */
    /* 0x6C */ actorfun_t update;

/* GS1 only */

    /* 0x70 */ vec3_t unk_70;
    /* 0x7C */ u32 unk_7C;
};

#endif /* _ACTOR_H_ */
