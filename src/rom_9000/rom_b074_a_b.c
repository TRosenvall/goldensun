/* Func_800b074  --  0x0800b074, split out of asm/rom_9000/rom_b074_a.s; UpdateSprite and
 * Func_800b388 stay in _c.s. Matched from scratch.
 *
 * COPY A DERIVED VALUE INTO ITS OWN LOCAL: `x >>= 16` in place kept x live across
 * the function, so gcc moved it out of r1 at entry and every later allocation
 * shifted; `x16 = x >> 16` lets the argument die in r1 (92 -> 2 aligned). The
 * last 2 were `signed char` -- gba/types.h's s8 is unsigned here.
 */
#include "gba/types.h"

struct OamSprite {
    unsigned char pad[4];
    unsigned int y:8;
    unsigned int affineMode:2;
    unsigned int objMode:2;
    unsigned int mosaic:1;
    unsigned int bpp:1;
    unsigned int shape:2;
    unsigned int x:9;
    unsigned int matrixNum:5;
    unsigned int size:2;
    unsigned int tileNum:10;
    unsigned int priority:2;
    unsigned int paletteNum:4;
};

struct SpritePair {
    struct OamSprite main;
    struct OamSprite shadow;
    u8 pad18[8];
    u8 width;
    u8 height;
    u8 pad22;
    signed char yoff;
};

void Func_800b074(struct SpritePair *o, int x, int bz, int oz, int cbz, int *scale)
{
    int hw = o->width >> 1;
    int hh = o->height >> 1;
    int offx = 8;
    int offy = 4;
    int mode = 1;
    int sx, sy;
    int px, py, x16;
    struct OamSprite *s;

    sx = *scale++;
    sy = *scale;
    if (sx > 0x10000 || sy > 0x10000) {
        mode = 3;
        offx = 0x10;
        offy = 8;
        hw <<= 1;
        hh <<= 1;
    }
    x16 = x >> 16;
    px = x16 - hw;
    py = ((oz - bz) >> 16) - hh - ((((o->height >> 1) - o->yoff) * sy + 0xffff) >> 16);
    o->main.affineMode = mode;
    o->main.x = px;
    o->main.y = py;
    px = x16 - offx;
    py = ((oz - cbz) >> 16) - offy;
    s = &o->shadow;
    s->affineMode = mode;
    s->x = px;
    s->y = py;
}
