/* Func_809ba90  --  0x0809ba90, was asm/rom_8a000/rom_9b698_c_c_a.s (this
 * function alone), so it converts whole. Matched from scratch.
 *
 * - The sprite's priority bits are a BITFIELD (lo:2, prio:2, hi:4): that gives
 *   the ROM's `mov #0xd / neg / and`, where `&= ~0xc` on a u8 gives `#0xf3`.
 * - A CHAR-TYPED STORE AS AN ORDERING CONSTRAINT. `*((u8 *)t + 0x26) = 0` has
 *   alias set 0, so it cannot pass the int stores around it; the same store as
 *   `t->f26` floats past them. The ROM needs it kept below two int stores while
 *   the pointer load stays early enough to hold local-alloc's r2/r3 choice, and
 *   only the alias-set-0 spelling in this statement order (found by exhaustive
 *   permutation) does both -- 76 of 76, against 70 for the struct field.
 */
#include "dma.h"

struct Sprite {
    u8 pad00[9];
    u8 lo:2;
    u8 prio:2;
    u8 hi:4;
};

struct Eff {
    struct Sprite *spr;
    u8 pad04[0x10];
    int f14;
    int f18;
    u8 pad1c[4];
    int f20;
    int f24;
    int f28;
    int f2c;
    u8 pad30[0x11];
    u8 f41;
    u8 f42;
    u8 f43;
    u8 f44;
    u8 f45;
    u8 f46;
    u8 f47;
};

extern struct Sprite *_CreateSprite(int id);
extern void Func_809ba5c(struct Eff *e, int x, int y);
extern void Func_809ba70(struct Eff *e, int n);
extern int Random(void);

void Func_809ba90(struct Eff *e, int id, int x, int y)
{
    struct Sprite *s, *t;

    DMA3_CLEAR(e, 0x48);
    s = _CreateSprite(id);
    e->spr = s;
    if (s != 0)
        s->prio = 0;
    Func_809ba5c(e, x, y);
    e->f20 = 0x20000;
    e->f28 = 0x10000;
    e->f2c = 0x10000;
    e->f14 = x;
    e->f18 = y;
    t = e->spr;
    e->f24 = 0x10000;
    *((u8 *)t + 0x26) = 0;
    e->f41 = 1;
    e->f42 = 1;
    e->f43 = 1;
    e->f44 = 1;
    e->f45 = 1;
    e->f46 = Random();
    e->f47 = 4;
    Func_809ba70(e, 1);
}
