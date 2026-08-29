#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"
#include "actor.h"

extern char *iwram_3001f30;
extern void __Func_8092adc(int a, int b, int c);
extern void __Actor_SetColorswap(struct Actor *a, int b);
extern void __CutsceneWait(int n);
extern struct Actor *__CreateActor(int kind, fx32 x, fx32 y, fx32 z);

void OvlFunc_957_2008c98(void)
{
    char *p;
    struct Actor *a;
    struct Actor *n;

    p = iwram_3001f30;
    a = *(struct Actor **)(p + 0x10);
    __Func_8092adc(*(short *)(p + 0x18), 0x80 << 7, 0);
    __Actor_SetColorswap(a, 0);
    __CutsceneWait(0x14);
    n = __CreateActor(0, a->pos.x, a->pos.y, a->pos.z);
    if (n != 0) {
        DMA3_COPY(a, n, 0x70);
        a->update = 0;
        *(struct Actor **)(p + 0x10) = n;
        a->drawKind = 0;
    }
}
