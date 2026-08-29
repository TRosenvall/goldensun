extern unsigned char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

int OvlFunc_904_2008054(void)
{
    unsigned char *base;
    unsigned char *a;
    int *p;
    unsigned int off;
    int x, y;
    int e1, f1;
    int z;

    x = 0xd8 << 16;
    y = 0x88 << 16;
    base = iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    p = (int *)(base + off);
    off += 0x44;
    *p = off;
    off -= 0x3c;
    p = (int *)(base + off);
    off = 0x18;
    *p = off;
    if (__GetFlag(0xc0 << 2) != 0) {
        __MapActor_SetPos(8, x, y);
        __MapActor_SetAnim(8, 2);
        a = __MapActor_GetActor(8);
        __Actor_SetSpriteFlags(a, 0);
        a = __MapActor_GetActor(8);
        a += 0x23;
        *a = 2;
        a = __MapActor_GetActor(8);
        z = 0;
        a += 0x59;
        *a = z;
        e1 = 0xb;
        f1 = 6;
        __Func_8010704(0xb, 0x24, 5, 5, e1, f1);
    }
    return 0;
}
