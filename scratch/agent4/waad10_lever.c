#include "gba/types.h"
#include "dma.h"

typedef void (*CopyFn)(void *dst, void *src, u32 len);
typedef void (*FillFn)(void *dst, u32 len, u32 value);

extern void Func_8001af8(void *dst, void *src, u32 len);
extern void Func_80008d8(void *dst, u32 len, u32 value);
extern unsigned char *iwram_3001f2c;
extern void Func_80a10d0(void *w, int a, int b, int c, int d, int e);
extern void WaitFrames(int n);
extern void _Func_8021a18(void *p);
extern unsigned char Data_af26c[];
extern void *GetSpritePalette(void);
extern void Func_80aac84(int n);
extern int Func_80aafb8(void *s);

int Func_80aad10(void)
{
    unsigned char *p;
    unsigned char *s;
    CopyFn copy;
    FillFn fill;
    vu16 *q;
    void *v1;
    void *v2;
    void *v3;
    void *v4;
    u32 n1;
    u32 n2;
    u32 n3;
    u32 n4;

    p = iwram_3001f2c;
    s = *(unsigned char **)(p + 0x184);
    Func_80a10d0(p + 0x30, 0, 5, 0x1e, 0xf, 2);
    WaitFrames(1);
    copy = Func_8001af8;
    v1 = (void *)0x6004000;
    n1 = 0x80 << 6;
    copy(s + 0xa8, v1, n1);
    v2 = (void *)0x5000080;
    n2 = 0x80;
    copy(s + 0x20a8, v2, n2);
    fill = Func_80008d8;
    v3 = (void *)0x6004000;
    n3 = 0x80 << 6;
    fill(v3, n3, 0x33333333);
    v4 = (void *)0x5000080;
    n4 = 0x80;
    fill(v4, n4, 0x55555555);
    _Func_8021a18((void *)0x6005000);
    copy((void *)0x60052c0, Data_af26c, 0x20);
    DMA3_COPY16(GetSpritePalette(), (void *)0x50000a0, 0x40);
    q = (vu16 *)0x50001e8;
    *(vu16 *)0x50000bc = *q;
    DMA3_COPY16((void *)0x50001e0, (void *)0x50000e0, 0x40);
    Func_80aac84(8);
    *(vu16 *)0x50000e8 = *q;
    *(vu16 *)0x50000c8 = *q;
    return Func_80aafb8(s);
}
