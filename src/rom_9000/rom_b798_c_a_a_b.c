/* Sprites: bind a part to its resource.
 *
 * Split out of asm/rom_9000/rom_b798_c_a_a.s; the _a and _c parts stay as
 * assembly and are listed around this one in stage1.ld, so the ROM layout is
 * unchanged.
 */
#include "gba/types.h"

struct SpritePart {
    /* 0x00 */ s16 id;          /* read SIGNED, and passed on as-is          */
    /* 0x02 */ u8 pad_02[2];
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 pad_05[2];
    /* 0x07 */ u8 unk_07;
    /* 0x08 */ void *pixels;
    /* 0x0C */ void *unk_0c;
    /* 0x10 */ u32 unk_10;
    /* 0x14 */ u8 unk_14;
    /* 0x15 */ u8 pad_15;
    /* 0x16 */ u8 unk_16;
};

struct SpriteInfo {
    /* 0x00 */ u8 kind;         /* zero means the header is empty            */
    /* 0x01 */ u8 pad_01[3];
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 pad_05[5];
    /* 0x0A */ u8 unk_0a;
    /* 0x0B */ u8 pad_0b;
    /* 0x0C */ void *pixels;    /* null means go and decompress it           */
    /* 0x10 */ void *unk_10;
};

extern struct SpriteInfo *_GetSpriteInfo(s32 id);
extern void *GetCachedSpriteGFX(s32 id);

/* Looks the part's resource header up by id, resolves the pixel data either
 * from the header or from the cache, and resets the animation state.
 *
 * No-op on a null part or an empty header. Note the two early returns are
 * separate tests, not one combined condition -- the header can only be read
 * after the lookup.
 */
void InitSpriteLayer(struct SpritePart *part)
{
    struct SpriteInfo *info;
    void *pixels;

    if (part == NULL)
        return;

    info = _GetSpriteInfo(part->id);
    if (info->kind == 0)
        return;

    pixels = info->pixels;
    if (pixels == NULL)
        pixels = GetCachedSpriteGFX(part->id);

    part->unk_04 = info->unk_04;
    part->unk_0c = info->unk_10;
    part->pixels = pixels;
    part->unk_07 = info->unk_0a;
    part->unk_16 = 0xff;
    part->unk_10 = 0;
    part->unk_14 = 0;
}
