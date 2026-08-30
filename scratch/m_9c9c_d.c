extern unsigned char L6064[] __asm__(".L6064");
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);

void OvlFunc_924_2009c9c(void)
{
    unsigned char *e;
    unsigned char *p;
    unsigned int x;
    int y;
    int lo;
    int one;

    if (__GetFlag(0x256) != 0)
        return;
    x = *(short *)(__MapActor_GetActor(0) + 0xa);
    x -= 0xa4;
    y = *(short *)(__MapActor_GetActor(0) + 0x12);
    if (x > 7)
        return;
    lo = 0xba << 1;
    if (y < lo)
        return;
    if (y >= lo + 8)
        return;
    __CutsceneStart();
    __SetFlag(0x256);
    __CutsceneWait(5);
    e = __MapActor_GetActor(0);
    *(int *)(e + 0xc) += 0xfffe0000;
    p = __MapActor_GetActor(0);
    *(int *)(p + 0x3c) = *(int *)(__MapActor_GetActor(0) + 0xc);
    one = 1;
    __CopyMapTiles(6, 0x1d, 0xa, 0x17, one, one);
    __PlaySound(0xd9);
    __Func_8010560(L6064, 0xa, 0x12);
    __CutsceneEnd();
}
