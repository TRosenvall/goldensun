extern unsigned char iwram_3001ebc[];
extern void *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8091e9c(int n);

void OvlFunc_963_2008288(void)
{
    char *base;
    short v;
    unsigned char *p;
    int two;

    base = *(char **)iwram_3001ebc;
    v = *(short *)(base + (0xb6 << 1));
    p = (unsigned char *)__MapActor_GetActor(0) + 0x55;
    *p = 0;
    __PlaySound(0x9e);
    two = 2;
    __CopyMapTiles(0x42, 0x24, 0x47, 8, two, two);
    __WaitFrames(4);
    __CopyMapTiles(0x44, 0x24, 0x47, 8, two, two);
    __WaitFrames(4);
    __Func_8092208(0, 3, -0x10);
    __Func_8091e9c(v);
}
