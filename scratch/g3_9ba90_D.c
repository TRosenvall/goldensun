#include "dma.h"

extern unsigned char *_CreateSprite(int id);
extern void Func_809ba5c(void *p, int x, int y);
extern void Func_809ba70(void *p, int n);
extern unsigned Random(void);

void Func_809ba90(unsigned char *e, int id, int x, int y)
{
    unsigned char *s;
    unsigned char *q;
    int mask;

    DMA3_CLEAR(e, 0x48);
    s = _CreateSprite(id);
    *(unsigned char **)e = s;
    if (s != 0) {
        mask = -13;
        s[9] = s[9] & mask;
    }
    Func_809ba5c(e, x, y);
    *(int *)(e + 0x20) = 0x80 << 10;
    *(int *)(e + 0x28) = 0x80 << 9;
    *(int *)(e + 0x2c) = 0x80 << 9;
    *(int *)(e + 0x24) = 0x80 << 9;
    *(int *)(e + 0x14) = x;
    *(int *)(e + 0x18) = y;
    *(unsigned char *)(*(int *)e + 0x26) = 0;
    e[0x41] = 1;
    e[0x42] = 1;
    e[0x43] = 1;
    e[0x44] = 1;
    e[0x45] = 1;
    e[0x46] = Random();
    e[0x47] = 4;
    Func_809ba70(e, 1);
}
