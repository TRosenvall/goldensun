#include "gba/types.h"

struct SpritePart {
    /* 0x00 */ s16 id;
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
    /* 0x17 */ u8 pad_17;
};

struct SpriteInfo {
    /* 0x00 */ u8 kind;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 pad_05;
    /* 0x06 */ u8 unk_06;
    /* 0x07 */ u8 unk_07;
    /* 0x08 */ u8 pad_08[2];
    /* 0x0A */ u8 unk_0a;
    /* 0x0B */ u8 pad_0b;
    /* 0x0C */ void *pixels;
    /* 0x10 */ void *unk_10;
};

extern struct SpriteInfo *_GetSpriteInfo(s32 id);
extern void *GetCachedSpriteGFX(s32 id);

int InitSprite(unsigned char *a)
{
    unsigned char *cnt;
    struct SpritePart **pp;
    struct SpritePart *part;
    struct SpriteInfo *info;
    void *pixels;
    int i;
    unsigned char *b;

    cnt = a + 0x27;
    i = 0;
    if (i < *cnt) {
        pp = (struct SpritePart **)(a + 0x28);
        do {
        part = *pp++;
        info = _GetSpriteInfo(part->id);
        if (info->kind != 0) {
            if (i == 0) {
                b = a;
                b[0x20] = info->kind;
                b[0x21] = info->unk_01;
                *(int *)(b + 0x18) = info->unk_02 << 8;
                b[0x23] = info->unk_07;
                b[0x22] = info->unk_06;
            }
            pixels = info->pixels;
            if (pixels == 0)
                pixels = GetCachedSpriteGFX(part->id);
            part->unk_04 = info->unk_04;
            part->unk_0c = info->unk_10;
            part->pixels = pixels;
            part->unk_07 = info->unk_0a;
            part->unk_16 = 0xff;
            part->unk_10 = 0;
            part->unk_14 = 0;
            }
            i++;
        } while (i < *cnt);
    }
    return 0;
}
