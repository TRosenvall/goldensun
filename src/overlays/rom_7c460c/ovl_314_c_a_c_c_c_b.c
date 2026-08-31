extern unsigned char iwram_3001ebc[];
extern unsigned char gState[];

struct A { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int f10; };

extern struct A *__MapActor_GetActor(int slot);
extern void __StopTask(void *fn);
extern void OvlFunc_939_2009240(void);

void OvlFunc_939_2009240(void)
{
    char *base;
    struct A *a;
    unsigned char *g;
    int z;
    int v;

    v = 0x5b;
    base = *(char **)iwram_3001ebc;
    a = __MapActor_GetActor(0);
    g = gState;
    if (*(short *)(g + (0x93 << 2)) == 0) {
        if ((unsigned int)(a->f8 + 0xff700000) <= (0x80 << 14)) {
            z = a->f10;
            if (z >= (0xa8 << 16)) {
                if (z < (0xb0 << 16)) {
                    __StopTask(OvlFunc_939_2009240);
                    *(unsigned short *)(base + (0xc1 << 1)) = v;
                }
            }
        }
    }
}
