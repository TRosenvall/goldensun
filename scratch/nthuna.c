#include "dma.h"

extern void *galloc_ewram(int kind, int size);
extern unsigned char *iwram_3001ed0;
extern void Func_8090a5c(int a, void *b, void *c, int d);
extern void Func_809088c(void *a, void *b, void *c, int d);
extern void StartTask(void *f, int n);
extern void Task_Thunder(void);

void StartThunder2(int first, int b)
{
    int a;
    unsigned char *buf;
    unsigned char *y;
    unsigned char *p;

    a = first;
    buf = galloc_ewram(0x1e, 0x1f88);
    y = iwram_3001ed0;
    DMA3_CLEAR(buf, 0x1f88);
    Func_8090a5c(a, y, buf, 1);
    p = buf + (0xa8 << 4);
    Func_8090a5c(b, y, p, 1);
    Func_809088c(p, buf, buf + (0xa8 << 5), 0xc);
    Func_8090a5c((int)buf, 0, y + (0xe0 << 4), 1);
    *(short *)(buf + (0xfc << 5)) = 0x78;
    *(short *)(buf + 0x1f82) = 0;
    StartTask(Task_Thunder, 0xc8 << 4);
}
