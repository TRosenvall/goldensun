/* Sprite_AddLayer -- 0x0800b8ac, asm/rom_9000/rom_b798_c_a_a_c_a.s.
 *
 * Finds the first free part slot on a sprite host, creates a part there, and on
 * the FIRST part copies the sprite's dimensions and correction offsets out of
 * its info record.
 *
 * WHOLE-FILE CONVERSION -- one function in the .s, no split, no Makefile rule,
 * no pins. tools/datacheck.py confirms no data section.
 *
 * TWO LEVERS, and both are the file-mate habit rather than anything clever:
 *
 *   1. THE SCAN VARIABLE AND THE CREATED PART ARE THE SAME VARIABLE. The ROM
 *      loads each slot into r5 and later puts CreateSpriteLayer's result in r5
 *      too. Testing the slot without naming it -- `if (s->parts[i] == NULL)` --
 *      gives the scan a scratch register and the part a different callee-saved
 *      one, rotating three registers for 25 differing of 70. Naming it
 *      (`part = s->parts[i]; if (part == NULL) break;`) is 25 -> 2.
 *   2. USE THE TREE'S OWN STRUCTS. The last two were the ROM loading
 *      `info->unk_02` one slot earlier than we did, and no spelling of the
 *      statement moved it. src/rom_9000/rom_b798_c_a_a_a.c -- a file-mate --
 *      already defines `struct SpriteHost`, `struct SpriteInfo` and
 *      `struct SpritePart` with real field names and types, including
 *      `u16 unk_02` where my hand-rolled version had `*(u16 *)(info + 2)`.
 *      Adopting them is exact.
 *
 * THAT SECOND ONE IS THE GENERAL RESULT: a hand-rolled struct of `u8 padNN[]`
 * and casts reproduces the OFFSETS but not the TYPES, and gcc schedules a typed
 * field load differently from a cast dereference. Check docs/structs.md and the
 * file-mates for an existing definition before inventing offsets -- CLAUDE.md
 * already says to, and this is what it buys.
 *
 * EXACT: 144 bytes, 70 encodings, 2 relocations, measured three times, clean on
 * tools/tryc.py with no warning.
 */
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

struct SpriteHost {
    /* 0x00 */ u8 pad_00[0x18];
    /* 0x18 */ s32 depth;
    /* 0x1c */ u8 pad_1c[4];
    /* 0x20 */ u8 width;
    /* 0x21 */ u8 height;
    /* 0x22 */ u8 corrX;
    /* 0x23 */ u8 corrY;
    /* 0x24 */ u8 pad_24[3];
    /* 0x27 */ u8 count;
    /* 0x28 */ struct SpritePart *parts[4];
};

extern struct SpritePart *CreateSpriteLayer(s32 id);
extern struct SpriteInfo *_GetSpriteInfo(s32 id);

struct SpritePart *Sprite_AddLayer(struct SpriteHost *s, s32 id)
{
    struct SpritePart *part;
    struct SpriteInfo *info;
    int i;

    for (i = 0; i < 4; i++) {
        part = s->parts[i];
        if (part == NULL)
            break;
    }
    if (i == 4)
        return (struct SpritePart *)-1;
    part = CreateSpriteLayer(id);
    if (part == NULL)
        return NULL;
    s->parts[i] = part;
    info = _GetSpriteInfo(id);
    if (s->count == 0) {
        s->width = info->kind;
        s->height = info->unk_01;
        s->depth = info->unk_02 << 8;
        s->corrY = info->unk_07;
        s->corrX = info->unk_06;
    }
    if (i == s->count)
        s->count = i + 1;
    return part;
}
