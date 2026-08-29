struct A { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int f10; };

extern unsigned char iwram_3001ebc[];
extern struct A *__MapActor_GetActor(int slot);
extern void __ClearFlag(int id);
extern void __StopTask(void *fn);
extern void OvlFunc_939_2009240(void);
extern void OvlFunc_939_200918c(void);

void OvlFunc_939_20091d0(void)
{
    char *base;
    struct A *a;
    int z;

    base = *(char **)iwram_3001ebc;
    __ClearFlag(0x241);
    __ClearFlag(0x90 << 2);
    a = __MapActor_GetActor(0);
    if ((unsigned int)(a->f8 + 0xff97ffff) <= 0x87fffe) {
        z = a->f10;
        if (z > (0xa0 << 16)) {
            if (z < (0xf8 << 16)) {
                __StopTask(OvlFunc_939_2009240);
                *(unsigned short *)(base + (0xc1 << 1)) = 0x5b;
            }
        }
    }
    OvlFunc_939_200918c();
    __ClearFlag(0x91 << 2);
}
