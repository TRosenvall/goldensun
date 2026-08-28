#include "gba/types.h"
#include "dma.h"

struct Act {
    u8 pad00[0x18];
    int f18;
    int f1c;
    u16 f20;
};

extern u16 *gL12c0 __asm__(".L12c0");
extern u16 *gL12c4 __asm__(".L12c4");
extern u16 *gL12c8 __asm__(".L12c8");
extern u8 gL111c[] __asm__(".L111c");
extern u8 ewram_2001000[];
extern u8 *iwram_3001ebc;

extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern int __GetFlag(int id);
extern void OvlFunc_916_2008a90(u16 *p);
extern int OvlFunc_916_2008b3c(u8 *p, int n);
extern void OvlFunc_916_2008194(void);
extern void __MapActor_SetAnim(int a, int b);
extern u8 *__MapActor_GetActor(int a);
extern void OvlFunc_916_2008e64(int a);

int OvlFunc_916_2008980(void)
{
    int z;
    int w;
    u8 *q;
    struct Act *r;
    u8 *b;

    gL12c4 = (u16 *)ewram_2001000;
    gL12c8 = (u16 *)(ewram_2001000 + 2);
    w = 0x40;
    gL12c0 = (u16 *)(ewram_2001000 + 4);
    z = 0;
    __Func_80105d4(0x20, 0, 0x40, 0x20, z, w);
    __Func_8010704(0, 0, 0x20, 0x20, z, w);
    __Func_8010704(0x20, 0, 0x20, 0x20, z, 0x20);
    if (__GetFlag(0x109) == 0) {
        DMA3_COPY(gL111c, gL12c0, 0x48);
        *gL12c4 = z;
        *gL12c8 = 1;
    }
    OvlFunc_916_2008a90(gL12c0);
    OvlFunc_916_2008b3c(gL111c, 0xff);
    OvlFunc_916_2008194();
    __MapActor_SetAnim(9, 0);
    q = __MapActor_GetActor(9);
    q[0x55] = z;
    r = (struct Act *)__MapActor_GetActor(0xa);
    r->f20 = 8;
    r->f18 = 0xc000;
    r->f1c = 0xc000;
    b = iwram_3001ebc + 0x1c0;
    *(int *)b = 0x204;
    if (__GetFlag(0x845) == 0)
        OvlFunc_916_2008e64(4);
    return 0;
}
