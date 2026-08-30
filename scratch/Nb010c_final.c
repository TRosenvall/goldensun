#include "dma.h"

extern unsigned char Lb3940[] __asm__(".Lb3940");
extern unsigned char Lb39c0[] __asm__(".Lb39c0");
extern unsigned char Lb3a40[] __asm__(".Lb3a40");
extern unsigned char Lb3ac0[] __asm__(".Lb3ac0");
extern unsigned char Lb3b40[] __asm__(".Lb3b40");
extern unsigned char Lb3bc0[] __asm__(".Lb3bc0");

extern unsigned char *galloc_iwram(int tag, int size);
extern void _Func_808e118(void);
extern int _Func_80796c4(unsigned char *p);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, unsigned char *gfx);
extern int StartTask(void *f, int prio);
extern void Func_80b00f4(void);

void Func_80b010c(void)
{
    unsigned char *p;
    int t;

    p = galloc_iwram(0x37, 0xa7 << 4);
    _Func_808e118();
    DMA3_CLEAR(p, 0xa7 << 4);
    p[0xea << 2] = 0xc;
    p[0x3a7] = _Func_80796c4(p + 0x36e);
    t = AllocSpriteSlot();
    *(short *)(p + (0xe4 << 2)) = t;
    UploadSpriteGFX(t, 0x80, Lb3940);
    t = AllocSpriteSlot();
    *(short *)(p + 0x392) = t;
    UploadSpriteGFX(t, 0x80, Lb3b40);
    t = AllocSpriteSlot();
    *(short *)(p + (0xe5 << 2)) = t;
    UploadSpriteGFX(t, 0x80, Lb3bc0);
    t = AllocSpriteSlot();
    *(short *)(p + 0x396) = t;
    UploadSpriteGFX(t, 0x80, Lb39c0);
    t = AllocSpriteSlot();
    *(short *)(p + 0x39a) = t;
    UploadSpriteGFX(t, 0x80, Lb3a40);
    t = AllocSpriteSlot();
    *(short *)(p + (0xe6 << 2)) = t;
    UploadSpriteGFX(t, 0x80, Lb3ac0);
    StartTask(Func_80b00f4, 0xc8 << 4);
}
