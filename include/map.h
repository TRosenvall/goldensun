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
/* CORRECTION.  An earlier version of this header called the 0x18-byte record
 * below `MapObject` and said it was what SLOT 4 returns.  That was wrong, and
 * wrong in the way this file warns against -- the layout came from Func_8b868's
 * annotation, which is real evidence, but Func_8b868 is fed by SLOT 3.  The
 * slot number was assumed, not checked.
 *
 * Measured across the corpus, on the tables each slot actually returns:
 *     slot 3   66 of 66 blocks divisible by 0x18
 *     slot 4   59 of 60 blocks divisible by 0x0C
 * so the two record types are as below.  (The one slot-4 block that does not
 * divide is not yet explained.)
 */

/* --- slot 3 records: 0x18 bytes ----------------------------------------- */
/* The array slot 3 returns, and the one Func_8b868 tags: it sets the halfword
 * at +0x02 on every record still holding 0 whose position lies inside the
 * active bounds.  In stored data that halfword is usually already 0xFFFF. */
typedef struct MapSpawn
{
    /* 0x00 */ s16 id;              /* -1 terminates the array                 */
    /* 0x02 */ u16 tag;             /* Func_8b868 writes here                   */
    /* 0x04 */ u8 unk_04[0x04];
    /* 0x08 */ s32 x;               /* 16.16                                   */
    /* 0x0C */ s32 y;
    /* 0x10 */ s32 z;
    /* 0x14 */ u8 unk_14[0x04];
} MapSpawn;

/* --- slot 4 records: 0x0C bytes ----------------------------------------- */
/* The map object table.  The stride is measured; the FIELDS ARE NOT.  Sampling
 * across overlays shows the first word is 0xFFFFFFFF on some records and a
 * small integer on others, and the halfword at +0x04 is an entity slot in
 * rom_780898 but 0x5A in rom_77a7c8 -- so a single reading does not hold yet.
 * Left as bytes rather than guessed at.
 *
 * NOTE the 0x0C records OvlFunc_b8c walks in rom_7a37f0 -- id at +0x00 with -1
 * terminating, an axis flag at +0x06 -- may be this same structure seen from
 * another angle.  Not confirmed either way. */
typedef struct MapObject
{
    /* 0x00 */ u8 raw[0x0C];
} MapObject;

#endif /* GUARD_MAP_H */
