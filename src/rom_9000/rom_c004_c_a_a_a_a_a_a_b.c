#include "dma.h"

extern void DeleteSprite(void *s);

void DeleteActor(unsigned char *e)
{
    void **list;
    void *s;
    int i;

    if (e == 0)
        return;
    switch (*(unsigned char *)(e + 0x54) & 0xf) {
    case 1:
        DeleteSprite(*(void **)(e + 0x50));
        break;
    case 2:
        list = *(void ***)(e + 0x50);
        for (i = 3; i >= 0; i--) {
            s = *list++;
            if (s != 0)
                DeleteSprite(s);
        }
        break;
    }
    DMA3_CLEAR(e, 0x70);
}
