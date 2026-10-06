/* Func_80ad608 @ 0x080ad608  --  *** BYTE-EXACT.  NOT A PARK. ***
 *
 * MATCHING, 0 differing encodings of 35.  objcmp.py under the production flags:
 *   OK Func_80ad608 -- 80 bytes, 35 encodings and 5 relocations identical
 * and with --whole, which is the figure that counts because a --func run cannot
 * see section-tail alignment fill:
 *   OK whole file -- 80 bytes, 35 encodings and 5 relocations identical
 * SIZE EXACT (80 against 80).  COUNT EXACT.  RELOCATIONS EXACT (5 against 5,
 * same symbols at the same offsets).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_a1000/rom_ad274_c_a_c.c \
 *     asm/rom_a1000/rom_ad274_c_a_c.s --func Func_80ad608
 *
 * SPLIT SHAPE: NONE NEEDED.  datacheck.py on asm/rom_a1000/rom_ad274_c_a_c.s
 * reports nothing (no data sections, no exports to promote) and the file holds
 * exactly ONE `.thumb_func_start`, so this .c replaces the whole TU.  No
 * linker-script edit, no `.global` pre-pass.
 *
 * PINS: 0.  No inline asm, no `register ... __asm__`, no per-file flag group.
 * RESIDUE: none.
 *
 * WHAT THE PARK HAD WRONG (src/non_matching/rom_a1000/rom_ad608.c, retired by
 * this landing).  Its header claimed a figure LARGER than the function, which
 * is the eighteen-park class batch 330 brief C diagnosed: the N came off
 * objcmp's ENCODINGS line (stream length, 32-bit entries and pads INCLUDED) and
 * the M off its INSTRUCTION COUNT line (16-bit encodings only).  Done exactly,
 * the reference is 30 sixteen-bit encodings + 3 `bl` + 2 literal pool words = 33
 * instructions in 80 bytes, and the park body was 34 + 3 + 2 = 37.  So the gap
 * was FOUR INSTRUCTIONS of length and none of it was pool or padding -- the pool
 * words and the pad count were identical on both sides.
 *
 * Its stated blocker -- "the ROM holds FOUR values across its calls in a
 * particular arrangement and no formulation here reproduces it" -- is REFUTED.
 * All three of its recorded negatives varied the ADDRESSING of
 * `state->sprites[slot]` (written out three times / cached in a local / reached
 * through a `void **`).  The dimension none of them varied is HOW MANY NAMES THE
 * TWO ASSIGNMENTS SHARE, which is the batch-330 `Func_801a910` lesson.
 *
 * THE MECHANISM.  With one name for both the destroyed sprite and its
 * replacement, gcc has ONE pseudo set twice, and that pseudo is live across
 * `_CreateSprite` and `_Sprite_SetAnim`, so it must be call-saved: it takes r5.
 * r4 is NOT available for it -- the tree builds with `-fcall-used-r4`
 * (Makefile:131-134), so r4 cannot hold a value across a call -- which leaves
 * r5/r6/r7 for the four crossing values and pushes `index` to r8 and `anim` to
 * sl.  Two high registers cost a `mov`+`push` pair and a `pop`+`mov` pair, and
 * the load into a call-saved r5 then needs `mov r0, r5` before `_DeleteSprite`.
 *
 * With two names the old sprite is dead the moment `_DeleteSprite` is entered,
 * so it lives in r0 and needs no move; r5 is then free to hold `index` across
 * the first call and be REUSED for the new sprite afterwards, exactly as the
 * park's own reading of the ROM described ("the resource index in r5, later
 * reused for the new sprite"); and only `anim` needs a high register, which is
 * the ROM's single r8.  All four instructions go at once.
 *
 * The park's note that one probe came out four instructions SHORTER than the
 * ROM, read as evidence "the original held something this one does not", was a
 * false signal: it was shorter because the `void **` form collapses base and
 * offset, not because anything was missing.
 */
#include "gba/types.h"

struct FieldState {
    u8 pad_0[0x224];
    void *sprites[0x40];
};

extern struct FieldState *iwram_3001f2c;
extern void *Data_80af304[] __asm__(".Laf304");
extern void _DeleteSprite(void *sprite);
extern void *_CreateSprite(void *resource);
extern void _Sprite_SetAnim(void *sprite, s32 anim);

/* Destroys whatever is in the slot, creates a replacement from the resource
 * table and starts it on the given animation. Returns 1 even when creation
 * fails -- the slot is simply left null, and callers do not check.
 */
s32 Func_80ad608(s32 slot, s32 index, s32 anim)
{
    struct FieldState *state = iwram_3001f2c;
    void *old = state->sprites[slot];
    void *sprite;

    if (old != NULL) {
        _DeleteSprite(old);
        state->sprites[slot] = NULL;
    }
    sprite = _CreateSprite(Data_80af304[index]);
    if (sprite != NULL)
        _Sprite_SetAnim(sprite, anim);
    state->sprites[slot] = sprite;
    return 1;
}
