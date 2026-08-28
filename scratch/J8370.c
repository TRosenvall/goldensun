#include "gba/types.h"
#include "actor.h"

struct Cmd {
    int f0;
    int f4;
    unsigned char pad08[0x18 - 0x08];
    short f18;
    unsigned char pad1a[0x1c - 0x1a];
    void *f1c;
    unsigned char pad20[0x28 - 0x20];
};

extern int iwram_3001e40;
extern struct Actor *__MapActor_GetActor(int slot);
extern unsigned char L1740[] __asm__(".L1740");
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct Cmd *p);

void OvlFunc_928_2008370(void)
{
    struct Cmd s;
    struct Actor *a;

    a = __MapActor_GetActor(0xe);
    if ((iwram_3001e40 & 3) == 0) {
        s.f0 = 1;
        s.f4 = 9;
        s.f18 = 0xa9;
        s.f1c = L1740;
        OvlFunc_common0_10c(a->pos.x, a->pos.y, a->pos.z + 0xffff0000, 0,
                            0xffff0000, 0xffff0000, 0xcc << 14, &s);
    }
}
