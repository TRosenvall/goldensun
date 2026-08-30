#include "dma.h"

typedef struct FX {
    /* 0x00 */ unsigned char *sprite;
    /* 0x04 */ int f04;
    /* 0x08 */ int f08;
    /* 0x0c */ int f0c;
    /* 0x10 */ int f10;
    /* 0x14 */ int f14;
    /* 0x18 */ int f18;
    /* 0x1c */ int f1c;
    /* 0x20 */ int f20;
    /* 0x24 */ int f24;
    /* 0x28 */ int f28;
    /* 0x2c */ int f2c;
    /* 0x30 */ unsigned char pad[0x11];
    /* 0x41 */ unsigned char f41;
    /* 0x42 */ unsigned char f42;
    /* 0x43 */ unsigned char f43;
    /* 0x44 */ unsigned char f44;
    /* 0x45 */ unsigned char f45;
    /* 0x46 */ unsigned char f46;
    /* 0x47 */ unsigned char f47;
} FX;

extern unsigned char *_CreateSprite(int id);
extern void Func_809ba5c(void *p, int x, int y);
extern void Func_809ba70(void *p, int n);
extern unsigned Random(void);

void Func_809ba90(FX *e, int id, int x, int y)
{
    unsigned char *s;
    int mask;

    DMA3_CLEAR(e, 0x48);
    s = _CreateSprite(id);
    e->sprite = s;
    if (s != 0) {
        mask = -13;
        s[9] = s[9] & mask;
    }
    Func_809ba5c(e, x, y);
    e->f20 = 0x80 << 10;
    e->f28 = 0x80 << 9;
    e->f24 = 0x80 << 9;
    e->f2c = 0x80 << 9;
    e->f14 = x;
    e->f18 = y;
    e->sprite[0x26] = 0;
    e->f41 = 1;
    e->f42 = 1;
    e->f43 = 1;
    e->f44 = 1;
    e->f45 = 1;
    e->f46 = Random();
    e->f47 = 4;
    Func_809ba70(e, 1);
}
