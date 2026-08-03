/* Func_80ad608 @ 0x080ad608 -- asm/rom_a1000/rom_ad274_c_a_c.s
 *
 * Blocker class 2 at scale: the ROM holds FOUR values across its calls in a
 * particular arrangement and no formulation here reproduces it. Three were
 * tried and each lands somewhere different:
 *
 *   state->sprites[slot] written out three times   39 instructions (rom 33)
 *   the base cached in a local, indexed three times 37, and it spills a
 *                                                   SECOND high register
 *                                                   (r8 and r10 vs the ROM's
 *                                                   r8 alone)
 *   a `void **` pointer to the slot, dereferenced   29 -- four instructions
 *                                                   SHORTER than the ROM
 *
 * The ROM's arrangement is base in r7, the byte offset in r6, the resource
 * index in r5 (later reused for the new sprite) and the animation in r8 --
 * with every access through `[r7, r6]` register-offset addressing. The
 * pointer form collapses base and offset into one register and comes out
 * shorter; the struct forms keep them apart but carry an extra live value.
 *
 * Worth noting the last of the three is SHORTER than the target, which is the
 * same signature seen in Func_8092504: when a reconstruction is shorter than
 * the ROM, the likely reading is that the original held something this one
 * does not, rather than that the C is merely arranged wrongly.
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
    void *sprite = state->sprites[slot];

    if (sprite != NULL) {
        _DeleteSprite(sprite);
        state->sprites[slot] = NULL;
    }
    sprite = _CreateSprite(Data_80af304[index]);
    if (sprite != NULL)
        _Sprite_SetAnim(sprite, anim);
    state->sprites[slot] = sprite;
    return 1;
}
