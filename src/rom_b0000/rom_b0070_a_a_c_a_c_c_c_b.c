#include "dma.h"

extern unsigned char L_b3e80[] __asm__(".Lb3e80");
extern unsigned char *galloc_ewram(int index, int size);
extern void Func_80b06ec(int digit, unsigned char *buf, int slot);
extern int AllocSpriteSlot(void);
extern void UploadSpriteGFX(int slot, int size, unsigned char *gfx);
extern unsigned int _Func_801eadc(unsigned int a, unsigned int b, unsigned int c,
                                  unsigned int d, unsigned int e);
extern void gfree(int index);

int Func_80b0744(int value, unsigned int a, unsigned int b, unsigned int c)
{
    unsigned char *buf;
    int slot;
    unsigned int res;

    buf = galloc_ewram(0xe, 0x80 << 3);
    res = 0;
    DMA3_COPY(L_b3e80, buf, 0x100);
    Func_80b06ec(value % 10, buf, 0);
    value = value / 10;
    if (value != 0) {
        Func_80b06ec(value % 10, buf, 1);
        value = value / 10;
        if (value != 0) {
            Func_80b06ec(value % 10, buf, 2);
            value = value / 10;
            if (value != 0) {
                Func_80b06ec(value % 10, buf, 3);
                value = value / 10;
                if (value != 0) {
                    Func_80b06ec(value % 10, buf, 4);
                }
            }
        }
    }
    slot = AllocSpriteSlot();
    if (slot != 0x60) {
        UploadSpriteGFX(slot, 0x80 << 1, buf);
        res = _Func_801eadc(slot, 0x80008000, a, b, c);
    }
    gfree(0xe);
    return res;
}
