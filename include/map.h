#ifndef GUARD_MAP_H
#define GUARD_MAP_H

#include "types.h"

/* Map state and the structures the overlays index into it.
 *
 * `iwram_1e70` is a POINTER to the map state, the same indirection Scene uses.
 * The block is allocated under tag 8 -- one annotation calls it 0x194 bytes,
 * but the world-map path reads as far as +0x338, so either the world map uses a
 * larger block or the 0x194 figure is wrong.  NOT resolved, so no struct here
 * either; the offsets below are what the annotations establish.
 */

extern void *iwram_1e70;

#define MAP_TRANSITION_X     0x004   /* Func_12330 writes these three, skipping */
#define MAP_TRANSITION_Y     0x008   /* any argument that is negative -- which  */
#define MAP_TRANSITION_Z     0x00C   /* is how a caller updates one axis only   */
#define MAP_COLOUR_DEPTH     0x016   /* flag: block geometry depends on it      */
#define MAP_FLAG_17          0x017   /* one-byte map-wide flag a pair of overlay
                                        triggers toggles                        */
#define MAP_TILE_ANIM_PAUSE  0x0FC   /* suspends tile animation                 */
#define MAP_BOUNDS_MIN_X     0x0EC   /* the four bounds every edge test reads   */
#define MAP_BOUNDS_MIN_Z     0x0F0
#define MAP_BOUNDS_MAX_X     0x0F4
#define MAP_BOUNDS_MAX_Z     0x0F8
#define MAP_CAMERA           0x104   /* camera record; +0x02 is the scroll and
                                        +0x06 the horizon, both halfwords      */
#define MAP_LAYERS           0x130   /* four layer descriptors, 0x30 apart:
                                        base + 0x130 + (n & 3) * 0x30          */
#define MAP_WORLD_QUADRANTS  0x138   /* world map only: a 16x16 quadrant grid   */
#define MAP_WORLD_CACHE      0x338   /* world map only -- see the size caveat   */

#define MAP_LAYER_STRIDE     0x30

/* --- the cell array ----------------------------------------------------- */
/* Rows are 0x200 bytes apart and cells 4 bytes, so a cell is
 *     layer_base + z * 0x200 + x * 4
 * Overlays stamp collision by writing ONE BYTE at +0x02 of the cell: 0xFF to
 * make it solid, 0 to clear it.  The metatile index in the low 12 bits is left
 * alone, which is why a pushable log can occupy a tile without changing how it
 * looks.  Func_10704 copies the top 20 attribute bits and leaves the index:
 *     dst = (dst & 0x00000FFF) | (src & 0xFFFFF000)
 */
#define MAP_ROW_STRIDE       0x200
#define MAP_CELL_STRIDE      0x004
#define MAP_CELL_ATTR        0x002   /* the byte overlays stamp                */
#define MAP_CELL_INDEX_MASK  0x00000FFF

/* --- map object records ------------------------------------------------- */
/* The tables slot 4 returns.  0x18 bytes each; Func_8b868 tags the ones whose
 * position falls inside the active bounds, and rom_7a0010 resets a whole array
 * of them in place rather than storing a variant per story state. */
typedef struct MapObject
{
    /* 0x00 */ u16 sprite;
    /* 0x02 */ u16 tag;             /* Func_8b868 tags records where this is 0 */
    /* 0x04 */ s32 flags;
    /* 0x08 */ s32 x;               /* 16.16                                   */
    /* 0x0C */ s32 y;
    /* 0x10 */ s32 z;
    /* 0x14 */ u8 unk_14[0x02];
    /* 0x16 */ u8 kind;
    /* 0x17 */ u8 unk_17;
} MapObject;

/* --- region records ----------------------------------------------------- */
/* 0x0C bytes, terminated by -1 in the first halfword.  The flag at +0x06
 * chooses WHICH AXIS gets a three-unit extension, so one record shape describes
 * both a wide region and a tall one -- see OvlFunc_b8c in rom_7a37f0. */
typedef struct MapRegion
{
    /* 0x00 */ s16 id;              /* -1 terminates the array                 */
    /* 0x02 */ s16 x;
    /* 0x04 */ s16 z;
    /* 0x06 */ s16 axis;            /* 0 extends x by 3, non-zero extends z    */
    /* 0x08 */ u8 unk_08[0x04];
} MapRegion;

#endif /* GUARD_MAP_H */
